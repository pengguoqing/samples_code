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
}


 struct mp_decode 
 {
	AVStream* stream;
	bool audio;

	AVCodecContext* decoder;
	AVCodec* codec;

	AVFrame* sw_frame;
	AVFrame* decode_frame;
	AVPacket* read_pkt;

	bool got_first_keyframe;
	bool frame_ready;
	bool eof;
	bool packet_pending;
};



class FFmpegReader::CEXFFmpegReader
{

public:
	CEXFFmpegReader();
	~CEXFFmpegReader();

	void Reset();
	bool InitAVFmt(const std::string& filename, bool raw_data);
	void SetReadMode(ReadMode mode);
	
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
	void PushFrameToCache();

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
	ReadMode	     m_read_mode;
	bool			 m_exit;
    int				 m_cur_pos;
	bool	         m_hasvideo;
	bool			 m_hasaudio;
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

