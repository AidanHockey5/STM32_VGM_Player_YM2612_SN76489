#include "megastream.h"
#include <string.h>

/*
 * This is based off FreeRTOS StreamBuffers which are really just ring buffers with some rtos-flavored stuff that we don't actually need.
 * 
 * Footguns:
 * - Only one thread can be a reader or a writer
 * - Don't write more than will fit
 * - Don't read more than is available, or peek when nothing is available
 * - Make sure nothing is reading or writing when resetting
*/

#define min(a,b) ((a) < (b) ? (a) : (b))

void MegaStream_Create(MegaStreamContext_t *ctx, uint8_t *buf, size_t size) {
    ctx->size = size;
    ctx->buf = buf;
    ctx->head = 0;
    ctx->tail = 0;
}

void MegaStream_Reset(MegaStreamContext_t *ctx) {
    ctx->head = 0;
    ctx->tail = 0;
}

void MegaStream_Send(MegaStreamContext_t *ctx, uint8_t *inbuf, size_t insize) {
    size_t nexthead = ctx->head;
    size_t firstlength = min(ctx->size-nexthead, insize);
    memcpy(&(ctx->buf[nexthead]), inbuf, firstlength);
    if (insize > firstlength) {
        memcpy(ctx->buf, &inbuf[firstlength], insize-firstlength);
    }
    nexthead += insize;
    if (nexthead >= ctx->size) {
        nexthead -= ctx->size;
    }
    ctx->head = nexthead;
}

void MegaStream_Recv(MegaStreamContext_t *ctx, uint8_t *outbuf, size_t outsize) {
    size_t nexttail = ctx->tail;
    size_t firstlength = min(ctx->size-nexttail, outsize);
    memcpy(outbuf, &(ctx->buf[nexttail]), firstlength);
    if (outsize > firstlength) {
        memcpy(&outbuf[firstlength], ctx->buf, outsize-firstlength);
    }
    nexttail += outsize;
    if (nexttail >= ctx->size) {
        nexttail -= ctx->size;
    }
    ctx->tail = nexttail;
}

uint8_t MegaStream_Peek(MegaStreamContext_t *ctx) {
    return ctx->buf[ctx->tail];
}

size_t MegaStream_Used(MegaStreamContext_t *ctx) {
    size_t c = ctx->size + ctx->head;
    c -= ctx->tail;
    if (c >= ctx->size) {
        c -= ctx->size;
    }
    return c;
}

size_t MegaStream_Free(MegaStreamContext_t *ctx) {
    size_t s = ctx->size + ctx->tail;
    s -= ctx->head;
    s--;
    if (s >= ctx->size) {
        s -= ctx->size;
    }
    return s;
}