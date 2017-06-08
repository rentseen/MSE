
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

extern "C" 
{
	#include "libavcodec/avcodec.h"
	#include "libavformat/avformat.h"
	#include "libavutil/avutil.h"
    //#include "libavutil/avstring.h"
	//#include "libavutil/mathematics.h"
}




int main(){

    AVFormatContext *format_ctx = NULL;
	AVCodecContext *codec_ctx = NULL;
    AVCodec *codec = NULL;

	AVPacket packet;
    AVFrame *frame = NULL;

    // register all the codecs
    av_register_all();
    avcodec_register_all();

    // autodetecting the format and read the header, exporting the information
    int result = avformat_open_input(&format_ctx, "./mp3/1769167647.mp3", NULL, NULL);
    if (result < 0) {
        std::cout << "Error avformat could not open input file, " << result << std::endl;
        char errbuf[256];
        av_strerror(result, errbuf, 256);
        std::cout << errbuf << std::endl;
        return 1;
    }
    
    // useful for file formats with no headers such as MPEG
    if (avformat_find_stream_info(format_ctx, NULL) < 0) {  
        std::cout << "Error found stream info failed " << std::endl;
        return 1; 
    }  
  
    // print the infomation
    av_dump_format(format_ctx, -1 , "./test.txt", 0);  


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
    std::cout << "codec_id=" << codec_ctx->codec_id << std::endl;
	if (codec == NULL) {
		std::cout << "Error codec not found" << std::endl;
		return 1;
	}
	
    if (avcodec_open2(codec_ctx, codec, NULL) < 0) {
		std::cout << "Error could not open codec" << std::endl;
		return 1;
	}

    frame = av_frame_alloc();
    if (frame == NULL) {
        std::cout << "Error could not alloc frame" << std::endl;
        return 1;
    }


    int size, len = 0, cnt_frame = 0, got_frame = 0;
    while (av_read_frame(format_ctx, &packet) >= 0) {
        size = packet.size;

        while (size > 0) {
            len = avcodec_decode_audio4(codec_ctx, frame, &got_frame, &packet);
            if (len < 0) {
                std::cout << "Error while decoding 1" << std::endl;
                return 1;
            }
            //std::cout << "decoded len = " << len << std::endl;

            if (got_frame) {
                int data_size = av_get_bytes_per_sample(codec_ctx->sample_fmt);
                if (data_size < 0) {
                    std::cout << "Error failed to calculate data size" << std::endl;
                    return 1;
                }

                // format is AV_SAMPLE_FMT_S16P, signed 16 bits, planar 
                // planar, data[i] contains sample of channle i
                if (cnt_frame == 0) {
                    std::cout << "format=" << frame->format << '/' << AV_SAMPLE_FMT_S16P << 
                        " nb_samples=" << frame->nb_samples <<
                        " sample_size=" << data_size << 
                        " channels=" << codec_ctx->channels << 
                        " pkt_duration=" << frame->pkt_duration << 
                        " pts=" << frame->pts << std::endl;
                        
                
                }
                
                if (cnt_frame == 5000){
                    for (int i = 0; i < frame->nb_samples; i++) {
                        if ( i != 500)
                            continue;
                        for (int j = 0; j < frame->nb_samples; j += 2) {
                            short sample = 0;
                            sample |= frame->data[0][j+1];
                            sample <<= 8;
                            sample |= frame->data[0][j];
                            std::cout << sample << '\t';
                        }
                        std::cout << std::endl << std::endl << std::endl;
                    }
                }

                // unreference all buffers referenced by the frame
                av_frame_unref(frame);
                cnt_frame++;
            }
            size -= len;
        }
        av_free_packet(&packet);
    }

    std::cout << "cnt_frame = " << cnt_frame << std::endl;
    std::cout << "fmt_ctx duration = " << format_ctx->duration << std::endl;

    avformat_close_input(&format_ctx);
	av_frame_free(&frame);

	return 0;
}
