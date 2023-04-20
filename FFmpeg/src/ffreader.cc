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

}

ClipInfo FFReader::GetClipInfo() const {
    return ClipInfo();
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
    decode.m_sw_frame       = av_frame_alloc();
    decode.m_read_pkt       = av_packet_alloc();
    return true;
    return false;
}

bool FFReader::OpenCodec(ffdecode *decode) {
    
    int ret{-1};
    AVCodecContext* codec_ctx{nullptr};
    
    do 
    {
#if LIBAVFORMAT_VERSION_INT >= AV_VERSION_INT(57, 40, 101)
        codec_ctx = avcodec_alloc_context3(decode->m_codec);
		ret = avcodec_parameters_to_context(codec_ctx, decode->m_stream->codecpar);
		if (ret < 0)
		{
			break;
		}
#else
        
#endif      
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

}