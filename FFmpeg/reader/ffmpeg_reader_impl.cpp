#include "ffmpeg_reader_impl.h"
using namespace std;

FFmpegReader::CEXFFmpegReader::CEXFFmpegReader()
    :m_read_mode(ReadMode::kSeek),
     m_exit(false),
     m_cur_pos(0),
     m_seek_pos(0),
     m_maxche_size(10),
     m_seek(false),
     m_reset(false),
     m_raw_data(false),
     m_hasvideo(false),
     m_hasaudio(false),
     m_eof(false)
{
    memset(&m_vdecode, 0, sizeof(mp_decode));
    memset(&m_adecode, 0, sizeof(mp_decode));
    memset(&m_mediainfo,    0, sizeof(media_params));
}

FFmpegReader::CEXFFmpegReader::~CEXFFmpegReader()
{
    m_exit = true;
    m_vcache_condi.notify_one();
    if (m_prepare_th.joinable())
    {
        m_prepare_th.join();
    }
    
    CloseFile();
}

void FFmpegReader::CEXFFmpegReader::Reset()
{
    std::unique_lock<std::mutex> reset_lock(m_status_mux);
    m_reset = true;

    return;
}

bool FFmpegReader::CEXFFmpegReader::InitAVFmt(const std::string& filename)
{
    CloseFile();

    AVInputFormat* format = av_find_input_format(filename.c_str());
    
    m_filefmt_ctx = avformat_alloc_context();
	int ret = avformat_open_input(&m_filefmt_ctx, filename.c_str(), format, nullptr);
    if (ret<0)
    {
        return false;
    }

    ret = avformat_find_stream_info(m_filefmt_ctx, nullptr);
    if (ret < 0)
    {
        return false;
    }

    m_hasvideo = InitDecoder(AVMEDIA_TYPE_VIDEO);
    InitDecoder(AVMEDIA_TYPE_AUDIO);
    
    ParaseMediaInfo();

    std::thread prepare_th(&CEXFFmpegReader::ThreadFunc, this);
    m_prepare_th = std::move(prepare_th);

    return false;
}

int FFmpegReader::CEXFFmpegReader::GetVideoFrame(std::vector<uint8_t*>& framedata, int frameindex)
{
    if (frameindex>m_mediainfo.nb_frames || frameindex<0)
    {
        return -1;
    }

    std::map<int, AVFrame*>::iterator search;
    do 
    {
		std::unique_lock<std::mutex> video_cache_lock(m_vcache_mux);
		search = m_vframe_cache.find(frameindex);
		if (search != m_vframe_cache.end())
		{
            break;
		}

		{
			std::unique_lock<std::mutex> seek_lock(m_status_mux);
            m_cur_pos = m_seek_pos = frameindex;
			m_seek = true;
		}

		m_vcache_condi.wait(video_cache_lock, [this, frameindex, &search] { return m_vframe_cache.end() != (search = m_vframe_cache.find(frameindex)); });
        
    } while (false);

    uint8_t** arr_data = search->second->data;
    for (; nullptr != *arr_data; arr_data++)
    {
        framedata.push_back(*arr_data);
    }
    
    return 0;
}

void FFmpegReader::CEXFFmpegReader::SetReadMode(ReadMode mode)
{
    m_read_mode = mode;
    return;
}

MediaInfo FFmpegReader::CEXFFmpegReader::GetMediaInfo() const
{
    return m_mediainfo;
}

bool FFmpegReader::CEXFFmpegReader::InitDecoder(enum AVMediaType type)
{
    mp_decode* decode = AVMEDIA_TYPE_VIDEO==type ? &m_vdecode : &m_adecode;
	
    int ret = av_find_best_stream(m_filefmt_ctx, type, -1, -1, nullptr, 0);
	if (AVERROR_STREAM_NOT_FOUND == ret)
	{
        return false;
	}

    AVStream* stream = decode->stream = m_filefmt_ctx->streams[ret];
	enum AVCodecID codec_id = stream->codecpar->codec_id;

    if (AV_CODEC_ID_VP8 == codec_id || AV_CODEC_ID_VP9 == codec_id) {
		AVDictionaryEntry* tag = NULL;
		tag = av_dict_get(stream->metadata, "alpha_mode", tag,
			AV_DICT_IGNORE_SUFFIX);

		if (tag && strcmp(tag->value, "1") == 0) {
			const char* codecname = (codec_id == AV_CODEC_ID_VP8) ? "libvpx" : "libvpx-vp9";
            decode->codec = avcodec_find_decoder_by_name(codecname);
		}
	}
    else
    {
        decode->codec = avcodec_find_decoder(codec_id);
    }

    if (nullptr == decode->codec)
    {
        return false;
    }

    if (!OpenCodec(decode))
    {
        return false;
    }
    
    decode->decode_frame =  av_frame_alloc();
    decode->sw_frame = av_frame_alloc();
    decode->read_pkt = av_packet_alloc();
    return true;
}

bool FFmpegReader::CEXFFmpegReader::OpenCodec(mp_decode* decode)
{
    AVCodecContext* codec_ctx(nullptr);
    int ret(-1);

    codec_ctx = avcodec_alloc_context3(decode->codec);
	if (nullptr == codec_ctx) {	
		return false;
	}

    do 
    {
		ret = avcodec_parameters_to_context(codec_ctx, decode->stream->codecpar);
		if (ret < 0)
		{
			break;
		}
        
        ret = avcodec_open2(codec_ctx, decode->codec, nullptr);
        if (ret < 0)
        {
            break;
        }

    } while (false);
	
	if (ret < 0)
	{
        avcodec_free_context(&codec_ctx);
        return false;
	}

    decode->decoder = codec_ctx;

    return true;
}

void FFmpegReader::CEXFFmpegReader::ParaseMediaInfo()
{
    m_mediainfo.width = m_vdecode.stream->codecpar->width;
    m_mediainfo.height = m_vdecode.stream->codecpar->height;
    m_mediainfo.pixfmt = m_vdecode.decoder->pix_fmt;
    strcpy_s(m_mediainfo.pix_name, av_get_pix_fmt_name(static_cast<AVPixelFormat>(m_mediainfo.pixfmt)));
    m_mediainfo.nb_frames = m_vdecode.stream->nb_frames;
    m_mediainfo.gop_size = std::max(m_vdecode.decoder->gop_size, 1);
    if (0 == m_mediainfo.nb_frames)
    {
        int64_t duration_second = m_vdecode.stream->duration * av_q2d(m_vdecode.stream->time_base);
        m_mediainfo.nb_frames = static_cast<int64_t>(duration_second * av_q2d (m_vdecode.stream->avg_frame_rate));
    }

    m_mediainfo.audio_samplerate = m_adecode.stream->codecpar->sample_rate;
    
    return;
}

void FFmpegReader::CEXFFmpegReader::CloseFile()
{
    ClearDecoderRes(&m_vdecode);
    ClearDecoderRes(&m_adecode);
    FlushFrameCache();
    memset(&m_adecode, 0, sizeof(mp_decode));
    memset(&m_vdecode, 0, sizeof(mp_decode));
    
    avformat_close_input(&m_filefmt_ctx);
}

void FFmpegReader::CEXFFmpegReader::ClearDecoderRes(mp_decode* decode)
{
	if (nullptr != decode->decoder)
	{
		avcodec_free_context(&decode->decoder);
	}

	if (nullptr != decode->sw_frame)
	{
        av_frame_unref(decode->sw_frame);
		av_frame_free(&decode->sw_frame);
	}

	if (nullptr != decode->decode_frame)
	{
		av_frame_unref(decode->decode_frame);
		av_frame_free(&decode->decode_frame);
	}

    if (nullptr != decode->read_pkt)
    {
        av_packet_unref(decode->read_pkt);
        av_packet_free(&decode->read_pkt);
    }

	if (nullptr != decode->decoder)
	{
		avcodec_free_context(&decode->decoder);
	}

    return;
}



void FFmpegReader::CEXFFmpegReader::FlushDecoder()
{
	/*for (auto vpkt = m_vpacket_cache.front(); !m_vpacket_cache.empty();)
	{
		av_packet_unref(vpkt);
	}

	for (auto apkt = m_apacket_cache.front(); !m_apacket_cache.empty();)
	{
		av_packet_unref(apkt);
	}*/


    avcodec_flush_buffers(m_vdecode.decoder);
    avcodec_flush_buffers(m_adecode.decoder);
	m_vdecode.eof = m_adecode.eof = false;
    m_vdecode.frame_ready = m_adecode.frame_ready = false;
}

void FFmpegReader::CEXFFmpegReader::FlushFrameCache()
{
    for (auto frame : m_vframe_cache)
    {
        av_frame_unref(frame.second);
        av_frame_free(&frame.second);
    }
    m_vframe_cache.clear();
    
}

void FFmpegReader::CEXFFmpegReader::ThreadFunc()
{
    while (!m_exit)
    {
        {
            std::unique_lock<std::mutex> statue_lock(m_status_mux);
            if (m_seek)
            {
                m_seek =false;
                SeekFile(m_seek_pos);
            }
            if (m_reset)
            {
                m_reset = false;
                SeekFile(0);
            }
        }

        PrepareFrame();
        PushFrameToCache();
    }

    return;
}

int FFmpegReader::CEXFFmpegReader::SeekFile(int seek_pos)
{
    int rec = av_seek_frame(m_filefmt_ctx, 0, seek_pos, AVSEEK_FLAG_FRAME);
    if (rec < 0)
    {
        return rec;
    }

    FlushDecoder();
    FlushFrameCache();
    return 0;
}

int FFmpegReader::CEXFFmpegReader::ReadNextPacket()
{
    AVPacket read_pkt;
	int ret = av_read_frame(m_filefmt_ctx, &read_pkt);
	if (ret < 0) 
    {
       return ret;
	}

	if (read_pkt.size > 0)
	{
		if (m_hasvideo && read_pkt.stream_index == m_vdecode.stream->index)
		{
            av_packet_move_ref(m_vdecode.read_pkt, &read_pkt);
            m_vdecode.packet_reafy = true;
		}
		else if (m_hasaudio && read_pkt.stream_index == m_adecode.stream->index)
		{
            av_packet_move_ref(m_adecode.read_pkt, &read_pkt);
            m_adecode.packet_reafy = true;
		}
	}
    
    av_packet_unref(&read_pkt);
	return ret;
}

bool FFmpegReader::CEXFFmpegReader::PrepareFrame()
{
    int rec = 0;
    while (CheckContinueDecode())
    {
        if (!m_eof)
        {
            rec = ReadNextPacket();
            if (AVERROR_EOF == rec)
            {
                m_eof = true;
                break;
            }
        }

         rec = DecodePacket();
         if (0 == rec || m_vdecode.eof)
         {
             return true;
         }
    }

    return false;
}

int FFmpegReader::CEXFFmpegReader::DecodePacket()
{
    
    int rec = avcodec_receive_frame(m_vdecode.decoder, m_vdecode.decode_frame);
	if (rec != 0 && rec != AVERROR(EAGAIN)) {
		if (AVERROR_EOF == rec)
		{
            m_vdecode.eof = true;
		}
		return rec;
	}

    if (!m_vdecode.packet_reafy)
    {
        return rec;
    }


    rec = avcodec_send_packet(m_vdecode.decoder, m_vdecode.read_pkt);
    if (rec != 0)
    {
        return rec;
    }

    rec = avcodec_receive_frame(m_vdecode.decoder, m_vdecode.decode_frame);
	if (rec != 0)
	{
		return rec;
	}

    m_vdecode.frame_ready = true;
    m_vdecode.packet_reafy = false;
    return 0;
}

bool FFmpegReader::CEXFFmpegReader::CheckContinueDecode()
{
    if (m_hasvideo && !m_eof && !m_vdecode.frame_ready)
    {
        return true;
    }

    return false;
}

void FFmpegReader::CEXFFmpegReader::PushFrameToCache()
{
    if (!m_vdecode.frame_ready)
    {
        return ;
    }

    m_vdecode.frame_ready = false;
    AVFrame* frame = av_frame_alloc();
    av_frame_move_ref(frame, m_vdecode.decode_frame);

    {
		std::unique_lock<std::mutex> vcache_lock(m_vcache_mux);
		m_vcache_condi.wait(vcache_lock, [this] {return m_vframe_cache.size() < m_maxche_size || m_exit; });
		m_vframe_cache.insert(pair<int, AVFrame*>(m_cur_pos++, frame));
    }
   
    m_vcache_condi.notify_one();
    return;
}

FFmpegReader::FFmpegReader() :m_impl_reader(std::make_unique<CEXFFmpegReader>()) {}
FFmpegReader::~FFmpegReader() {}
bool FFmpegReader::InitAVFmt(const std::string& filename) {return m_impl_reader->InitAVFmt(filename);}
MediaInfo FFmpegReader::GetMediaInfo() {return m_impl_reader->GetMediaInfo();}

void FFmpegReader::SetFrameParam(PixFmt pixfmt, int width, int height)
{
    return;
}
int  FFmpegReader::ReadVideoFrame(std::vector<uint8_t*>& framedata, int frameindex) {return m_impl_reader->GetVideoFrame(framedata, frameindex);};
void FFmpegReader::Reset() {return;}