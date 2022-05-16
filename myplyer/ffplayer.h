#ifndef FFPALYER_H
#define FFPALYER_H
#include <stdio.h>
extern "C"{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/pixdesc.h>
#include <libavutil/hwcontext.h>
#include <libavutil/opt.h>
#include <libavutil/avassert.h>
#include <libavutil/imgutils.h>
}
#include <QString>

#include "glyuvwidget2.h"
class player
{
public:
    player(glyuvwidget* glw);
    int start(const char * in);
private:
    int decode_write(AVCodecContext *avctx, AVPacket *packet);
    int hw_decoder_init(AVCodecContext *ctx, const enum AVHWDeviceType type);
    static enum AVPixelFormat get_hw_format(AVCodecContext *ctx,const enum AVPixelFormat *pix_fmts);

    AVBufferRef *hw_device_ctx = NULL;
    glyuvwidget* glwidget;

#endif // FFPALYER_H
};
