/*
* create data: 2023-02-28
* this class is the implement for readerbase pure virtual class  in ffmpeg
* contact info: forwardapeng@gmail.com
*/

#pragma once

#include "readerbase.h"
#include <map>
#include <unordered_map>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <iostream>
#include <atomic>

extern "C"
{
	#include "libavformat/avformat.h"
	#include "libavcodec/avfft.h"
	#include "libavcodec/avcodec.h"
	#include "libavutil/avutil.h"
    #include "libavutil/pixdesc.h"
    #include "libswscale/swscale.h"
	#include "libswresample/swresample.h"
	#include "libavutil/imgutils.h"
    #include "libavutil/samplefmt.h"
}


namespace mediaio {

class FFReader: public IClipReader {
   struct ffdecode {
      ffdecode() = default;
	  AVStream*		  m_stream{nullptr};
	  AVCodecContext* m_decode_ctx{nullptr};
	  const AVCodec*  m_codec{nullptr};

	  AVFrame*		  m_sw_frame{nullptr};
	  AVFrame*		  m_decode_frame{nullptr};
	  AVPacket*       m_read_pkt{nullptr};

      SwsContext*     m_sws_ctx{nullptr};
	  SwrContext*     m_swr_ctx{nullptr};

	  bool			  m_frame_ready{false};
	  bool			  m_packet_ready{false};
	  bool			  m_pkt_eof{false};
	  bool            m_need{false};
   };

    public:
        FFReader() = default;
        ~FFReader();
            
        bool        OpenClipFile(const std::string& filepath, SoureType metatype)  override;
        void        CloseClipFile()                         override;
        ClipInfo    GetClipInfo() const                     override;
        bool        GetSourceData(uint64_t pos, AVSoucreData* frame)  override;
	
    private:
		bool 		StartRead();
	    bool 		InitDecoder(enum AVMediaType type);
	    bool 		OpenCodec(ffdecode* decode);
	    void 		ParaseMediaInfo();
		bool		SeekToFrameNum(uint64_t seek_pos);
		void		SeekFile();
	    
	    int  		ReadNextPacket();
	    bool 		PrepareFrame();
	    int	 		DecodeFrame();
	    bool 		CheckContinueDecode();
	    void 		MoveFrameToCache();
		AudioFmt    ConverAudioFmt(AVSampleFormat samplefmt);

		void 		ThreadFunc();

    private:
	    void CloseFile();
	    void ClearDecoderRes(ffdecode* decode);
	    void FlushDecoder();
	    void FlushFrameCache();
	
    private:
	    AVFormatContext* m_filefmt_ctx{nullptr};
	    ffdecode         m_vdecode;
	    ffdecode		 m_adecode;
		ffdecode*  	   	 m_curvalid_decode{nullptr};
	    ClipInfo		 m_mediainfo;
	    SoureType   	 m_read_type{SoureType::kSourceTypeUnknow};
	    bool			 m_exit{false};
	    bool		     m_eof{false};
	    bool			 m_sws{false};
	    bool             m_swr{false};

		uint64_t				 m_available_pos{0};
		uint64_t 				 m_decode_pos{0};
	    uint64_t				 m_seek_pos{0};
		
	    const size_t				m_maxche_size{5};
	    std::mutex					m_cache_mux;
	    std::condition_variable_any m_cache_condi;
	    std::map<uint64_t, std::shared_ptr<AVFrame>>	m_frame_cache;

	    std::condition_variable_any m_status_condi;

	    std::atomic<bool>           m_seek{false};
	    std::atomic<bool>			m_reset{false};
		std::atomic<bool>			m_expect_new_frame{false};

	    int64_t          			m_seek_pts{0};

    private:
	    std::thread      m_read_th;
};

}
