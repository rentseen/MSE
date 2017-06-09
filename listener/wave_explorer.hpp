#ifndef WAVE_EXPLORER_H
#define WAVE_EXPLORER_H

#include <iostream>
#include <vector>
//#include <complex>
#include <math.h>

#include <cairo/cairo.h>

extern "C" 
{
	#include "libavcodec/avcodec.h"
	#include "libavformat/avformat.h"
	#include "libavutil/avutil.h"
    //#include "libavutil/avstring.h"
	//#include "libavutil/mathematics.h"
}

//typedef std::complex<double> Complex;

// used for FFT
const double PI = 3.141592653589793238460;


class KeyWaves{	
public:
	// X_k in FFT
	//Complex waves[8];
	// index of waves in FFT
	int index[8];
};


class WaveExplorer{
private:
	// ffmpeg data-structure to decode mp3 file
	AVFormatContext *format_ctx;
    AVCodecContext *codec_ctx;
    AVCodec *codec;
    AVFrame *frame;
    AVPacket packet;

    // decoded samples of a frame, and waves by fft
    int nb_samples;
    int nb_slice;
    int16_t *samples = NULL;
    //CArray *waves = NULL;

    //std::vector<double> real;
    //std::vector<double> imag;

    KeyWaves keywaves;

	// alloc and set samples of a frame
	int get_samples();

	// use cairo to draw frames
	void draw_samples(cairo_t *cr);
	void draw_waves(cairo_t *cr, double real, double imag, int k);


public:

	WaveExplorer();
	~WaveExplorer();

	// open, find and load codec
	int load_file(const char *file_path);

	// parse a frame: decode, fft, return nb_samples
	int poll_frames();

	// draw samples, key waves
	void draw_frames(const char *file_path, int maxk);

	void close_file();


};


#endif