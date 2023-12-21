#pragma "ones"
#include <bits/stdc++.h>
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
//#include <libswscale/swscale.h>
}

#include <iostream>
class MediaWriter{
public:
    AVFormatContext *out_format_ctx;
    std::string Filename;
    MediaWriter(const std::string filename);
    void add_stream(AVStream *stream);
    void write(AVPacket *packet, bool is_first_call);
    void close();
};