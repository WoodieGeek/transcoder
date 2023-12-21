#include "media_writer.h"

MediaWriter::MediaWriter(std::string filename) {
    Filename = filename;
    int ret = avformat_alloc_output_context2(&out_format_ctx, nullptr, nullptr, filename.data());
    if (ret < 0) {
        throw std::runtime_error("Error allocating format context");
    }
}
void MediaWriter::add_stream(AVStream *stream) {
    AVStream *newStream = avformat_new_stream(out_format_ctx, nullptr);
    int ret = avcodec_parameters_copy(newStream->codecpar, stream->codecpar);

    if (ret < 0) {
        throw std::runtime_error("Error in writer");
    }

    if (!newStream) {
        throw std::runtime_error("Error adding new stream");
    }
    newStream->time_base = stream->time_base;
}
void MediaWriter::write(AVPacket *packet) {
    int ret;
    if (is_write_header) {
        is_write_header = 0;
        ret = avio_open(&out_format_ctx->pb, Filename.data(), AVIO_FLAG_WRITE);
        if (ret < 0) {
            throw std::runtime_error("av open failed");
        }

        ret = avformat_write_header(out_format_ctx, nullptr);
        if (ret < 0) {
            throw std::runtime_error("Error writing header");
        }
    }

    //av_packet_rescale_ts(packet, packet->time_base, out_format_ctx->streams[packet->stream_index]->time_base);
    ret = av_interleaved_write_frame(out_format_ctx, packet);
    av_packet_unref(packet);
    if (ret < 0) {
        throw std::runtime_error("Error writing frame");
    }
}

void MediaWriter::close() {
    av_write_trailer(out_format_ctx);
    avio_close(out_format_ctx->pb);
}
