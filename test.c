#include <stdio.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
///usr/include/x86_64-linux-gnu 헤더파일경로
///usr/lib/x86_64-linux-gnu 라이브러리 경로
// gcc -o test test.c -I/usr/include/x86_64-linux-gnu -L/usr/lib/x86_64-linux-gnu -lavformat -lavcodec -lavutil -lswscale -lswresample
// 컴파일 방법 옵션추가 ffmpeg

#include "test.h"
int main() {
    avformat_network_init();
    AVFormatContext *format_ctx = NULL;
    avformat_open_input(&format_ctx, "./videos/adidas1.mp4", NULL, NULL);
    avformat_find_stream_info(format_ctx, NULL);

    int video_stream_index = -1;
    for (int i = 0; i < format_ctx->nb_streams; i++) {
        if (format_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            video_stream_index = i;
            break;
        }
    }

    AVCodecParameters *codec_params = format_ctx->streams[video_stream_index]->codecpar;
    AVCodec *codec = avcodec_find_decoder(codec_params->codec_id);
    AVCodecContext *codec_ctx = avcodec_alloc_context3(codec);
    avcodec_parameters_to_context(codec_ctx, codec_params);
    avcodec_open2(codec_ctx, codec, NULL);

    AVFrame *frame = av_frame_alloc();
    AVPacket packet;
    while (av_read_frame(format_ctx, &packet) >= 0) {
        if (packet.stream_index == video_stream_index) {
            avcodec_send_packet(codec_ctx, &packet);
            avcodec_receive_frame(codec_ctx, frame);
            // Process the frame here
        }
        av_packet_unref(&packet);
    }

    av_frame_free(&frame);
    avcodec_free_context(&codec_ctx);
    avformat_close_input(&format_ctx);
    printf("%s\n",ending);

    //////////////////////////////////////////
    unsigned version = avcodec_version();
    unsigned major = (version >> 16) & 0xFF;
    unsigned minor = (version >> 8) & 0xFF;
    unsigned micro = version & 0xFF;

    printf("FFmpeg libavcodec version: %u.%u.%u\n", major, minor, micro);

    return 0;
}