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

const std::string input_file = "../in.mp4";
const char* output_file = "../out.mp4";

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

int main(int argc, char* argv[]) {
    const int time_start = 0, time_end = 10;

    Reader reader(input_file, time_start, time_end);

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
            codec_options.width = 256;
            codec_options.height = 144;
            codec_options.codec_name = "libx264";

            encoders[stream->index] = new Encoder(codec_options);
            scalers[i] = new Scaler(stream, 256, 144);
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
