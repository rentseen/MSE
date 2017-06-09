#include "wave_explorer.hpp"


WaveExplorer::WaveExplorer() 
{
    format_ctx = NULL;
    codec_ctx = NULL;
    codec = NULL;
    frame = NULL;

    // register all the codecs
    av_register_all();
    avcodec_register_all();
    
    frame = av_frame_alloc();
    if (frame == NULL) {
        std::cout << "Error could not alloc frame" << std::endl;
    }
    
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
    //std::cout << "read_frame: begin" << std::endl;
    if (av_read_frame(format_ctx, &packet) < 0 || packet.size <= 0) {
        return 1;
    }
    //std::cout << "read_frame: finish" << std::endl;

    //frame = av_frame_alloc();

    int got_frame;
    //std::cout << "decode_frame: begin" << std::endl;
    if (avcodec_decode_audio4(codec_ctx, frame, &got_frame, &packet) < 0) {
        std::cout << "Error while decoding 1" << std::endl;
        return 1;
    }
    //std::cout << "decode_frame: finish" << std::endl;
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

    //av_frame_free(&frame);
    av_free_packet(&packet);
    return 0;
}

/*
void fft(double *xreal, double *ximag, Co *out, int N, int step)
{
    //std::cout << "here step = " << step << std::endl;
    if (step < N) {
        fft(out, buf, N, step * 2);
        fft(out + step, buf + step, N, step * 2);
 
        for (int i = 0; i < N; i += 2 * step) {
            //Complex t = cexp(-I * PI * i / N) * out[i + step];
            Complex t = std::polar(1.0, -2 * PI * i / N) * out[i + step];
            buf[i / 2]     = out[i] + t;
            buf[(i + N)/2] = out[i] - t;
        }
    }
}
*/


void fft(double *xr, double *xi, double *yr, double *yi, int N, int step)
{
    if (step < N) {
        fft(yr, yi, xr, xi, N, step * 2);
        fft(yr + step, yi + step, xr + step, xi + step, N, step * 2);
 
        for (int i = 0; i < N; i += 2 * step) {
            //Complex t = cexp(-I * PI * i / N) * out[i + step];
            //Complex t = std::polar(1.0, -2 * PI * i / N) * out[i + step];
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




int get_keywaves(int16_t *samples, int nb_samples, double *maxr, double *maxi, int *maxk, int nb_max)
{

    int N = log2(nb_samples);
    if (nb_samples != pow(2, N))
        N = pow(2, N + 1);

    double xr[N];
    double xi[N];
    double yr[N];
    double yi[N]; 

    for (int i = 0; i < nb_samples; i++) {
        xr[i] = yr[i] = double(samples[i]);
        xi[i] = yi[i] = 0;
    }

    for (int i = nb_samples; i < N; i++) {
        xr[i] = yr[i] = xi[i] = yi[i] = 0;
    }

    /*
    for (int i = 0; i < nb_samples; i++)
        std::cout << buf[i] << ',' << out[i] << '\t';
    std::cout << std::endl;
    */
   
    fft(xr, xi, yr, yi, N, 1);
    maxr[0] = 0;
    maxi[0] = 0;
    for (int i = 0; i < nb_samples; i++) {
        if ((maxr[0] * maxr[0] + maxi[0] * maxi[0]) < (xr[i] * xr[i] + xi[i] * xi[i])) {
            maxr[0] = xr[i];
            maxi[0] = xi[i];
            maxk[0] = i;
        }
    }

    /*
    for (int i = 0; i < nb_samples; i++)
        std::cout << xr[i] << ',' << xi[i] << ' '; // << out[i] << '\t';
    std::cout << std::endl;
    */

    return 0;
}


int WaveExplorer::poll_frames()
{
    if (get_samples()) {
        return 0;
    }

    double maxr[8];
    double maxi[8];
    int maxk[8];

    get_keywaves(samples, nb_samples, maxr, maxi, maxk, 8);

    nb_slice= log2(nb_samples);
    if (nb_samples != pow(2, nb_slice))
        nb_slice = pow(2, nb_slice + 1);

    std::cout << maxr[0] << ' ' << maxi[0] << ' ' << maxk[0] << std::endl;

/*
    if (get_keywaves()) {
        return 0;
    }
*/
    //std::cout << "get_waves finished" << std::endl;

    return maxk[0];
}



// use cairo to draw waves 

void WaveExplorer::draw_samples(cairo_t *cr)
{
    cairo_set_source_rgba (cr, 0.2, 0.4, 0.6, 0.6);
    cairo_set_line_width (cr, 1.0);

    for (int i = 0; i < nb_samples; i++) {
        cairo_move_to(cr, i, 350);
        cairo_line_to(cr, i, 350 - ((int)samples[i] / 100));
    }

    cairo_stroke(cr);
    cairo_paint(cr);
}

void WaveExplorer::draw_waves(cairo_t *cr, double real, double imag, int k) 
{
    //cairo_set_source_rgba (cr, 0.8, 0.2, 0.2, 0.6);
    cairo_set_line_width (cr, 1.0);

    double PI = 3.141592653589793238460;

    double theta = 2.0 * PI * k / nb_slice;

    for (int i = 0; i < nb_samples; i++) {
        //Complex value = X * std::polar(1.0, i * theta);

        cairo_move_to(cr, i, 350);
        double value = 350 * cos(theta * i);
        cairo_line_to(cr, i, 350 + 300 * cos(theta * i));
        //cairo_line_to(cr, i, 380);
    }

    cairo_stroke(cr);
    cairo_paint(cr);
}

void WaveExplorer::draw_frames(const char *file_path, int maxk) 
{
    cairo_surface_t *surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, nb_samples, 700);
    cairo_t *cr = cairo_create(surface);
    
    draw_samples(cr);

    draw_waves(cr, 2, 2, maxk);

    cairo_destroy (cr);
    cairo_surface_write_to_png (surface, file_path);
    cairo_surface_destroy (surface);
}
