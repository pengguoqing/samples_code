#include "ffreader.h"

namespace mediaio {

FFReader::ffdecode::ffdecode()
 :m_stream(nullptr),
  m_decode_ctx(nullptr),
  m_codec(nullptr),
  m_sw_frame(nullptr),
  m_decode_frame(nullptr),
  m_read_pkt(nullptr),
  m_sws_ctx(nullptr),
  m_swr_ctx(nullptr),
  m_frame_ready(false),
  m_packet_ready(false),
  m_file_eof(false),
  m_need(false)
  {}

bool FFReader::OpenClipFile(std::string filepath) {

    CloseFile();

    const AVInputFormat* format = av_find_input_format(filepath.c_str());
    
    m_filefmt_ctx = avformat_alloc_context();
	int ret = avformat_open_input(&m_filefmt_ctx, filepath.c_str(), format, nullptr);
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

void FFReader::CloseClipFile() {
    CloseFile();
}

ClipInfo FFReader::GetClipInfo() const {
    return ClipInfo();
}

void FFReader::SetOutputFmt(const ClipOutputFmt &info)
{
    m_dst_frameinfo = info; 
}

bool FFReader::GetSourceV(VideoSource *frame, uint64_t pos)
{
    return false;
}

bool FFReader::GetSourceA(AudioSource *frame, uint64_t pos)
{
    return false;
}

void FFReader::ReleaseFrame(uint64_t pos)
{
}

bool FFReader::InitDecoder(AVMediaType type) {

    ffdecode& decode  = AVMEDIA_TYPE_VIDEO==type ? m_vdecode : m_adecode;
	
    int ret = av_find_best_stream(m_filefmt_ctx, type, -1, -1, nullptr, 0);
	if (AVERROR_STREAM_NOT_FOUND == ret)
	{
        return false;
	}

    AVStream* stream = decode.m_stream = m_filefmt_ctx->streams[ret];
	enum AVCodecID codec_id = stream->codecpar->codec_id;

    if (AV_CODEC_ID_VP8 == codec_id || AV_CODEC_ID_VP9 == codec_id) {
		AVDictionaryEntry* tag = NULL;
		tag = av_dict_get(stream->metadata, "alpha_mode", tag,
			AV_DICT_IGNORE_SUFFIX);

		if (tag && strcmp(tag->value, "1") == 0) {
			const char* codecname = (codec_id == AV_CODEC_ID_VP8) ? "libvpx" : "libvpx-vp9";
            decode.m_codec = avcodec_find_decoder_by_name(codecname);
		}
	}
    else
    {
        decode.m_codec = avcodec_find_decoder(codec_id);
    }

    if (nullptr == decode.m_codec)
    {
        return false;
    }

    if (!OpenCodec(&decode))
    {
        return false;
    }
    
    decode.m_decode_frame   =  av_frame_alloc();
    decode.m_sw_frame       =  av_frame_alloc();
    decode.m_read_pkt       =  av_packet_alloc();
    return true;
    return false;
}

bool FFReader::InitDecoder(AVMediaType type)
{
    return false;
}


bool FFReader::OpenCodec(ffdecode *decode)
{

    AVCodecContext* codec_ctx = avcodec_alloc_context3(decode->m_codec);
	if (nullptr == codec_ctx) {	
		return false;
	}

    int ret{-1};
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
void FFReader::ParaseMediaInfo() {

    m_mediainfo.m_width      = m_vdecode.m_stream->codecpar->width;
    m_mediainfo.m_height     = m_vdecode.m_stream->codecpar->height;
   
    m_mediainfo.m_pixfmtname = av_get_pix_fmt_name(m_vdecode.m_decode_ctx->pix_fmt);
    m_mediainfo.m_gop_size   = std::max(m_vdecode.m_decode_ctx->gop_size, 1);
    m_mediainfo.m_nb_frames  = m_vdecode.m_stream->nb_frames;
    if (0 == m_mediainfo.m_nb_frames)
    {
        int64_t duration_second = m_vdecode.m_stream->duration * av_q2d(m_vdecode.m_stream->time_base);
        m_mediainfo.m_nb_frames = static_cast<int64_t>(duration_second * av_q2d (m_vdecode.m_stream->avg_frame_rate));
    }

    m_mediainfo.m_samplerate  = m_adecode.m_stream->codecpar->sample_rate;
    m_mediainfo.m_audio_depth = m_adecode.m_stream->codecpar->bits_per_coded_sample;
    m_mediainfo.m_samplefmt   = m_adecode.m_decode_ctx->sample_fmt;
    
    m_mediainfo.m_vcodecname  = m_vdecode.m_codec->long_name;
    m_mediainfo.m_acodecname  = m_adecode.m_codec->long_name;

    m_mediainfo.m_duration    = m_filefmt_ctx->duration/AV_TIME_BASE;
    
    return;

}

void FFReader::ThreadFunc() {

}

int FFReader::SeekFile(uint64_t seek_pos) {
    return 0;
}

int FFReader::ReadNextPacket() {
    return 0;
}

bool FFReader::PrepareFrame() {
    return false;
}

int FFReader::DecodePacket() {
    return 0;
}

bool FFReader::CheckContinueDecode() {
    return false;
}

void FFReader::MoveFrameToCache() {
}

AVPixelFormat FFReader::GetAVPixFmt() {
    return AVPixelFormat();
}

void FFReader::CloseFile() {
    ClearDecoderRes(&m_vdecode);
    ClearDecoderRes(&m_adecode);
    FlushFrameCache();
    avformat_close_input(&m_filefmt_ctx);

}

void FFReader::ClearDecoderRes(ffdecode *decode) {

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

}

void FFReader::FlushDecoder() {
    avcodec_flush_buffers(m_vdecode.m_decode_ctx);
    avcodec_flush_buffers(m_adecode.m_decode_ctx);
	m_vdecode.m_file_eof = m_adecode.m_file_eof = false;
    m_vdecode.m_frame_ready = m_adecode.m_frame_ready = false;
}

void FFReader::FlushFrameCache() {
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


}