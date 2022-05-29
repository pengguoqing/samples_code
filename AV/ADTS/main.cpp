#include <iostream>
#include <vector>
#include <unordered_map>
#include <fstream>

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

using namespace std;

//确保原素材中包含 AAC 音频流信息
bool CheckAACInClip(const char* url);

//从素材中读取每一帧音频包
bool ReadAACPkt(AVPacket* aac_pkt);

// 将读取的AAC帧数据保存在文件头中
void SaveCurADTSInfo(AVPacket* aac_pkt);

static unordered_map<int, int> g_samplerate_table = {
	{96000, 0},
	{88200, 1},
	{64000, 2},
	{48000, 3},
	{44100, 4},
	{32000, 5},
	{24000, 6},
	{22050, 7},
	{16000, 8},
	{12000, 9},
	{11025, 10},
	{8000,  11},
	{7350,  12}
};
 
static AVFormatContext* g_filefmt_ctx = nullptr;
static int g_audio_stream_index = -1;
static vector<char> g_adts_header(7);

int main(int argc, char* argv[])
{
	char* src_file = argv[1];
	char* dst_aac_file = argv[2];
	int  rec = -1;

	ofstream out_aac_file;
	bool prepare = false; 

	do 
	{
		if (!CheckAACInClip(src_file))
		{
			cout << "there is no aac stream in clip" << endl;
			break;
		}

		out_aac_file.open(dst_aac_file, ios_base::binary | ios_base::out | ios_base::app);
		if (!out_aac_file.good())
		{
			break;
		}
		prepare = true;

	} while (false);

	
	AVPacket* aac_pkt =av_packet_alloc();
	av_packet_unref(aac_pkt);
	while (prepare && ReadAACPkt(aac_pkt))
	{
		SaveCurADTSInfo(aac_pkt);
		out_aac_file.write(&g_adts_header.front(), g_adts_header.size() );
		out_aac_file.write(reinterpret_cast<char*>(aac_pkt->data), aac_pkt->size);
		
		av_packet_unref(aac_pkt);
	}

	out_aac_file.close();
	av_packet_free(&aac_pkt);
	avformat_close_input(&g_filefmt_ctx);
	return 0;
}

bool CheckAACInClip(const char* url)
{
	bool has_aac = false;
	int  rec = -1;
	do
	{
		rec = avformat_open_input(&g_filefmt_ctx, url, nullptr, nullptr);
		if (rec < 0)
		{
			break;
		}

		rec = avformat_find_stream_info(g_filefmt_ctx, nullptr);
		if (rec < 0)
		{
			break;
		}

		av_dump_format(g_filefmt_ctx, 0, url, 0);

		g_audio_stream_index = av_find_best_stream(g_filefmt_ctx, AVMEDIA_TYPE_AUDIO, -1, -1, nullptr, 0);
		if (AVERROR_STREAM_NOT_FOUND == g_audio_stream_index)
		{
			break;
		}

		if (AV_CODEC_ID_AAC != g_filefmt_ctx->streams[g_audio_stream_index]->codecpar->codec_id)
		{
			break;
		}

		has_aac = true;

	} while (false);

	return has_aac;
}

bool ReadAACPkt(AVPacket* aac_pkt)
{
	bool get_acc = false;
	
	while ( 0 == av_read_frame(g_filefmt_ctx, aac_pkt))
	{
		if (g_audio_stream_index == aac_pkt->stream_index)
		{
			get_acc = true;
			break;		
		}

		av_packet_unref(aac_pkt);
	}

	return get_acc;
}

void SaveCurADTSInfo(AVPacket* aac_pkt)
{
	//syncword 0xFFF
	g_adts_header[0] |= 0xff;
	g_adts_header[1] |= 0xf0;

	//ID 
	g_adts_header[1] |= (0<<3);

	//layer
	g_adts_header[1] |= (0<<1);

	//potection_absent
	g_adts_header[1] |= 1;

	//profile
	int profile = g_filefmt_ctx->streams[g_audio_stream_index]->codecpar->profile;
	g_adts_header[2] = (profile << 6);

	//sampple rate
	int sample_rate = g_filefmt_ctx->streams[g_audio_stream_index]->codecpar->sample_rate;
	int samplerate_mark = g_samplerate_table.find(sample_rate)->second;
	g_adts_header[2] |= (samplerate_mark << 2);

	//private bit
	g_adts_header[2] |= (0<<1);
	
	//channel
	int channels = g_filefmt_ctx->streams[g_audio_stream_index]->codecpar->channels;
	g_adts_header[2] |= (channels & 0x04) >> 2;
	g_adts_header[3] =  (channels & 0x03) << 6;

	//original_copy
	g_adts_header[3] |= (0<<5);

	//home
	g_adts_header[3] |= (0<<4);

	//copyright_identification_bit
	g_adts_header[3] |= (0<<3);

	//copyright_identification_start
	g_adts_header[3] |= (0<<2);

	//aac_frame_length
	int data_lenth = aac_pkt->size + static_cast<int>( g_adts_header.size() );
	//int data_lenth = aac_pkt->size + 7;
	g_adts_header[3] |= ( (data_lenth & 0x1800) >> 11 );
	g_adts_header[4] =  ( (data_lenth & 0x7F8) >> 3 );
	g_adts_header[5] =  ( (data_lenth & 0x7) << 5 );

	//adts_buffer_fullness
	g_adts_header[5] |= (0x7C >> 6);
	g_adts_header[6] |=  (0x3F << 2);
	
	//number_of_raw_data_blocks_in_frame
	g_adts_header[6] |= (0x00);
}