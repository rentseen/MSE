#ifndef WAVE_EXPLORER_H
#define WAVE_EXPLORER_H


extern "C" 
{
	#include "libavcodec/avcodec.h"
	#include "libavformat/avformat.h"
	#include "libavutil/avutil.h"
    //#include "libavutil/avstring.h"
	//#include "libavutil/mathematics.h"
}



class WaveExplorer{
private:







public:

	WaveExplorer(){}

	//
	void load_file(const char *file_path);

	void close_file();



}





#endif