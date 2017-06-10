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

// info of a wave
class Wave{
public:
	double real; 	// real part of fft K
	double imag;	// imag part of fft K
	int index;	// index in fft

	Wave(double r = 0, double i = 0, int k = 0) : real(r), imag(i), index(k) {}

	double square() {
		return pow(real, 2) + pow(imag, 2);
	}
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
    int16_t *samples;

	// align nb_samples to power of 2
	int nb_slices;
    
    // key waves get by fft
    int nb_waves;
    Wave *keywaves;

	// alloc and set samples of a frame
	int get_samples();

	// use cairo to draw frames
	void draw_samples(cairo_t *cr);
	void draw_awave(cairo_t *cr, Wave wave);
	void draw_kwaves(cairo_t *cr, int k);


public:

	WaveExplorer();
	~WaveExplorer();

	// open, find and load codec
	int load_file(const char *file_path);

	// parse a frame: decode, fft, return nb_samples
	int poll_frames();

	// draw samples, k key waves
	void draw_frames(const char *file_path, int k);

	void close_file();


};


#endif