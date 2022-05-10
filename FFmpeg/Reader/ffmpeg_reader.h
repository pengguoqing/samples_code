#pragma once
#include <memory>
#include <vector>
#include <string>

typedef struct media_params
{
	uint64_t  nb_frames;
	int		  pixfmt;
	std::string    pixfmt_name;
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
	bool InitAVFmt(const std::string& filename, bool raw_data = false);
	void SetReadMode(ReadMode mode);
	
	int  ReadVideoFrame(std::vector<uint8_t*>& framedata, int frameindex);
	MediaInfo  GetMediaInfo();
private:
	class CEXFFmpegReader;
	std::unique_ptr<CEXFFmpegReader> m_impl_reader;
};

