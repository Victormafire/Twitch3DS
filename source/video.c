/**
 *@file video.c
 *@author Lectem
 *@date 14/06/2015
 */
#include <3ds.h>
#include <3ds/gfx.h>

#include "video.h"


int video_open_stream(StreamState *ss)
{
    if (ss->videoStream == -1)
        return -1;

    // Get a pointer to the codec context for the video stream
    ss->pCodecCtxOrig = ss->pFormatCtx->streams[ss->videoStream]->codec;
    // Find the decoder for the video stream
    ss->pCodec = avcodec_find_decoder(ss->pCodecCtxOrig->codec_id);
    if (ss->pCodec == NULL)
    {
        fprintf(stderr, "Unsupported video codec!\n");
        return -1; // Codec not found
    }
    else
    {
        printf("video decoder : %s - OK\n", ss->pCodec->name);
    }

    // Copy context
    ss->pCodecCtx = avcodec_alloc_context3(ss->pCodec);
    if (avcodec_copy_context(ss->pCodecCtx, ss->pCodecCtxOrig) != 0)
    {
        fprintf(stderr, "Couldn't copy video codec context");
        return -1; // Error copying codec context
    }
    // Open codec
    if (avcodec_open2(ss->pCodecCtx, ss->pCodec, NULL) < 0)
    {
        printf("Could not open codec\n");
        return -1;
    }
    return 0;
}

void display(AVFrame *pFrame)
{
//    gspWaitForVBlank();
//    gfxSwapBuffers();
    int i, j, c;
    const int width = 400 <= pFrame->width ? 400 : pFrame->width;
    const int height = 240 <= pFrame->height ? 240 : pFrame->height;
    if (gfxGetScreenFormat(GFX_TOP) == GSP_BGR8_OES)
    {
        u8 *const src = pFrame->data[0];
        u8 *const fbuffer = gfxGetFramebuffer(GFX_TOP, GFX_LEFT, 0, 0);

        for (i = 0; i < width; ++i)
        {
            for (j = 0; j < height; ++j)
            {
                for (c = 0; c < 3; ++c)
                {
                    fbuffer[3 * 240 * i + (239 - j) * 3 + c] = src[1024 * 3 * j + i * 3 + c];
                }
            }
        }
    }
    else if (gfxGetScreenFormat(GFX_TOP) == GSP_RGBA8_OES)
    {

        u32 *const src = (u32 *) pFrame->data[0];
        u32 *const fbuffer = (u32 *) gfxGetFramebuffer(GFX_TOP, GFX_LEFT, 0, 0);
        for (i = 0; i < width; ++i)
        {
            for (j = 0; j < height; ++j)
            {
                fbuffer[240 * i + (239 - j)] = src[1024 * j + i];
            }
        }
    }
    else
    {
        printf("format not supported\n");
    }
}
