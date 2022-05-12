#pragma once

#include "ffmpeg_reader.h"
#include <map>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>

extern "C"
{
	#include "libavformat/avformat.h"
	#include "libavcodec/avfft.h"
	#include "libavutil/avutil.h"
    #include "libavutil/pixdesc.h"
    #include "libswscale/swscale.h"
	#include "libswresample/swresample.h"
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
	SwrContext*     m_swr_ctx;

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
	bool SetFrameParam(const FrameDataParam& dataparams);
	int  GetVideoFrame(std::vector<uint8_t*>& framedata,  int frameindex);
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
	MediaInfo		 m_mediainfo;
	FrameDataParam   m_dst_frameinfo;
	bool			 m_exit;
    int				 m_cur_pos;
	int				 m_seek_pos;
	bool		     m_eof;


	const int					m_maxche_size;
	std::mutex					m_vcache_mux;
	std::condition_variable_any m_vcache_condi;
	std::map<int, AVFrame*>		m_vframe_cache;
	std::mutex		 m_status_mux;
	bool             m_seek;
	bool			 m_reset;
	bool			 m_raw_data;

private:
	std::thread m_prepare_th;
};
