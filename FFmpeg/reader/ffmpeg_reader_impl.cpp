#include "ffmpeg_reader_impl.h"
using namespace std;

FFmpegReader::CEXFFmpegReader::CEXFFmpegReader()
    :m_exit(false),
     m_cur_pos(0),
     m_seek_pos(0),
     m_maxche_size(50),
     m_seek(false),
     m_reset(false),
     m_eof(false),
     m_curdecode(nullptr),
     m_sws(false),
     m_swr(false),
     m_seek_pts(0)
{
    memset(&m_vdecode, 0, sizeof(mp_decode));
    memset(&m_adecode, 0, sizeof(mp_decode));
    memset(&m_mediainfo,    0, sizeof(media_params));
    memset(&m_dst_frameinfo, 0, sizeof(ReaderParam));
}

FFmpegReader::CEXFFmpegReader::~CEXFFmpegReader()
{
    m_exit = true;
    m_cache_condi.notify_one();
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

    m_mediainfo.m_hasvideo = InitDecoder(AVMEDIA_TYPE_VIDEO);
    m_mediainfo.m_hasaudio = InitDecoder(AVMEDIA_TYPE_AUDIO);
    
    ParaseMediaInfo();

    if (!m_mediainfo.m_hasaudio && !m_mediainfo.m_hasvideo)
    {
        return false;
    }
    else
    {
        return true;
    }
    
}

bool FFmpegReader::CEXFFmpegReader::SetFrameParam(const ReaderParam& dataparams)
{
    memcpy(&m_dst_frameinfo, &dataparams, sizeof(dataparams));
    m_vdecode.m_need = m_dst_frameinfo.m_type == MetaDataType::kVideo;
    m_adecode.m_need = m_dst_frameinfo.m_type == MetaDataType::kAudio;
    if (m_vdecode.m_need)
    {
        m_curdecode = &m_vdecode;
    }
    else if (m_adecode.m_need)
    {
        m_curdecode = &m_adecode;
    }
    
    AVPixelFormat dst_pixfmt = GetPixfmt();
    if ( (m_mediainfo.m_width != m_dst_frameinfo.m_width
          || m_mediainfo.m_height != m_dst_frameinfo.m_height
          || m_mediainfo.m_pixfmt != dst_pixfmt
         ) && m_vdecode.m_need )
    {
        sws_freeContext(m_vdecode.m_sws_ctx);
        m_vdecode.m_sws_ctx = sws_getContext(m_mediainfo.m_width, m_mediainfo.m_height, static_cast<AVPixelFormat>(m_mediainfo.m_pixfmt),
                                            m_dst_frameinfo.m_width, m_dst_frameinfo.m_height, dst_pixfmt,
                                             SWS_FAST_BILINEAR, nullptr, nullptr, nullptr
                                            );
         m_sws = true;
    }

    if (  (m_mediainfo.m_samplerate != m_dst_frameinfo.m_samplerate
          || AV_SAMPLE_FMT_S16 != m_adecode.m_decode_ctx->sample_fmt
          )&& m_adecode.m_need )

    {

        m_adecode.m_swr_ctx = swr_alloc_set_opts(m_adecode.m_swr_ctx, m_adecode.m_decode_ctx->channel_layout, AV_SAMPLE_FMT_S16, m_dst_frameinfo.m_samplerate,
                                                 m_adecode.m_decode_ctx->channel_layout, m_adecode.m_decode_ctx->sample_fmt, m_adecode.m_decode_ctx->sample_rate,
                                                 0, nullptr
                                                 );
        m_swr = true;
    }

    if (nullptr != m_curdecode)
    {
        m_exit = true;
        if (m_prepare_th.joinable())
        {
            m_prepare_th.join();
            
        }
        m_exit = false;
        SeekFile(0);

		std::thread prepare_th(&CEXFFmpegReader::ThreadFunc, this);
		m_prepare_th = std::move(prepare_th);
        return true;
    }
    else
    {
        return false;
    }
}

int FFmpegReader::CEXFFmpegReader::GetVideoFrame(FrameInfo& frame, int frameindex)
{
    if (frameindex > m_mediainfo.m_nb_frames || frameindex < 0)
    {
        return -1;
    }

    std::map<int, AVFrame*>::iterator search;
    
    do 
    {
        std::unique_lock<std::mutex> video_cache_lock(m_vcache_mux);
		search = m_frame_cache.find(frameindex);
		if (search != m_frame_cache.end())
		{
            break;
		}

        if (m_maxche_size == m_frame_cache.size())
        {
			std::unique_lock<std::mutex> seek_lock(m_status_mux);
			m_seek_pos = frameindex;
			m_seek = true;
            m_cache_condi.notify_one();
            std::cout << "seek pos -- " << m_seek_pos << std::endl;
        }
        
		m_cache_condi.wait(video_cache_lock, [this,&search, frameindex] { return !m_seek && (search = m_frame_cache.find(frameindex)) != m_frame_cache.end(); });
        
    } while (false);

    AVFrame* cur_frame = search->second;
    memcpy(frame.data, cur_frame->data, sizeof(cur_frame->data));
    frame.pts = cur_frame->pts;
    return 0;
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

    AVStream* stream = decode->m_stream = m_filefmt_ctx->streams[ret];
	enum AVCodecID codec_id = stream->codecpar->codec_id;

    if (AV_CODEC_ID_VP8 == codec_id || AV_CODEC_ID_VP9 == codec_id) {
		AVDictionaryEntry* tag = NULL;
		tag = av_dict_get(stream->metadata, "alpha_mode", tag,
			AV_DICT_IGNORE_SUFFIX);

		if (tag && strcmp(tag->value, "1") == 0) {
			const char* codecname = (codec_id == AV_CODEC_ID_VP8) ? "libvpx" : "libvpx-vp9";
            decode->m_codec = avcodec_find_decoder_by_name(codecname);
		}
	}
    else
    {
        decode->m_codec = avcodec_find_decoder(codec_id);
    }

    if (nullptr == decode->m_codec)
    {
        return false;
    }

    if (!OpenCodec(decode))
    {
        return false;
    }
    
    decode->m_decode_frame =  av_frame_alloc();
    decode->m_sw_frame = av_frame_alloc();
    decode->m_read_pkt = av_packet_alloc();
    return true;
}

bool FFmpegReader::CEXFFmpegReader::OpenCodec(mp_decode* decode)
{
    AVCodecContext* codec_ctx(nullptr);
    int ret(-1);

    codec_ctx = avcodec_alloc_context3(decode->m_codec);
	if (nullptr == codec_ctx) {	
		return false;
	}

    do 
    {
		ret = avcodec_parameters_to_context(codec_ctx, decode->m_stream->codecpar);
		if (ret < 0)
		{
			break;
		}
        
        ret = avcodec_open2(codec_ctx, decode->m_codec, nullptr);
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

    decode->m_decode_ctx = codec_ctx;

    return true;
}

void FFmpegReader::CEXFFmpegReader::ParaseMediaInfo()
{
    m_mediainfo.m_width = m_vdecode.m_stream->codecpar->width;
    m_mediainfo.m_height = m_vdecode.m_stream->codecpar->height;
    m_mediainfo.m_pixfmt = m_vdecode.m_decode_ctx->pix_fmt;
    strcpy_s(m_mediainfo.m_pixfmt_name, av_get_pix_fmt_name(static_cast<AVPixelFormat>(m_mediainfo.m_pixfmt)));
    m_mediainfo.m_gop_size = std::max(m_vdecode.m_decode_ctx->gop_size, 1);
    m_mediainfo.m_nb_frames = m_vdecode.m_stream->nb_frames;
    if (0 == m_mediainfo.m_nb_frames)
    {
        int64_t duration_second = m_vdecode.m_stream->duration * av_q2d(m_vdecode.m_stream->time_base);
        m_mediainfo.m_nb_frames = static_cast<int64_t>(duration_second * av_q2d (m_vdecode.m_stream->avg_frame_rate));
    }

    m_mediainfo.m_samplerate = m_adecode.m_stream->codecpar->sample_rate;
    m_mediainfo.m_audio_depth = m_adecode.m_stream->codecpar->bits_per_coded_sample;
    m_mediainfo.m_samplefmt = m_adecode.m_decode_ctx->sample_fmt;
    strcpy_s(m_mediainfo.m_samplefmt_name, av_get_sample_fmt_name(m_adecode.m_decode_ctx->sample_fmt));
    
    strcpy_s(m_mediainfo.m_vcodec_name, m_vdecode.m_codec->long_name);
    strcpy_s(m_mediainfo.m_acodec_name, m_adecode.m_codec->long_name);

    m_mediainfo.m_total_sec = m_filefmt_ctx->duration/AV_TIME_BASE;
    
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
	if (nullptr != decode->m_decode_ctx)
	{
		avcodec_free_context(&decode->m_decode_ctx);
	}

	if (nullptr != decode->m_sw_frame)
	{
        av_frame_unref(decode->m_sw_frame);
		av_frame_free(&decode->m_sw_frame);
	}

	if (nullptr != decode->m_decode_frame)
	{
		av_frame_unref(decode->m_decode_frame);
		av_frame_free(&decode->m_decode_frame);
	}

    if (nullptr != decode->m_read_pkt)
    {
        av_packet_unref(decode->m_read_pkt);
        av_packet_free(&decode->m_read_pkt);
    }

	//if (nullptr != decode->m_decode_ctx)
	{
		avcodec_free_context(&decode->m_decode_ctx);
	}

    //if (nullptr != decode->m_sws_ctx)
    {
        sws_freeContext(decode->m_sws_ctx);
        decode->m_sws_ctx = nullptr;
    }

    //if (nullptr != decode->m_swr_ctx)
    {
        swr_free(&decode->m_swr_ctx);
        decode->m_swr_ctx = nullptr;
    }

    return;
}



void FFmpegReader::CEXFFmpegReader::FlushDecoder()
{
    avcodec_flush_buffers(m_vdecode.m_decode_ctx);
    avcodec_flush_buffers(m_adecode.m_decode_ctx);
	m_vdecode.m_eof = m_adecode.m_eof = false;
    m_vdecode.m_frame_ready = m_adecode.m_frame_ready = false;
}

void FFmpegReader::CEXFFmpegReader::FlushFrameCache()
{
    for (auto frame : m_frame_cache)
    {
        if (m_sws || m_swr)
        {
             av_free(*frame.second->data);
        }
        else
        {
            av_frame_unref(frame.second);
        }
       
        av_frame_free(&frame.second);
    }
    m_frame_cache.clear();
    
}

void FFmpegReader::CEXFFmpegReader::ThreadFunc()
{
    while (!m_exit)
    {
        {
            std::unique_lock<std::mutex> statue_lock(m_status_mux);
            if (m_seek)
            {               
                SeekFile(m_seek_pos);
            }
            if (m_reset)
            {
                SeekFile(0);
                m_reset = false;
            }
        }
        PrepareFrame();
        MoveFrameToCache();
    }

    return;
}

int FFmpegReader::CEXFFmpegReader::SeekFile(int seek_pos)
{
    float seek_percent = seek_pos/static_cast<float>(m_mediainfo.m_nb_frames);
    int64_t seek_to = seek_percent * m_filefmt_ctx->duration; 
    AVRational time_base ={ 1, AV_TIME_BASE};
	m_seek_pts = av_rescale_q(seek_to, time_base, m_curdecode->m_stream->time_base);
    int rec = av_seek_frame(m_filefmt_ctx, m_curdecode->m_stream->index, m_seek_pts, AVSEEK_FLAG_BACKWARD);
    if (rec < 0)
    {
        return rec;
    }
    
    FlushDecoder();
    FlushFrameCache();
    m_cur_pos = seek_pos;
    m_seek = false;
    m_status_condi.notify_one();
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
		if (m_vdecode.m_need && m_mediainfo.m_hasvideo && read_pkt.stream_index == m_vdecode.m_stream->index)
		{
            av_packet_unref(m_vdecode.m_read_pkt);
            av_packet_move_ref(m_vdecode.m_read_pkt, &read_pkt);
            m_vdecode.m_packet_ready = true;
		}
		else if (m_adecode.m_need && m_mediainfo.m_hasaudio && read_pkt.stream_index == m_adecode.m_stream->index)
		{
            av_packet_unref(m_adecode.m_read_pkt);
            av_packet_move_ref(m_adecode.m_read_pkt, &read_pkt);
            m_adecode.m_packet_ready = true;
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
                m_vdecode.m_eof = m_adecode.m_eof = m_eof = true;
                break;
            }
        }

         rec = DecodePacket();
         if (0 == rec || m_curdecode->m_eof)
         {
             return true;
         }
    }

    return false;
}

int FFmpegReader::CEXFFmpegReader::DecodePacket()
{
    int rec = avcodec_receive_frame(m_curdecode->m_decode_ctx, m_curdecode->m_decode_frame);
	if (rec != 0 && rec != AVERROR(EAGAIN)) {
		if (AVERROR_EOF == rec)
		{
            m_curdecode->m_eof = true;
		}
		return rec;
	}

    if (!m_curdecode->m_packet_ready)
    {
        return rec;
    }


    rec = avcodec_send_packet(m_curdecode->m_decode_ctx, m_curdecode->m_read_pkt);
    if (rec != 0)
    {
        return rec;
    }

    rec = avcodec_receive_frame(m_curdecode->m_decode_ctx, m_curdecode->m_decode_frame);
	if (rec != 0)
	{
		return rec;
	}

    m_curdecode->m_frame_ready = true;
    m_curdecode->m_packet_ready = false;
    return 0;
}

bool FFmpegReader::CEXFFmpegReader::CheckContinueDecode()
{
    if (m_mediainfo.m_hasvideo && !m_eof && !m_vdecode.m_frame_ready)
    {
        return true;
    }

    if (m_mediainfo.m_hasaudio && !m_eof && !m_vdecode.m_frame_ready)
    {
        return true;
    }

    return false;
}

void FFmpegReader::CEXFFmpegReader::MoveFrameToCache()
{
    if (!m_curdecode->m_frame_ready )
    {
        return ;
    }

    m_curdecode->m_frame_ready = false;
    AVFrame* deframe = m_curdecode->m_decode_frame;
    if (deframe->pts < m_seek_pts)
    {
        av_frame_unref(deframe);
        return;
    }

    AVFrame* useframe = av_frame_alloc();
    std::cout<<"pts:"<<deframe->pts<<std::endl;
    if (m_sws)
    {
        AVPixelFormat dst_pixfmt = GetPixfmt();
        av_image_alloc(useframe->data, useframe->linesize, m_dst_frameinfo.m_width, m_dst_frameinfo.m_height, dst_pixfmt, 32);

        sws_scale(m_curdecode->m_sws_ctx, deframe->data, deframe->linesize, 0, deframe->height,
                  useframe->data, useframe->linesize);

        useframe->pts = deframe->pts;
        useframe->width = m_dst_frameinfo.m_width;
        useframe->height = m_dst_frameinfo.m_height;
        av_frame_unref(deframe);
    }
    else if (m_swr)
    {
		int dst_frames = (int)av_rescale_rnd(deframe->nb_samples, m_dst_frameinfo.m_samplerate,
			                                 m_curdecode->m_decode_ctx->sample_rate,
			                                 AV_ROUND_UP);

		av_samples_alloc(useframe->data, nullptr, m_curdecode->m_decode_ctx->channels,
			dst_frames, AV_SAMPLE_FMT_S16, 0);

		swr_convert(m_curdecode->m_swr_ctx, useframe->data, dst_frames, (const uint8_t**)deframe->data, deframe->nb_samples);

		useframe->pts = deframe->pts;
		av_frame_unref(deframe);
    }
    else
    {
        av_frame_move_ref(useframe, deframe);
    }


    {
		std::unique_lock<std::mutex> vcache_lock(m_vcache_mux);
		m_cache_condi.wait(vcache_lock, [this] {return m_frame_cache.size() < m_maxche_size || m_exit || m_seek; });
		m_frame_cache.insert(pair<int, AVFrame*>(m_cur_pos++, useframe));
    }
   
    m_cache_condi.notify_one();
    return;
}

AVPixelFormat FFmpegReader::CEXFFmpegReader::GetPixfmt()
{
    switch (m_dst_frameinfo.m_pixfmt)
    {
    case PixFmt::kRGB:
        return AV_PIX_FMT_RGB24;
    case PixFmt::kYUVP:
        return AV_PIX_FMT_YUV444P;
    case PixFmt::kYUV420P:
        return AV_PIX_FMT_YUV420P;
    case PixFmt::kYUV422:
        return AV_PIX_FMT_YUYV422;
    case PixFmt::kYUV422P:
        return AV_PIX_FMT_YUV422P;
    default:
        break;
    }

    return AV_PIX_FMT_NONE;
}

FFmpegReader::FFmpegReader() :m_impl_reader(std::make_unique<CEXFFmpegReader>()) {}
FFmpegReader::~FFmpegReader() {}
bool FFmpegReader::InitAVFmt(const std::string& filename) {return m_impl_reader->InitAVFmt(filename);}
MediaInfo FFmpegReader::GetMediaInfo() {return m_impl_reader->GetMediaInfo();}

bool FFmpegReader::SetFrameParam(const ReaderParam& params)
{
    return m_impl_reader->SetFrameParam(params);
}

int  FFmpegReader::GetVideoFrame(FrameInfo& frame, int frameindex) {return m_impl_reader->GetVideoFrame(frame, frameindex);};
void FFmpegReader::Reset() {return m_impl_reader->Reset();}