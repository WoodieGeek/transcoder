#include "encoder.h"


*Encoder::Encoder(par info) {
    const AVCodec *out_codec = avcodec_find_encoder_by_name(info.codec_name.c_str());
    out_codec_ctx = avcodec_alloc_context3(out_codec);
    out_codec_ctx->width = info.width;
    out_codec_ctx->height = info.height;
    out_codec_ctx->time_base = info.time_base;
    out_codec_ctx->pix_fmt = info.pix_fmt;
    out_codec_ctx->sample_aspect_ratio = info.sample_aspect_ratio;
    avcodec_open2(out_codec_ctx, out_codec, nullptr);
}

std::vector<AVPacket *> Encoder::encoder(AVFrame *Frame) {
    int ret = avcodec_send_frame(out_codec_ctx, Frame);
    if (ret < 0) {
        std::cout << ret << std::endl;
        throw std::runtime_error("Error sending frame for encoding");
    }


    std::vector<AVPacket *> res;
    while (ret >= 0) {
        AVPacket *pkt = av_packet_alloc();
        if (!pkt) {
            throw std::runtime_error("Could not allocate AVPacket");
        }
        ret = avcodec_receive_packet(out_codec_ctx, pkt);

        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
            break;
        } else if (ret < 0) {
            throw std::runtime_error("Could not allocate AVPacket");
        }
        res.emplace_back(pkt);
    }
    return res;
}