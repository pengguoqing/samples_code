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
      ffdecode();
	  AVStream*		  m_stream;
	  AVCodecContext* m_decode_ctx;
	  const AVCodec*  m_codec;

	  AVFrame*		  m_sw_frame;
	  AVFrame*		  m_decode_frame;
	  AVPacket*       m_read_pkt;

      SwsContext*     m_sws_ctx;

	  SwrContext*     m_swr_ctx;

	  bool			  m_frame_ready;
	  bool			  m_packet_ready;
	  bool			  m_file_eof;
	  bool            m_need;
   };

    public:
        FFReader();
        ~FFReader();
            
        bool        OpenClipFile(std::string pathname)      override;
        void        CloseClipFile()                         override;
        ClipInfo    GetClipInfo() const                     override;
        void        SetOutputFmt(const ClipOutputFmt& info) override;
        bool        GetSourceV(VideoSource* frame, uint64_t pos)   override;
        bool        GetSourceA(AudioSource* frame, uint64_t pos)   override;
        void        ReleaseFrame(uint64_t pos)              override;

    private:
	    bool InitDecoder(enum AVMediaType type);
	    bool OpenCodec(ffdecode* decode);
	    void ParaseMediaInfo();
	
	    void ThreadFunc();
	    int  SeekFile(uint64_t seek_pos);
	    int  ReadNextPacket();
	    bool PrepareFrame();
	    int	 DecodePacket();
	    bool CheckContinueDecode();
	    void MoveFrameToCache();
	    AVPixelFormat GetPixfmt();

    private:
	    void CloseFile();
	    void ClearDecoderRes(ffdecode* decode);
	    void FlushDecoder();
	    void FlushFrameCache();

    private:
	    AVFormatContext* m_filefmt_ctx;
	    ffdecode         m_vdecode;
	    ffdecode		 m_adecode;
	    ffdecode*        m_curdecode;
	    ClipInfo		 m_mediainfo;
	    ReaderParam      m_dst_frameinfo;
	    bool			 m_exit;
        int				 m_cur_pos;
	    int				 m_seek_pos;
	    bool		     m_eof;
	    bool			 m_sws;
	    bool             m_swr;

	    const int					m_maxche_size;
	    std::mutex					m_vcache_mux;
	    std::condition_variable_any m_cache_condi;
	    std::map<int, AVFrame*>	    m_frame_cache;
	
	    std::condition_variable_any m_status_condi;
	    std::mutex		 m_status_mux;
	    bool             m_seek;
	    bool			 m_reset;

	    int64_t          m_seek_pts;

    private:
	    std::thread m_prepare_th;
};

}
