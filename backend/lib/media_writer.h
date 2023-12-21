#pragma "ones"
#include <bits/stdc++.h>
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
//#include <libswscale/swscale.h>
}

#include <iostream>
class MediaWriter{
private:
    bool is_write_header = 1;
    AVFormatContext *out_format_ctx;
public:
    std::string Filename;
    MediaWriter(const std::string filename);
    void add_stream(AVStream *stream);
    void write(AVPacket *packet);
    void close();
};