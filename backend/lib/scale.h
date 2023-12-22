#pragma once

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}

#include <stdexcept>

class Scaler {
public:
    Scaler(AVStream*, int, int, AVPixelFormat = AV_PIX_FMT_YUV420P);
    AVFrame* ScaleFrame(AVFrame*, AVPixelFormat = AV_PIX_FMT_YUV420P);

private:
    struct SwsContext* resize_context;
    int out_width, out_height;
};
