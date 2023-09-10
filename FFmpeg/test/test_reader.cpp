#include <windows.h>
#include <iostream>
#include <string>
#include <memory>
#include <functional>
#include <iomanip>
#include <random>
#include <thread>

#include "factorybase.h"
#include "readerbase.h"


using namespace std;
using FactoryCreater = mediaio::IFactory*();
using FactoryDeleter = void(void*);


int main(int argc, char** argv) {
	
	string libpath{"G://samples_code//FFmpeg//install//ffrw.dll"};
	HMODULE h_moudle = LoadLibrary(libpath.c_str());
	if (nullptr != h_moudle){

		function<FactoryDeleter> factory_deleter = reinterpret_cast<FactoryDeleter*>(GetProcAddress(h_moudle, "ReleaeFactory"));
		function<FactoryCreater> factory_creater = reinterpret_cast<FactoryCreater*>(GetProcAddress(h_moudle, "CreateMediaIOFactory"));
		mediaio::IFactory* factory =  factory_creater();
		mediaio::IClipReader* clip_reader = factory->CreateReader();
		mediaio::ClipInfo media_info;

		string filepath{ "F://clip//01.mp4" };
		if (clip_reader->OpenClipFile(filepath, mediaio::SoureType::kSourceTypeV)) {
			mediaio::ClipInfo media_info = clip_reader->GetClipInfo();
			mediaio::AVSoucreData videoframe;
			
			//radom get 10 frames
			for (uint64_t i{0}; i<10; i++){
				std::random_device rd;  
				std::mt19937 gen(rd());
				uniform_int_distribution<> distrib(0, media_info.m_nb_frames - 1);
				uint64_t pos = distrib(gen);

				chrono::high_resolution_clock::time_point begin = chrono::high_resolution_clock::now();
				clip_reader->GetSourceData(pos, &videoframe);
				chrono::high_resolution_clock::time_point end = chrono::high_resolution_clock::now();
				chrono::duration<double, milli> duration_ms = end- begin;
				
				cout << "pos:" << left << setw(6) << pos << "--> duration: " << left << setw(8) << fixed << std::setprecision(6) << videoframe.m_duration << " -- pts:" << left << setw(16) << videoframe.m_pts << "****** performance:"<< fixed << std::setprecision(6) << duration_ms.count() <<endl;
			}
			this_thread::sleep_for(chrono::seconds(3));
			for (uint64_t i{0}; i<media_info.m_nb_frames; i++)
			{
				chrono::high_resolution_clock::time_point begin = chrono::high_resolution_clock::now();
				clip_reader->GetSourceData(i, &videoframe);
				chrono::high_resolution_clock::time_point end = chrono::high_resolution_clock::now();
				chrono::duration<double, milli> duration_ms = end - begin;

				cout << "pos:" << left << setw(6) << i << "--> duration: " << left << setw(8) << fixed << std::setprecision(6) << videoframe.m_duration << " -- pts:" << left << setw(16) << videoframe.m_pts << "****** performance:" << fixed << std::setprecision(6) << duration_ms.count()<< endl;
			}

		}

		clip_reader->CloseClipFile();

		factory->Release(clip_reader);
		factory_deleter(factory);
	}


	


	return 0;
}
