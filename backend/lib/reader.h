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

    Reader(std::string file_name_);

    std::vector<AVFrame*> ReadFrame();
    std::vector<AVStream*> GetStreams();
private:
    std::string file_name;
    AVFormatContext* in_format_ctx = nullptr;
    std::vector<Decoder> decoder_mas;
};
