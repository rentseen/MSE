#include "wave_explorer.hpp"
#include "fft.hpp"

#define IMAGE_WIDTH     7350
#define IMAGE_HEIGHT    4200

const double PI = 3.141592653589793238460;

WaveSequece::WaveSequece()
{
    nb_waves = 0;
}

void WaveSequece::fast_fourier_transform(std::vector<int> samples, int nb_samples)
{
    nb_waves = nb_samples;
    real.resize(nb_waves);
    imag.resize(nb_waves);
    index.resize(nb_waves);

    for (int i = 0; i < nb_waves; i++) {
        real[i] = double(samples[i]);
        imag[i] = 0; 
        index[i] = i;
    }

    Fft::transform(real, imag);
}

double square(double real, double imag)
{
    return pow(real, 2) + pow(imag, 2);
}

void quick_sort(std::vector<double> &real, std::vector<double> &imag, 
                std::vector<int> &index, int left, int right)
{
    if (left >= right)
        return;

    int i = left, j = right;

    double re = real[i], im = imag[i], idx = index[i];
    while(i < j){

        while(i < j && square(re, im) >= square(real[j], imag[j]))
            j--;

        if (i < j) {
            real[i] = real[j];
            imag[i] = imag[j];
            index[i] = index[j];
        }

        while(i < j && square(re, im) <= square(real[i], imag[i]))
            i++;

        if (i < j) {
            real[j] = real[i];
            imag[j] = imag[i];
            index[j] = index[i];
        }
    }

    real[i] = re;
    imag[i] = im;
    index[i] = idx;

    quick_sort(real, imag, index, left, i - 1);
    quick_sort(real, imag, index, i + 1, right);
}

void WaveSequece::sort_waves()
{
    quick_sort(real, imag, index, 0, nb_waves - 1);
}

void WaveSequece::print_waves()
{
    for (int i = 0; i < nb_waves; i++)
        std::cout << square(real[i], imag[i]) << '\t' << real[i] << '\t' << imag[i] 
                    << '\t' << index[i] << std::endl;
}



WaveExplorer::WaveExplorer() 
{
    format_ctx = NULL;
    codec_ctx = NULL;
    codec = NULL;

    // register all the codecs
    av_register_all();
    avcodec_register_all();
    
    frame = av_frame_alloc();
    if (frame == NULL) {
        std::cout << "Error could not alloc frame" << std::endl;
    }

    empty_song = false;

    nb_samples = 0;
    samples.resize(NB_SAMPLES_PER_SECOND);
    next_sample_pos = -1;
}

WaveExplorer::~WaveExplorer() 
{
    av_frame_free(&frame);
}


int WaveExplorer::load_file(const char *file) 
{
    // autodetecting the format and read the header, exporting the information
    if (avformat_open_input(&format_ctx, file, NULL, NULL) < 0) {
        std::cout << "Error avformat could not open input file, " << std::endl;
        return 1;
    }
    
    // useful for file formats with no headers such as MPEG
    if (avformat_find_stream_info(format_ctx, NULL) < 0) {  
        std::cout << "Error found stream info failed " << std::endl;
        return 1; 
    }  
  
    // print the infomation
    av_dump_format(format_ctx, -1 , file, 0);  

    // find the first audio stream
    int audio_stream = -1;
    for( int i = 0; i < format_ctx -> nb_streams; ++i ) {
        if (format_ctx->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO) {
            audio_stream = i;
            //std::cout << "found audio stream with index=" << audio_stream << std::endl;
            break;
        }
    }

    // get the pointer to the codec context for the audio stream
    codec_ctx = format_ctx->streams[audio_stream]->codec;

    // find the decoder for the stream
    codec = avcodec_find_decoder(codec_ctx->codec_id);
    //std::cout << "codec_id=" << codec_ctx->codec_id << std::endl;
    if (codec == NULL) {
        std::cout << "Error codec not found" << std::endl;
        return 1;
    }
    
    if (avcodec_open2(codec_ctx, codec, NULL) < 0) {
        std::cout << "Error could not open codec" << std::endl;
        return 1;
    }
    return 0;
}

void WaveExplorer::close_file() 
{
    avformat_close_input(&format_ctx);
}

/*
int WaveExplorer::get_samples() 
{
    if (av_read_frame(format_ctx, &packet) < 0 || packet.size <= 0) {
        return 1;
    }

    int got_frame;
    if (avcodec_decode_audio4(codec_ctx, frame, &got_frame, &packet) < 0) {
        std::cout << "Error while decoding 1" << std::endl;
        return 1;
    }

    if (got_frame) {
        if (samples == NULL) {
            samples = new int16_t[frame->nb_samples];
        } else if (nb_samples < frame->nb_samples) {
            delete [] samples;
            samples = new int16_t[frame->nb_samples];
        }
        nb_samples = frame->nb_samples;

        switch (frame->format) {
            case AV_SAMPLE_FMT_S16P:
                // format is AV_SAMPLE_FMT_S16P, signed 16 bits, planar 
                // planar, data[i] contains sample of channle i
                for (int i = 0; i < nb_samples; i++) {
                    int16_t value = 0;
                    value |= frame->data[0][i*2+1];
                    value <<= 8;
                    value |= frame->data[0][i*2];
                    samples[i] = (int16_t)value;
                    //std::cout << samples[i] << ' ';
                }
                //std::cout << std::endl;
        }
        // unreference all buffers referenced by the frame
        av_frame_unref(frame);
    } else {
        nb_samples = 0;
        return 1;
    }

    av_free_packet(&packet);
    return 0;
}
*/

void get_frame_samples(AVFrame *frame, int &next_sample_pos, std::vector<int> &samples, int &nb_samples)
{
    switch (frame->format) {
        case AV_SAMPLE_FMT_S16P:
            for (; next_sample_pos < frame->nb_samples && nb_samples < NB_SAMPLES_PER_SECOND; next_sample_pos++) {
                int16_t value = 0;
                value |= frame->data[0][next_sample_pos*2+1];
                value <<= 8;
                value |= frame->data[0][next_sample_pos*2];
                samples[nb_samples++] = (int16_t)value;
                //std::cout << samples[i] << ' ';
            }
    }
}


int WaveExplorer::next_second_samples() 
{
    nb_samples = 0;
    while (nb_samples < NB_SAMPLES_PER_SECOND) {
        if (next_sample_pos == -1) {
            if (av_read_frame(format_ctx, &packet) < 0 || packet.size <= 0) {
                empty_song = true;
                return 1;
            }

            int got_frame;
            if (avcodec_decode_audio4(codec_ctx, frame, &got_frame, &packet) < 0) {
                std::cout << "Error while decoding 1" << std::endl;
                empty_song = true;
                return 1;
            }
            if (!got_frame) {
                empty_song = true;
                return 1;
            } else {
                empty_song = false;
            }
        }

        get_frame_samples(frame, next_sample_pos, samples, nb_samples);
    
        if (next_sample_pos >= frame->nb_samples) {        
            av_frame_unref(frame);
            av_free_packet(&packet);
            next_sample_pos = -1;
        }
        
    }

    return 0;
}


int WaveExplorer::next_second()
{
    if (next_second_samples()) {
        return 0;
    }

    waves.fast_fourier_transform(samples, nb_samples);
    waves.sort_waves();

    return 0;
}



// use cairo to draw waves 
void WaveExplorer::draw_samples(cairo_t *cr)
{
    for (int i = 0; i < nb_samples; i++) {
        int x0 = i % IMAGE_WIDTH;
        int y0 = 400 + 800 * (i / IMAGE_WIDTH);
        cairo_move_to(cr, x0, y0);
        cairo_line_to(cr, x0, y0 - ((int)samples[i] / 40));
    }
    cairo_stroke(cr);
}


void WaveExplorer::draw_kwaves(cairo_t *cr, int k) 
{
    std::cout << "draw_kwaves, nb_waves=" << waves.nb_waves << std::endl;
    int N = waves.nb_waves;
    double scale[k], alpha[k], theta[k];
    for (int i = 0; i < k; i++) {
        scale[i] = - sqrt((pow(waves.real[i], 2) + pow(waves.imag[i], 2))) / 40 / N;
        alpha[i] = 2.0 * PI * waves.index[i] / N;
        theta[i] = atan2(waves.imag[i], waves.real[i]);
    }

    for (int i = 0; i < nb_samples; i++) {
        double value = 0;
        for (int j = 0; j < k; j++) {
            value +=  scale[j] * cos(alpha[j] * i + theta[j]);
        }

        int x0 = i % IMAGE_WIDTH;
        int y0 = 400 + 800 * (i / IMAGE_WIDTH);
        
        cairo_move_to(cr, x0, y0 + value - 2);
        cairo_line_to(cr, x0, y0 + value + 2);
    }

    cairo_stroke(cr);
}

void WaveExplorer::draw_frames(const char *file_path, int k) 
{
    if (k > nb_samples) {
        std::cout << "Error too large keywaves wanted by k" << std::endl;
        return;
    }
    cairo_surface_t *surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 7350, 4800);
    cairo_t *cr = cairo_create(surface);

    cairo_set_source_rgb (cr, 1,1,1);
    cairo_paint (cr);

    cairo_set_source_rgba (cr, 0.2, 0.8, 0.8, 1.0);
    cairo_set_line_width (cr, 1.0);
    draw_samples(cr);
    
    cairo_set_source_rgba (cr, 0.6, 0.3, 0.3, 1.0);
    cairo_set_line_width (cr, 2.0);

    //draw_awave(cr, Wave(waves.real[2], waves.imag[2], waves.index[2]));

    draw_kwaves(cr, k);

    //waves.print_waves();

    cairo_destroy (cr);
    cairo_surface_write_to_png (surface, file_path);
    cairo_surface_destroy (surface);
}
