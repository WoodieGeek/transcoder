extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
//#include <libswscale/swscale.h>
}

#include <iostream>
#include <utility>

#include "lib/encoder.h"
#include "lib/decoder.h"
#include "lib/reader.h"
#include "lib/media_writer.h"


static void save_frame(AVFrame* frame)
{

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

    const char* input_file = "../test.mp4";
    const char* output_file = "../out.mp4";

    Reader reader(input_file, 0, 100);
    auto streams = reader.GetStreams();

    std::vector<Encoder*> encoders((int)streams.size());

    MediaWriter Writer(output_file);

    for (AVStream* i : streams) {
        if (i->index == 0) continue;
        Writer.add_stream(i);

        if (i->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            encoders[i->index] = new Encoder({i->time_base, i->codecpar->width, i->codecpar->height, "libx264", i->sample_aspect_ratio, AV_PIX_FMT_YUV420P});
        }
        if (i->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            encoders[i->index] = new Encoder({i->time_base, 0, 0, "aac", i->sample_aspect_ratio, AV_PIX_FMT_YUV420P});
        }
    }

    int cnt = 0;
    while (true) {
        std::vector<std::pair<AVFrame *, int>> Frames = reader.ReadFrame();

        if (Frames[0].first == nullptr) break;

        bool is_first_call = 1;
        for (std::pair<AVFrame *, int> i: Frames) {
            if (i.second == 0) continue;

            ++cnt;
            std::vector<AVPacket *> pkt_to_writer = encoders[i.second]->encoder(i.first);

            for (AVPacket *pkt: pkt_to_writer) {
                std::cout << pkt->size << std::endl;
                pkt->stream_index = 0;
                Writer.write(pkt);
            }

        }
    }

    Writer.close();
    std::cout << cnt << '\n';
    return 0;
}
