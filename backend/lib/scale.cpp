#include "scale.h"

Scaler::Scaler(AVStream* stream, int out_width, int out_height, AVPixelFormat out_pixel_format): out_width(out_width), out_height(out_height) {
    resize_context = sws_getContext(stream->codecpar->width,
                                    stream->codecpar->height, AVPixelFormat(stream->codecpar->format), out_width, out_height,
                                    out_pixel_format, SWS_BILINEAR, nullptr, nullptr, nullptr);
}

AVFrame* Scaler::ScaleFrame(AVFrame* frame, AVPixelFormat out_pixel_format) {
    AVFrame* ready_frame = av_frame_alloc();

    ready_frame->width = out_width;
    ready_frame->height = out_height;
    ready_frame->format = out_pixel_format;
    ready_frame->pts = frame->pts;

    int response_key = av_frame_get_buffer(ready_frame, 32);
    if (response_key < 0) {
        throw std::runtime_error("Error of getting buffer: process finished with incorrect exit code.");
    }

    response_key = sws_scale(resize_context, frame->data, frame->linesize,
                             0, frame->height, ready_frame->data, ready_frame->linesize);

    if (response_key < 0) {
        throw std::runtime_error("Error of scaling frame: process finished with incorrect exit code.");
    }

    // av_frame_unref(frame);
    return ready_frame;
}