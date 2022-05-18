/*
* 2022-05-11
*this class is use ffmpeg to read video or audio metadata for clip
*user can specify video frame pixfmt¡¢width¡¢height
*
*warning: both video and audio resources are managed internally in the class
*         the user can not release any of them
*
*/





#pragma once
#include <memory>
#include <vector>
#include <string>

typedef struct media_params
{
	uint64_t  m_nb_frames;
	int		  m_pixfmt;
	char	  m_pixfmt_name[9];
	int		  m_width;
	int		  m_height;
	int		  m_gop_size;
	uint64_t  m_samplerate;
	int       m_samplefmt;
	char      m_samplefmt_name[9];
	int       m_audio_depth;
	bool	  m_hasvideo;
	bool      m_hasaudio;
}MediaInfo;


enum class PixFmt
{
	kYUV422,
	kYUV422P,
	kYUV420P,
	kYUVP,
	kRGB
};

enum class MetaDataType
{
	kVideo,
	kAudio,
};

struct ReaderParam
{
	bool         m_origin;
	MetaDataType m_type;
	int			 m_width;
	int			 m_height;
	PixFmt		 m_pixfmt;

	int			 m_audio_depth;
	int			 m_samplerate;	
};


struct FrameInfo
{
	uint8_t* data[8];
	int64_t  pts;
};

class FFmpegReader
{
public:
	FFmpegReader();
	~FFmpegReader();

	void Reset();
	bool InitAVFmt(const std::string& filename);
	
	bool SetFrameParam(const ReaderParam& params);
	int  GetVideoFrame(FrameInfo& frame, int frameindex);
	MediaInfo  GetMediaInfo();
private:
	class CEXFFmpegReader;
	std::unique_ptr<CEXFFmpegReader> m_impl_reader;
};

