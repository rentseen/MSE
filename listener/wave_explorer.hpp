#ifndef WAVE_EXPLORER_H
#define WAVE_EXPLORER_H

#include <iostream>
#include <vector>
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



// info of a wave
class Wave{
public:
	// encodes both amplitude and phase
	double real; 	// real part of fft K
	double imag;	// imag part of fft K
	int index;		// index in fft

	Wave(double r = 0, double i = 0, int k = 0) : real(r), imag(i), index(k) {}

	double square() {
		return pow(real, 2) + pow(imag, 2);
	}
};

class WaveSequece{
public:
	int nb_waves;
	std::vector<double> real;
	std::vector<double> imag;
	std::vector<int> index;

	void fill(int16_t *samples, int nb_samples);

	WaveSequece();
	void fast_fourier_transform(int16_t *samples, int nb_samples);
	void sort_waves();
	void print_waves();
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
    
    WaveSequece waves;

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