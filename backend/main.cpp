extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}

#include <iostream>
#include <utility>
#include "lib/encoder.h"
#include "lib/decoder.h"
#include "lib/reader.h"
#include "lib/media_writer.h"
#include "lib/scale.h"

void ensure(int error_id) {
    char str[AV_ERROR_MAX_STRING_SIZE];
    throw std::runtime_error(av_make_error_string(str, AV_ERROR_MAX_STRING_SIZE, error_id));
}

static void save_frame(AVFrame* frame) {
    unsigned char *buf = (unsigned char*)frame->data[0];
    int wrap = frame->linesize[0];
    int xs = frame->width;
    int ys = frame->height;
    std::cout << xs << ": " << ys << std::endl;
    const char *filename = "one_frame.pbm";

    FILE *f;
    f = fopen(filename,"w");

    fprintf(f, "P5\n%d %d\n%d\n", xs, ys, 255);

    int i = 0;
    for (i = 0; i < ys; i++)
        fwrite(buf + i * wrap, 1, xs, f);
    fclose(f);
}

void defineResolutions(std::map<std::string, std::pair<int, int>> &resolutions) {
    resolutions["144p"] = {256, 144};
    resolutions["240p"] = {426, 240};
    resolutions["360p"] = {640, 360};
    resolutions["480p"] = {854, 480};
    resolutions["720p"] = {1280, 720};
    resolutions["1080p"] = {1920, 1080};
    resolutions["1440p"] = {2560, 1440};
    resolutions["2160p"] = {3840, 2160};
    resolutions["4320p"] = {7680, 4320};
}

int main(int argc, char* argv[]) {
    std::map<std::string, std::pair<int, int>> resolutions;
    defineResolutions(resolutions);

    if (argc < 6) {
        throw std::runtime_error("Too few arg for main.");
    }

    std::vector<std::string> arg_mas;
    for (int i = 0; i < argc; ++i) arg_mas.emplace_back(argv[i]), std::cout << arg_mas[i] << std::endl;

    const char* input_file = arg_mas[1].c_str();
    const char* output_file = arg_mas[2].c_str();

    Reader reader(input_file, stoi(arg_mas[3]), stoi(arg_mas[4]));

    if (!resolutions.count(arg_mas[5])) {
        throw std::runtime_error("Incorrect resolution provided.");
    }

    auto width = resolutions[arg_mas[5]].first, height = resolutions[arg_mas[5]].second;

    auto streams = reader.GetStreams();

    std::map<int, Encoder*> encoders;
    std::map<int, Scaler*> scalers;

    MediaWriter Writer(output_file);

    for (int i = 0; i < streams.size(); ++i) {
        auto stream = streams[i];
        Writer.add_stream(stream);

        par codec_options;
        codec_options.time_base = stream->time_base;
        codec_options.sample_aspect_ratio = stream->sample_aspect_ratio;
        codec_options.pix_fmt = AV_PIX_FMT_YUV420P;
        codec_options.sample_rate = stream->codecpar->sample_rate;
        codec_options.channels = 2;
        codec_options.channel_layout = AV_CH_LAYOUT_STEREO;
        codec_options.sample_fmt = (AVSampleFormat)stream->codecpar->format;

        if (stream->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            codec_options.width = width;
            codec_options.height = height;
            codec_options.codec_name = "libx264";

            encoders[stream->index] = new Encoder(codec_options);
            scalers[i] = new Scaler(stream, width, height);
        }

        if (stream->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            codec_options.codec_name = "aac";
            encoders[stream->index] = new Encoder(codec_options);
        }
    }

    while (true) {
        std::vector<std::pair<AVFrame*, int>> Frames = reader.ReadFrame();

        if (Frames.empty()) continue;
        if (Frames[0].first == nullptr) break;

        for (std::pair<AVFrame*, int> frame: Frames) {
            AVFrame* ready_frame = av_frame_alloc();

            if (scalers.count(frame.second)) {
                ready_frame = scalers[frame.second]->ScaleFrame(frame.first);
            } else {
                ready_frame = frame.first;
            }

//          if (i.second == 0) continue;
            std::vector<AVPacket*> pkt_to_writer = encoders[frame.second]->encoder(ready_frame);

            for (AVPacket *pkt: pkt_to_writer) {
                pkt->stream_index = frame.second;

                Writer.write(pkt);
                av_packet_unref(pkt);
            }

            av_frame_unref(frame.first);
            av_frame_unref(ready_frame);
        }
    }

    for (int i = 0; i < (int)streams.size(); ++i) {
        auto mas = encoders[i]->encoder(nullptr);
        for (AVPacket *pkt: mas) {
            pkt->stream_index = i;
            Writer.write(pkt);
        }
    }

    Writer.close();
    return 0;
}
