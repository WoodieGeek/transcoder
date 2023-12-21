#include "reader.h"

void Reader::ensure(int error_id) {
    char str[AV_ERROR_MAX_STRING_SIZE];
    throw std::runtime_error(av_make_error_string(str, AV_ERROR_MAX_STRING_SIZE, error_id));
}


Reader::Reader(std::string file_name, int l, int r): LEFT(l), RIGHT(r) {
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

    Seek(LEFT);
    for (int i = 0; i < in_format_ctx->nb_streams; ++i) {
        decoder_mas.emplace_back(in_format_ctx->streams[i]);
    }

    stream_is_finished.resize(in_format_ctx->nb_streams, false);
    cnt_finished_streams = 0;
}

std::vector<AVFrame*> Reader::ReadFrame() {
    AVPacket *packet = av_packet_alloc();
    if (!packet) {
        throw std::runtime_error("Could not allocate AVPacket");
    }

    if (cnt_finished_streams == in_format_ctx->nb_streams) {
        return {};
    }

    int ret = av_read_frame(in_format_ctx, packet);
    if (stream_is_finished[packet->stream_index]) {
        return {};
    }

    if (ret < 0) {
        std::cout << "Failded to read frame!\n";
        ensure(ret);
    }

    std::vector<AVFrame*> get_frames = decoder_mas[packet->stream_index].decode(packet);

    for (int i = 0; i < get_frames.size(); ++i) {
        if (get_frames[i]->pts > av_rescale_q(RIGHT * AV_TIME_BASE, AV_TIME_BASE_Q, in_format_ctx->streams[packet->stream_index]->time_base)) {
            get_frames.resize(i);

            stream_is_finished[packet->stream_index] = true;
            ++cnt_finished_streams;

            break;
        }
    }

    return get_frames;
}

void Reader::Seek(int time) {
    for (int stream_index = 0; stream_index < in_format_ctx->nb_streams; ++stream_index) {
        int time_base = av_rescale_q(time * AV_TIME_BASE, AV_TIME_BASE_Q, in_format_ctx->streams[stream_index]->time_base);
        int response_key = av_seek_frame(in_format_ctx, stream_index, time_base, AVSEEK_FLAG_BACKWARD);
        if (response_key < 0) {
            throw std::runtime_error("Error of seeking streams: process finished with incorrect exit code.");
        }
    }
}

std::vector<AVStream*> Reader::GetStreams() {
    std::vector<AVStream*> streams;
    for (int i = 0; i < in_format_ctx->nb_streams; i++)
        streams.push_back(in_format_ctx->streams[i]);

    return streams;
}
