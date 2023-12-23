#pragma once
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}

#include <iostream>
#include <vector>
#include "decoder.h"

class Reader {
public:
    void ensure(int error_id);

    Reader(std::string, int, int);

    std::vector<std::pair<AVFrame*, int>> ReadFrame();
    std::vector<AVStream*> GetStreams();
private:
    void Seek(int);
    std::string file_name;
    AVFormatContext* in_format_ctx = nullptr;

    std::vector<bool> stream_is_closed;
    int cnt_closed_streams;

    int LEFT, RIGHT;
    std::vector<Decoder*> decoder_mas;
};

