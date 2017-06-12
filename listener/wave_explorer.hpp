#ifndef WAVE_EXPLORER_H
#define WAVE_EXPLORER_H

#include <iostream>
#include <vector>
#include <math.h>

#include <cairo/cairo.h>

#define NB_SAMPLES_PER_SECOND 	44100

extern "C" 
{
	#include "libavcodec/avcodec.h"
	#include "libavformat/avformat.h"
	#include "libavutil/avutil.h"
    //#include "libavutil/avstring.h"
	//#include "libavutil/mathematics.h"
}



class WaveSequece{
public:
	int nb_waves;
	std::vector<double> real;
	std::vector<double> imag;
	std::vector<int> index;

	WaveSequece();
	void fast_fourier_transform(std::vector<int> samples, int nb_samples);
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

    bool empty_song;

    // decoded samples of a frame, and waves by fft
    int nb_samples;
    std::vector<int> samples;
    int next_sample_pos;
    
    WaveSequece waves;

	// alloc and set samples of a frame
	int next_second_samples();

	// use cairo to draw frames
	void draw_samples(cairo_t *cr);
	void draw_kwaves(cairo_t *cr, int k);

public:

	WaveExplorer();
	~WaveExplorer();

	// open, find and load codec
	int load_file(const char *file_path);

	// parse a frame: decode, fft, return nb_samples
	int next_second();

	// draw samples, k key waves
	void draw_frames(const char *file_path, int k);

	void close_file();

};

#endif