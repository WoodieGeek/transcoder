extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
//#include <libswscale/swscale.h>
}

#include <iostream>
#include <utility>
#include "encoder.h"
#include "decoder.h"
#include "reader.h"
#include "media_writer.h"

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

    Reader reader(input_file);
    auto streams = reader.GetStreams();

    std::vector<Encoder*> encoders((int)streams.size());

    MediaWriter Writer(input_file);

    for (AVStream* i : streams) {
        Writer.add_stream(i);

        if (i->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            encoders[(int)i->index] = new Encoder({i->time_base, i->codecpar->width, i->codecpar->height, "libx264", i->sample_aspect_ratio, AV_PIX_FMT_YUV420P});
        }
        if (i->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            encoders[i->index] = new Encoder({i->time_base, 0, 0, "libx264", i->sample_aspect_ratio, AV_PIX_FMT_YUV420P});
        }
    }

    std::vector<std::pair<AVFrame*, int>> Frames = reader.ReadFrame();


    for (std::pair<AVFrame*, int> i : Frames) {
        std::vector<AVPacket *> pkt_to_writer = encoders[i.second]->encoder(i.first);
        for (AVPacket* pkt : pkt_to_writer) Writer.write(pkt);
    }

    av_write_trailer(out_format_ctx);
    avio_close(out_format_ctx->pb);
    avformat_free_context(out_format_ctx);
    avcodec_free_context(&in_codec_ctx);
    avcodec_free_context(&out_codec_ctx);
    avformat_close_input(&in_format_ctx);

    return 0;
}