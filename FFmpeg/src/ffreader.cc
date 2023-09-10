#include "ffreader.h"

namespace mediaio {

  FFReader::~FFReader() {
    CloseFile();
    }

  bool FFReader::OpenClipFile(const std::string& filepath, SoureType metatype) {
      
      CloseFile();

      const AVInputFormat *format = av_find_input_format(filepath.c_str());

      m_filefmt_ctx = avformat_alloc_context();
      int ret = avformat_open_input(&m_filefmt_ctx, filepath.c_str(), const_cast<AVInputFormat*>(format), nullptr);
      if (ret < 0) {
          return false;
      }

      ret = avformat_find_stream_info(m_filefmt_ctx, nullptr);
      if (ret < 0) {
          return false;
      }

      m_mediainfo.m_hasvideo = InitDecoder(AVMEDIA_TYPE_VIDEO);
      m_mediainfo.m_hasaudio = InitDecoder(AVMEDIA_TYPE_AUDIO);

      ParaseMediaInfo();
    
      m_read_type = metatype;
      return StartRead();
      
    }

    void FFReader::CloseClipFile() {
        CloseFile();
    }   

    ClipInfo FFReader::GetClipInfo() const {
        return m_mediainfo;
    }

    bool FFReader::GetSourceData(uint64_t pos, AVSoucreData* frame) {

        if (pos >= m_mediainfo.m_nb_frames){
            return false;
        }
        
        if (pos<m_available_pos || pos > m_decode_pos) {
                SeekToFrameNum(pos);
                m_cache_condi.notify_one();
        }

        {
             const AVFrame* crrespond_frame{nullptr};
             std::unique_lock<std::mutex> cache_lock(m_cache_mux);

             if ( m_decode_pos == pos ) {      
                m_expect_new_frame.store(true, std::memory_order_relaxed);
                m_cache_condi.notify_one();
             }

             std::map<uint64_t, std::shared_ptr<AVFrame>>::iterator dstframe;                    
             m_cache_condi.wait(cache_lock, [this, pos, &dstframe]{ dstframe = m_frame_cache.find(pos); return m_frame_cache.end() != dstframe ;});
             crrespond_frame = dstframe->second.get();       
           
             for(int i{0}; i < kMaxAVPlanes; i++){
                    frame->m_data[i]     = crrespond_frame->data[i];
                    frame->m_duration    = crrespond_frame->pkt_duration * av_q2d(m_curvalid_decode->m_stream->time_base);
                    frame->m_linesize[i] = crrespond_frame->linesize[i];
                    frame->m_pts         = av_q2d(m_curvalid_decode->m_stream->time_base) * crrespond_frame->pts;
                }
        }
       
        return true;
    }

    bool FFReader::SeekToFrameNum(uint64_t seek_pos) {
        if (seek_pos >= m_mediainfo.m_nb_frames) {
            return false;
        }
    
         //wait when workthread is seeking
        while (m_seek.load(std::memory_order_relaxed)){         
        }

        m_seek_pos = seek_pos;
        m_seek.store(true, std::memory_order_relaxed);

        return 0;
    }

    void FFReader::SeekFile() {

        FlushDecoder();
        FlushFrameCache();
    
        float seek_percent { m_seek_pos/static_cast<float>(m_mediainfo.m_nb_frames) };
        int64_t seek_to    { static_cast<int64_t> (seek_percent * m_filefmt_ctx->duration) }; 
        AVRational time_base {1, AV_TIME_BASE};
	    m_seek_pts = av_rescale_q(seek_to, time_base, m_curvalid_decode->m_stream->time_base);
        av_seek_frame(m_filefmt_ctx, m_curvalid_decode->m_stream->index, m_seek_pts, AVSEEK_FLAG_BACKWARD);
        m_decode_pos = m_available_pos = m_seek_pos;  
        m_curvalid_decode->m_code_eof = m_curvalid_decode->m_pkt_eof = m_curvalid_decode->m_frame_ready = m_curvalid_decode->m_packet_ready = false;
    }

    bool FFReader::InitDecoder(AVMediaType type) {

        ffdecode& decode  = AVMEDIA_TYPE_VIDEO==type ? m_vdecode : m_adecode;
    
        int ret = av_find_best_stream(m_filefmt_ctx, type, -1, -1, nullptr, 0);
	    if (AVERROR_STREAM_NOT_FOUND == ret) {
            return false;
	    }

        const AVStream* stream = decode.m_stream = m_filefmt_ctx->streams[ret];
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
        else{
            decode.m_codec = avcodec_find_decoder(codec_id);
        }

        if (nullptr == decode.m_codec) {
            return false;
        }

        if (!OpenCodec(&decode)) {
            return false;
        }

        decode.m_decode_frame   =  av_frame_alloc();
        decode.m_sw_frame       =  av_frame_alloc();
        decode.m_read_pkt       =  av_packet_alloc();

        return true;
    }   

    bool FFReader::StartRead() {

        if(SoureType::kSourceTypeA==m_read_type && m_mediainfo.m_hasaudio) {
                  m_read_type = SoureType::kSourceTypeA;
                  m_curvalid_decode = &m_adecode;
              }else if(SoureType::kSourceTypeV==m_read_type && m_mediainfo.m_hasvideo) {
                  m_read_type = SoureType::kSourceTypeV;
                  m_curvalid_decode = &m_vdecode;
              }else {
                  m_read_type = SoureType::kSourceTypeUnknow;
                  return false;
        }
        
        if (m_curvalid_decode){
			std::thread read_th(&FFReader::ThreadFunc, this);
			m_read_th = std::move(read_th);
			m_read_th.detach();
            m_runing.store(true, std::memory_order_relaxed);
            return true;
        }
     
        return false;
    }

    bool FFReader::OpenCodec(ffdecode *decode) {
        AVCodecContext* codec_ctx = avcodec_alloc_context3(decode->m_codec);
	    if (nullptr == codec_ctx) {	
	    	return false;
	    }

        int ret{-1};
        do {
		    ret = avcodec_parameters_to_context(codec_ctx, decode->m_stream->codecpar);
		    if (ret < 0) {
			break;
		    }
        
            ret = avcodec_open2(codec_ctx, decode->m_codec, nullptr);
            if (ret < 0) {
                break;
            }

        } while (false);
	
	    if (ret < 0) {
            avcodec_free_context(&codec_ctx);
            return false;
	    }

        decode->m_decode_ctx = codec_ctx;

        return true;
    }
    
    void FFReader::ParaseMediaInfo() {

        if (m_vdecode.m_stream){
			m_mediainfo.m_width = m_vdecode.m_stream->codecpar->width;
			m_mediainfo.m_height = m_vdecode.m_stream->codecpar->height;

			m_mediainfo.m_pixfmt_name = av_get_pix_fmt_name(m_vdecode.m_decode_ctx->pix_fmt);
			m_mediainfo.m_gop_size = std::max(m_vdecode.m_decode_ctx->gop_size, 1);
			m_mediainfo.m_nb_frames = m_vdecode.m_stream->nb_frames;
			if (0 == m_mediainfo.m_nb_frames) {
				int64_t duration_second = m_vdecode.m_stream->duration * av_q2d(m_vdecode.m_stream->time_base);
				m_mediainfo.m_nb_frames = static_cast<uint64_t>(duration_second * av_q2d(m_vdecode.m_stream->avg_frame_rate));
			}
            m_mediainfo.m_vcodec_name = m_vdecode.m_codec->long_name;
        }
        
        if (m_adecode.m_stream){
			m_mediainfo.m_frame_rate = av_q2d(av_guess_frame_rate(m_filefmt_ctx, m_vdecode.m_stream, nullptr));
			m_mediainfo.m_samplerate = m_adecode.m_stream->codecpar->sample_rate;
			m_mediainfo.m_audio_depth = m_adecode.m_stream->codecpar->bits_per_coded_sample;
			m_mediainfo.m_samplefmt = ConverAudioFmt(m_adecode.m_decode_ctx->sample_fmt);
			m_mediainfo.m_acodec_name = m_adecode.m_codec->long_name;
        }
      
        m_mediainfo.m_duration     = m_filefmt_ctx->duration/AV_TIME_BASE;
    
        return;

    }

    void FFReader::ThreadFunc() {

        while (m_runing.load(std::memory_order_relaxed)) {

            {
                if (m_seek.load(std::memory_order_relaxed)){               
                    SeekFile();
                    m_seek.store(false, std::memory_order_relaxed);
                }
            
                if (m_reset.load(std::memory_order_relaxed)){
                    SeekFile();
                    m_reset.store(false, std::memory_order_relaxed);
                }
            }

            PrepareFrame();
            MoveFrameToCache();
        } 
    }

    int FFReader::ReadNextPacket() {
    
        AVPacket read_pkt;
        av_packet_unref(m_curvalid_decode->m_read_pkt);

        do {
			int ret = av_read_frame(m_filefmt_ctx, &read_pkt);
			if (AVERROR_EOF == ret) {
				m_curvalid_decode->m_pkt_eof = true;
                m_curvalid_decode->m_packet_ready = true;
                break;
			}

			if (m_curvalid_decode->m_stream->index == read_pkt.stream_index) {
				av_packet_move_ref(m_curvalid_decode->m_read_pkt, &read_pkt);
                m_curvalid_decode->m_packet_ready = true;
			}

            av_packet_unref(&read_pkt);
        } while (!m_curvalid_decode->m_packet_ready);
	           
	    return 0;
    }

    bool FFReader::PrepareFrame() {
        
        do {
            int ret = DecodeFrame();
            if (ret >= 0) {
                break;
            }
            ReadNextPacket();
            if (m_curvalid_decode->m_pkt_eof && m_curvalid_decode->m_code_eof) {
                break;
            }
        
        } while(true);

        return false;
    }

    int FFReader::DecodeFrame() {
        int ret = avcodec_receive_frame(m_curvalid_decode->m_decode_ctx, m_curvalid_decode->m_decode_frame);
        if(ret >= 0) {
            m_curvalid_decode->m_frame_ready = true;      
            return  ret;
        }else if (AVERROR_EOF == ret) {
            m_curvalid_decode->m_code_eof = true;
            avcodec_flush_buffers(m_curvalid_decode->m_decode_ctx);
        }

        if (m_curvalid_decode->m_packet_ready) {
            avcodec_send_packet(m_curvalid_decode->m_decode_ctx, m_curvalid_decode->m_read_pkt);
            m_curvalid_decode->m_packet_ready = false;
        }

        return ret;
    }

    bool FFReader::CheckContinueDecode() {
        return false;
    }

    void FFReader::MoveFrameToCache() {
        if (!m_curvalid_decode->m_frame_ready ) {
            return ;
        }

        m_curvalid_decode->m_frame_ready = false;
        AVFrame* codec_frame = m_curvalid_decode->m_decode_frame;
        if (codec_frame->pts < m_seek_pts) {
            av_frame_unref(codec_frame);
            return;
        } 
        
        {
            std::unique_lock<std::mutex> cache_lock(m_cache_mux);
            m_cache_condi.wait(cache_lock, [this] {return m_frame_cache.size() < m_maxche_size || m_expect_new_frame.load(std::memory_order_relaxed) || m_seek.load(std::memory_order_relaxed);});      
            if (m_frame_cache.size() < m_maxche_size) {
                std::shared_ptr<AVFrame> newframe{av_frame_alloc(), [](AVFrame* frame){av_frame_unref(frame); av_frame_free(&frame);}};
                av_frame_ref(newframe.get(), codec_frame);
                m_frame_cache.insert(std::pair<uint64_t, std::shared_ptr<AVFrame>>{m_decode_pos++, newframe});
            }else {
                auto first_node = m_frame_cache.begin();
                std::shared_ptr<AVFrame> frame = first_node->second;
                av_frame_unref(frame.get());
                m_frame_cache.erase(first_node);
                m_available_pos++;
                av_frame_move_ref(frame.get(), codec_frame);
                m_frame_cache.insert(std::pair<uint64_t, std::shared_ptr<AVFrame>>{m_decode_pos++, frame});
            }

            
            if (m_expect_new_frame.load(std::memory_order_relaxed)){
                m_expect_new_frame.store(false, std::memory_order_relaxed);
            }
        }
        m_cache_condi.notify_one();
        return;

    }

    AudioFmt FFReader::ConverAudioFmt(AVSampleFormat samplefmt) {
        switch (samplefmt) {
        case AV_SAMPLE_FMT_U8:
            return AudioFmt::kAudioFmtU8BIT;
        case AV_SAMPLE_FMT_S16:
            return AudioFmt::kAudioFmt16BIT;
        case AV_SAMPLE_FMT_S32:
            return AudioFmt::kAudioFmt32BIT;
        case AV_SAMPLE_FMT_FLT:
            return AudioFmt::kAudioFmtFLOAT;
        case AV_SAMPLE_FMT_U8P:
            return AudioFmt::kAudioFmtU8BIT_PLANAR;
        case AV_SAMPLE_FMT_S16P:
            return AudioFmt::kAudioFmt16BIT_PLANAR;
        case AV_SAMPLE_FMT_S32P:
            return AudioFmt::kAudioFmt32BIT_PLANAR;
        case AV_SAMPLE_FMT_FLTP:
            return AudioFmt::kAudioFmtFLOAT_PLANAR;
        default:
            return AudioFmt::kAudioFmtUNKNOWN;
        }
    }

    void FFReader::CloseFile() {
        m_runing.store(false, std::memory_order_relaxed);
        ClearDecoderRes(&m_vdecode);
        ClearDecoderRes(&m_adecode);
        FlushFrameCache();
        avformat_close_input(&m_filefmt_ctx);
    }

    void FFReader::ClearDecoderRes(ffdecode * decode) {

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

            // if (nullptr != decode->m_decode_ctx)
            {
                avcodec_free_context(&decode->m_decode_ctx);
            }

            // if (nullptr != decode->m_sws_ctx)
            {
                sws_freeContext(decode->m_sws_ctx);
                decode->m_sws_ctx = nullptr;
            }

            // if (nullptr != decode->m_swr_ctx)
            {
                swr_free(&decode->m_swr_ctx);
                decode->m_swr_ctx = nullptr;
            }
    }

    void FFReader::FlushDecoder() {
        avcodec_flush_buffers(m_curvalid_decode->m_decode_ctx); 
    }

    void FFReader::FlushFrameCache() {
        m_frame_cache.clear();
    }
}