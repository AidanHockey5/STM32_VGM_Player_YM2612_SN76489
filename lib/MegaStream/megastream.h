#ifndef MEGASTREAM_H
#define MEGASTREAM_H

#include <stddef.h>
#include <stdint.h>


typedef struct {
    uint8_t *buf;
    volatile size_t tail;
    volatile size_t head;
    size_t size;
} MegaStreamContext_t;
#ifdef __cplusplus
extern "C" {
#endif
void MegaStream_Create(MegaStreamContext_t *ctx, uint8_t *buf, size_t size);
void MegaStream_Reset(MegaStreamContext_t *ctx);
void MegaStream_Send(MegaStreamContext_t *ctx, uint8_t *inbuf, size_t insize);
void MegaStream_Recv(MegaStreamContext_t *ctx, uint8_t *outbuf, size_t outsize);
uint8_t MegaStream_Peek(MegaStreamContext_t *ctx);
size_t MegaStream_Used(MegaStreamContext_t *ctx);
size_t MegaStream_Free(MegaStreamContext_t *ctx);
#ifdef __cplusplus
}
#endif
#endif


/*
Example:

MegaStreamContext_t stream;
uint8_t bigbuffer[20000];
MegaStream_Create(&stream, bigbuffer, 20000);
*/