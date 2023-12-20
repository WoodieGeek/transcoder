#pragma once

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}

#include <bits/stdc++.h>


struct par{
    AVRational time_base;
    int width, height;
    std::string codec_name;
    AVRational sample_aspect_ratio;
    AVPixelFormat pix_fmt;
};

class Encoder{
private:
    AVCodecContext *out_codec_ctx;
public:
    Encoder(par info);
    std::vector<AVPacket*> encoder(AVFrame *Frame);
};
