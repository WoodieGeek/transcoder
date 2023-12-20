#include <bits/stdc++.h>
#include <vector>
#include "decoder.h"

#include <iostream>

static void save_frame(AVFrame* frame)
{

    unsigned char *buf = (unsigned char*)frame->data[0];
    int wrap = frame->linesize[0];
    int xsize = frame->width;
    int ysize = frame->height;
    std::cout << xsize << ": " << ysize << std::endl;
    const char *filename = "one_frame.pbm";

    FILE *f;
    f = fopen(filename,"w");

    fprintf(f, "P5\n%d %d\n%d\n", xsize, ysize, 255);

    int i = 0;
    for (i = 0; i < ysize; i++)
        fwrite(buf + i * wrap, 1, xsize, f);
    fclose(f);
}

int main() {
    const char *input_file = "../in.mp4";
    const char *output_file = "../out.mp4";

    const char* output_codec_name = "libx264";

    AVFormatContext* in_format_ctx = nullptr;
    avformat_open_input(&in_format_ctx, input_file, nullptr, nullptr);
    avformat_find_stream_info(in_format_ctx, nullptr);
    int video_stream_idx = av_find_best_stream(in_format_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);

    std::cout << "video index:" << video_stream_idx << std::endl;
    AVStream* in_stream = in_format_ctx->streams[video_stream_idx];

    AVCodec* in_codec = avcodec_find_decoder(in_format_ctx->streams[video_stream_idx]->codecpar->codec_id);
    AVCodecContext* in_codec_ctx = avcodec_alloc_context3(in_codec);
    avcodec_parameters_to_context(in_codec_ctx, in_format_ctx->streams[video_stream_idx]->codecpar);
    avcodec_open2(in_codec_ctx, in_codec, nullptr);

    AVFormatContext* out_format_ctx = nullptr;
    avformat_alloc_output_context2(&out_format_ctx, nullptr, nullptr, output_file);
    AVStream* out_stream = avformat_new_stream(out_format_ctx, in_codec);

    AVCodec *out_codec = avcodec_find_encoder_by_name(output_codec_name);
    AVCodecContext *out_codec_ctx = avcodec_alloc_context3(out_codec);
    avcodec_parameters_to_context(out_codec_ctx, in_format_ctx->streams[video_stream_idx]->codecpar);
    out_codec_ctx->time_base = in_stream->time_base;

    int ret = avcodec_open2(out_codec_ctx, out_codec, nullptr);
    if (ret < 0) {
        char str[AV_ERROR_MAX_STRING_SIZE];
        throw std::runtime_error(av_make_error_string(str, AV_ERROR_MAX_STRING_SIZE, ret));
    }

    avcodec_parameters_from_context(out_stream->codecpar, out_codec_ctx);
    avio_open(&out_format_ctx->pb, output_file, AVIO_FLAG_WRITE);
    int res = avformat_write_header(out_format_ctx, nullptr);
    std::cout << res << std::endl;

    AVPacket* packet = av_packet_alloc();

    auto decoder = Decoder(in_stream);
    while (av_read_frame(in_format_ctx, packet) >= 0) {
        if (packet -> stream_index == video_stream_idx) {
            auto frames = decoder.decode(packet);

            for (auto &frame : frames) {
                ret = avcodec_send_frame(out_codec_ctx, frame);
                if (ret < 0) {
                    std::cout << ret << std::endl;
                    throw std::runtime_error("Error sending frame for encoding");
                }

                AVPacket* pkt = av_packet_alloc();
                if (!pkt) {
                    throw std::runtime_error("Could not allocate AVPacket");
                }

                while (ret >= 0) {
                    ret = avcodec_receive_packet(out_codec_ctx, pkt);
                    if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
                        break;
                    } else if (ret < 0) {
                        throw std::runtime_error("Could not allocate AVPacket");
                    }

                    av_packet_rescale_ts(pkt, in_stream->time_base, out_codec_ctx->time_base);
                    packet->stream_index = out_stream->index;

                    av_interleaved_write_frame(out_format_ctx, pkt);
                    av_packet_unref(pkt);
                }

                av_frame_unref(frame);
            }
        }
    }

    av_write_trailer(out_format_ctx);
    avio_close(out_format_ctx->pb);
    avformat_free_context(out_format_ctx);
    avcodec_free_context(&in_codec_ctx);
    avcodec_free_context(&out_codec_ctx);
    avformat_close_input(&in_format_ctx);

    return 0;
}