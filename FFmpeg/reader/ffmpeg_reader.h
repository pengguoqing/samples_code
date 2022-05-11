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
	uint64_t  nb_frames;
	int		  pixfmt;
	char	  pix_name[9];
	int		  width;
	int		  height;
	int		  gop_size;
	uint64_t  audio_samplerate;
}MediaInfo;

enum class ReadMode
{
	kSeek,
	kPlay,
};

enum class PixFmt
{
	kOrigin,
	kYUV422,
	kYUV422P,
	kYUV420,
	kYUV420P,
	kYUV,
	kRGB
};

enum class RecErrors
{
	kInitAVError,
	kFramePosError,
	
};




class FFmpegReader
{
public:
	FFmpegReader();
	~FFmpegReader();

	void Reset();
	bool InitAVFmt(const std::string& filename);
	
	void SetFrameParam(PixFmt pixfmt, int width, int height);
	int  ReadVideoFrame(std::vector<uint8_t*>& framedata, int frameindex);
	MediaInfo  GetMediaInfo();
private:
	class CEXFFmpegReader;
	std::unique_ptr<CEXFFmpegReader> m_impl_reader;
};

