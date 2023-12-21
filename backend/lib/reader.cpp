#include "reader.h"

void Reader::ensure(int error_id) {
    char str[AV_ERROR_MAX_STRING_SIZE];
    throw std::runtime_error(av_make_error_string(str, AV_ERROR_MAX_STRING_SIZE, error_id));
}


Reader::Reader(std::string file_name) {
    int ret = avformat_open_input(&in_format_ctx, file_name.c_str(), nullptr, nullptr);
    if (ret < 0) {
        std::cout << "Failed to open input file " << file_name << "!\n";
        ensure(ret);
    }
    ret = avformat_find_stream_info(in_format_ctx, nullptr);
    if (ret < 0) {
        std::cout << "Failed to open input file " << file_name << "!\n";
        ensure(ret);
    }
    for (int i = 0; i < in_format_ctx->nb_streams; ++i) {
        decoder_mas.emplace_back(in_format_ctx->streams[i]);
    }
}

std::vector<std::pair<AVFrame*, int>> Reader::ReadFrame() {
    AVPacket *packet = av_packet_alloc();
    if (!packet) {
        throw std::runtime_error("Could not allocate AVPacket");
    }
    int ret = av_read_frame(in_format_ctx, packet);
    if (ret < 0) {
        std::cout << "Failded to read frame!\n";
        ensure(ret);
    }
    std::vector<std::pair<AVFrame*, int>> get_frames = decoder_mas[packet->stream_index].decode(packet, packet->stream_index);
    return get_frames;
}

std::vector<AVStream*> Reader::GetStreams() {
    std::vector<AVStream*> streams;
    for (int i = 0; i < in_format_ctx->nb_streams; i++)
        streams.push_back(in_format_ctx->streams[i]);
    return streams;
}



