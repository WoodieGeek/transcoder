#include "media_writer.h"

MediaWriter::MediaWriter(std::string filename) {
    int ret = avformat_alloc_output_context2(&out_format_ctx, nullptr, nullptr, filename.data());
    if (ret < 0) {
        throw std::runtime_error("Error allocating format context");
    }
    avio_open(&out_format_ctx->pb, filename.data(), AVIO_FLAG_WRITE);
}
void MediaWriter::add_stream(AVStream *stream) {
    int ret = avformat_write_header(out_format_ctx, nullptr);
    if (ret < 0) {
        throw std::runtime_error("Error writing header");
    }
    AVStream *newStream = avformat_new_stream(out_format_ctx, nullptr);
    if (!newStream) {
        throw std::runtime_error("Error adding new stream");
    }
    newStream->time_base = stream->time_base;
}
void MediaWriter::write(AVPacket *packet) {
    int ret = av_write_frame(out_format_ctx, packet);
    if (ret < 0) {
        throw std::runtime_error("Error writing frame");
    }
}
void MediaWriter::close() {
    av_write_trailer(out_format_ctx);
    avio_close(out_format_ctx->pb);
}
