#include "decoder.h"

Decoder::Decoder(AVStream* stream) {
    const AVCodec* codec = avcodec_find_decoder(stream -> codecpar -> codec_id);
    codec_context = avcodec_alloc_context3(codec);

    int response_key = avcodec_parameters_to_context(codec_context, stream -> codecpar);
    if (response_key < 0) {
        throw std::runtime_error("Error of Decoder initialization process finished with incorrect exit code.");
    }

    response_key = avcodec_open2(codec_context, codec, nullptr);
    if (response_key < 0) {
        throw std::runtime_error("Error of Decoder initialization: process finished with incorrect exit code.");
    }
}

std::vector<std::pair<AVFrame*, int>> Decoder::decode(AVPacket* packet, int stream_index) {
    int response_key = avcodec_send_packet(codec_context, packet);

    if (response_key < 0) {
        throw std::runtime_error("Error of sending packet: process finished with incorrect exit code.");
    }

    std::vector<std::pair<AVFrame*, int>> frame_list;
    while (response_key >= 0) {
        AVFrame* single_frame = av_frame_alloc();
        response_key = avcodec_receive_frame(codec_context, single_frame);

        if (response_key == AVERROR(EAGAIN) || response_key == AVERROR_EOF) {
            break;
        } else if (response_key < 0) {
            throw std::runtime_error("Error of getting frame: process finished with incorrect exit code.");
        }

        frame_list.emplace_back(single_frame, stream_index);
    }

    return frame_list;
}
