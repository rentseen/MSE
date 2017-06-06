
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



#define AUDIO_INBUF_SIZE 20480
#define AUDIO_REFILL_THRESH 4096


int main(){

    AVFormatContext *formatContext = NULL;
	AVCodec *codec;
	AVCodecContext *codecContext = NULL;
	int out_size, len;
	FILE *in, *out;
	uint8_t inbuf[AUDIO_INBUF_SIZE + AV_INPUT_BUFFER_PADDING_SIZE];
	AVPacket avpkt;
    AVFrame *decoded_frame = NULL;

    // register all the codecs
    av_register_all();
    avcodec_register_all();

	av_init_packet(&avpkt);

	std::cout << "Audio decoding" << std::endl;

	// find the mpeg audio decoder
	codec = avcodec_find_decoder(AV_CODEC_ID_MP3);
	if (!codec) {
		std::cout << "Error codec not found" << std::endl;
		return 1;
	}

	codecContext = avcodec_alloc_context3(codec);
    if (!codecContext) {
        std::cout << "Error could not allocate audio codec context" << std::endl;
        return 1;
    }

	if (avcodec_open2(codecContext, codec, NULL) < 0) {
		std::cout << "Error could not open codec" << std::endl;
		return 1;
	}

    /*
	in = fopen("./mp3/1769167647.mp3", "rb");
	if (!in) {
		std::cout << "Error could not open input file" << std::endl;
		return 1;
	}
	out = fopen("./1769167647.wav", "wb");
	if (!out) {
        std::cout << "Error could not open output file" << std::endl;
        av_free(codecContext);
    	return 1;
	}
    */

    int result = avformat_open_input(&formatContext, "./mp3/1769167647.mp3", NULL, NULL);
    if (result < 0) {
        std::cout << "Error avformat could not open input file, " << result << std::endl;
        char errbuf[256];
        av_strerror(result, errbuf, 256);
        std::cout << errbuf << std::endl;
        return 1;
    }

    if (avformat_find_stream_info(formatContext,NULL) < 0) {  
  
        std::cout << "Error found stream info failed " << std::endl;
        return 1; 
    }  
  
    av_dump_format(formatContext, -1 , "./test.txt", 0);  


    /*
	// decode until eof
	avpkt.data = inbuf;
	avpkt.size = fread(inbuf, 1, AUDIO_INBUF_SIZE, in);

	while (avpkt.size > 0) {
        int i, ch;
        int got_frame = 0;

        if (!decoded_frame) {
            if (!(decoded_frame = av_frame_alloc())) {
                std::cout << "Error could not allocate audio frame" << std::endl;
                return 1;
            }
        }

		len = avcodec_decode_audio4(codecContext, decoded_frame, &got_frame, &avpkt);
		if (len < 0) {
			std::cout << "Error while decoding" << std::endl;
			return 1;
		}
        if (got_frame) {
            int data_size = av_get_bytes_per_sample(codecContext->sample_fmt);
            if (data_size < 0) {
                std::cout << "Error failed to calculate data size" << std::endl;
                return 1;
            }
            for (i = 0; i < decoded_frame->nb_samples; i++)
                for (ch = 0; ch < codecContext->channels; ch++)
                    fwrite(decoded_frame->data[ch] + data_size * i, 1, data_size, out);
        }

		avpkt.size -= len;
		avpkt.data += len;

        avpkt.dts =
        avpkt.pts = AV_NOPTS_VALUE;
		if (avpkt.size < AUDIO_REFILL_THRESH) {
			memmove(inbuf, avpkt.data, avpkt.size);
			avpkt.data = inbuf;
			len = fread(avpkt.data + avpkt.size, 1, AUDIO_INBUF_SIZE - avpkt.size, in);

			if (len > 0)
				avpkt.size += len;
		}
	}

	fclose(out);
	fclose(in);

    */
    avformat_close_input(&formatContext);
	avcodec_free_context(&codecContext);
	av_frame_free(&decoded_frame);

	return 0;
}
