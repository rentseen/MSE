#include "wave_explorer.hpp"
#include "fft.hpp"

const double PI = 3.141592653589793238460;

WaveSequece::WaveSequece()
{
    nb_waves = 0;
}

void WaveSequece::fill(int16_t *samples, int nb_samples)
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
}

void WaveSequece::fast_fourier_transform(int16_t *samples, int nb_samples)
{
    fill(samples, nb_samples);
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

    nb_samples = 0;
    samples = NULL;
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
    delete samples;
}

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

/*
void fft(double *xr, double *xi, double *yr, double *yi, int N, int step)
{
    if (step < N) {
        fft(yr, yi, xr, xi, N, step * 2);
        fft(yr + step, yi + step, xr + step, xi + step, N, step * 2);
 
        for (int i = 0; i < N; i += 2 * step) {
            double cosa = cos(-2 * PI * i / N);
            double sina = sin(-2 * PI * i / N);
            double tr = cosa * yr[i + step] - sina * yi[i + step];
            double ti = cosa * yi[i + step] + sina * yr[i + step];
            xr[i / 2]     = yr[i] + tr;
            xi[i / 2]     = yi[i] + ti;
            xr[(i + N)/2] = yr[i] - tr;
            xi[(i + N)/2] = yi[i] - ti;
        }
    }
}

*/


int WaveExplorer::poll_frames()
{
    if (get_samples()) {
        return 0;
    }

    //waves.fill(samples, nb_samples);
    waves.fast_fourier_transform(samples, nb_samples);

    return 0;
}



// use cairo to draw waves 
void WaveExplorer::draw_samples(cairo_t *cr)
{
    for (int i = 0; i < nb_samples; i++) {
        cairo_move_to(cr, i, 350);
        cairo_line_to(cr, i, 350 - ((int)samples[i] / 100));
    }
    cairo_stroke(cr);
}

void WaveExplorer::draw_awave(cairo_t *cr, Wave wave) 
{
    int N = waves.nb_waves;
    double scale = - sqrt((pow(wave.real, 2) + pow(wave.imag, 2))) / 100 / N;
    double alpha = 2.0 * PI * wave.index / N;
    double theta = atan2(wave.imag, wave.real);

    for (int i = 0; i < nb_samples; i++) {   
        double value = scale * cos(alpha * i + theta);
        cairo_move_to(cr, i, 348 + value);
        cairo_line_to(cr, i, 352 + value);
    }

    cairo_stroke(cr);
}

void WaveExplorer::draw_kwaves(cairo_t *cr, int k) 
{
    waves.sort_waves();

    int N = waves.nb_waves;
    double scale[k], alpha[k], theta[k];
    for (int i = 0; i < k; i++) {
        scale[i] = - sqrt((pow(waves.real[i], 2) + pow(waves.imag[i], 2))) / 100 / N;
        alpha[i] = 2.0 * PI * waves.index[i] / N;
        theta[i] = atan2(waves.imag[i], waves.real[i]);
    }

    for (int i = 0; i < nb_samples; i++) {
        double value = 0;
        for (int j = 0; j < k; j++) {
            value +=  scale[j] * cos(alpha[j] * i + theta[j]);
        }
        
        cairo_move_to(cr, i, 349 + value);
        cairo_line_to(cr, i, 351 + value);
    }

    cairo_stroke(cr);
}

void WaveExplorer::draw_frames(const char *file_path, int k) 
{
    if (k > nb_samples) {
        std::cout << "Error too large keywaves wanted by k" << std::endl;
        return;
    }
    cairo_surface_t *surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, nb_samples, 700);
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
