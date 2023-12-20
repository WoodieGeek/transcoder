#pragma once

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}

#include <stdexcept>
#include <vector>
#include <iostream>

class Decoder {
private:
    AVCodecContext* codec_context;

public:
    Decoder(AVStream* stream);
    std::vector<AVFrame*> decode(AVPacket* packet);
};
