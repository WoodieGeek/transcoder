extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}

#include <iostream>
#include <utility>
#include "lib/encoder.h"
#include "lib/decoder.h"
#include "lib/reader.h"
#include "lib/media_writer.h"

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

int main(int argc, char* argv[]) {
    if (argc < 5) {
        assert(false);
    }

    std::vector<std::string> arg_mas;
    for (int i = 0; i < argc; ++i) arg_mas.emplace_back(argv[i]), std::cout << arg_mas[i] << std::endl;

    const char* input_file = arg_mas[1].c_str();
    const char* output_file = arg_mas[2].c_str();

    Reader reader(input_file, stoi(arg_mas[3]), stoi(arg_mas[4]));
    auto streams = reader.GetStreams();

    std::vector<Encoder*> encoders((int)streams.size());

    MediaWriter Writer(output_file);

    for (AVStream* i : streams) {
        Writer.add_stream(i);

        par codec_options;
        codec_options.time_base = i->time_base;
        codec_options.sample_aspect_ratio = i->sample_aspect_ratio;
        codec_options.pix_fmt = AV_PIX_FMT_YUV420P;
        codec_options.sample_rate = i->codecpar->sample_rate;
        codec_options.channels = 2;
        codec_options.channel_layout = AV_CH_LAYOUT_STEREO;
        codec_options.sample_fmt = (AVSampleFormat)i->codecpar->format;
        if (i->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            codec_options.width = i->codecpar->width;
            codec_options.height = i->codecpar->height;
            codec_options.codec_name = "libx264";
            encoders[i->index] = new Encoder(codec_options);
        }
        if (i->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            codec_options.codec_name = "aac";
            encoders[i->index] = new Encoder(codec_options);
        }
    }

    while (true) {
        std::vector<std::pair<AVFrame*, int>> Frames = reader.ReadFrame();

        if (Frames.empty()) continue;
        if (Frames[0].first == nullptr) break;

        for (std::pair<AVFrame*, int> i: Frames) {

//          if (i.second == 0) continue;
            std::vector<AVPacket*> pkt_to_writer = encoders[i.second]->encoder(i.first);

            for (AVPacket *pkt: pkt_to_writer) {
                pkt->stream_index = i.second;

                Writer.write(pkt);
                av_packet_unref(pkt);
            }

            av_frame_unref(i.first);
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