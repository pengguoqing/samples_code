#pragma once

#include "ffmpeg_reader.h"
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
	#include "libavutil/avutil.h"
    #include "libavutil/pixdesc.h"
    #include "libswscale/swscale.h"
	#include "libswresample/swresample.h"
	#include "libavutil/imgutils.h"
    #include "libavutil/samplefmt.h"
}


 struct mp_decode 
 {
	AVStream*		m_stream;
	AVCodecContext* m_decode_ctx;
	AVCodec*	    m_codec;

	AVFrame*		m_sw_frame;
	AVFrame*		m_decode_frame;
	AVPacket*       m_read_pkt;

	SwsContext*     m_sws_ctx;

	SwrContext* m_swr_ctx;

	bool			m_frame_ready;
	bool			m_packet_ready;
	bool			m_eof;
	bool            m_need;
};



class FFmpegReader::CEXFFmpegReader
{

public:
	CEXFFmpegReader();
	~CEXFFmpegReader();

	void Reset();
	bool InitAVFmt(const std::string& filename);
	bool SetFrameParam(const ReaderParam& dataparams);
	int  GetVideoFrame(FrameInfo& frame,  int frameindex);
	MediaInfo  GetMediaInfo() const;

private:
	bool InitDecoder(enum AVMediaType type);
	bool OpenCodec(mp_decode* decode);
	void ParaseMediaInfo();
	
	void ThreadFunc();
	int  SeekFile(int seek_pos);
	int  ReadNextPacket();
	bool PrepareFrame();
	int	 DecodePacket();
	bool CheckContinueDecode();
	void MoveFrameToCache();
	AVPixelFormat GetPixfmt();

private:
	void CloseFile();
	void ClearDecoderRes(mp_decode* decode);
	void FlushDecoder();
	void FlushFrameCache();

private:
	AVFormatContext* m_filefmt_ctx;
	mp_decode        m_vdecode;
	mp_decode		 m_adecode;
	mp_decode*       m_curdecode;
	MediaInfo		 m_mediainfo;
	ReaderParam   m_dst_frameinfo;
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

