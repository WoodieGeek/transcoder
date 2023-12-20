extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
//#include <libswscale/swscale.h>
}

#include <iostream>
#include <vector>

void ensure(int error_id) {
    char str[AV_ERROR_MAX_STRING_SIZE];
    throw std::runtime_error(av_make_error_string(str, AV_ERROR_MAX_STRING_SIZE, error_id));
}

class Reader {
public:
    int ret;

    Reader(char* file_name_) {
        file_name = file_name_;
        ret = avformat_open_input(&in_format_ctx, file_name, nullptr, nullptr);
        if (ret < 0) {
            std::cout << "Failed to open input file " << file_name << "!\n";
            ensure(ret);
        }
        ret = avformat_find_stream_info(in_format_ctx, nullptr);
        if (ret < 0) {
            std::cout << "Failed to open input file " << file_name << "!\n";
            ensure(ret);
        }
    }

    AVFrame* ReadFrame() {
        ret = av_read_frame(in_format_ctx, &packet);
        if (ret < 0) {
            std::cout << "Failded to read frame!\n";
            ensure(ret);
        }

    }

    std::vector<AVStream*> GetStreams() {
        std::vector<AVStream*> streams;
        for (int i = 0; i < in_format_ctx->nb_streams; i++)
            streams.push_back(in_format_ctx->streams[i]);
        return streams;
    }
private:
    char* file_name;
    const char* output_codec_name = "libx264";
    AVFormatContext* in_format_ctx = nullptr;
    AVPacket packet;

};
