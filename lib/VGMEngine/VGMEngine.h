#ifndef _VGM_ENGINE_H_
#define _VGM_ENGINE_H_

#include "DeviceEnable.h" //YOU MUST ENABLE EVERY DEVICE YOU PLAN TO USE FIRST!

#include "VGMHeader.h"
#include "megastream.h"
#include "GD3.h"

#include "SPIRAM.h"
#include "YM2612.h"
#include "SN76489.h"

#define min(a,b) ((a) < (b) ? (a) : (b))
#define MAX_PCM_BUFFER_SIZE 124000

class VGMEngineClass
{
public:
    VGMEngineClass();
    ~VGMEngineClass();
    bool begin(File *f);
    VGMHeader header;
    GD3 gd3;
    #if ENABLE_SPIRAM
        SPIRAM ram = SPIRAM(PB12);
    #endif
    #if ENABLE_SN76489
        SN76489* sn76489;
    #endif
    #if ENABLE_YM2612 
        YM2612* ym2612; 
    #endif
    bool load(bool singleChunk = false);
    void tick();
    bool play();
    uint16_t getLoops();
    uint16_t maxLoops = 3;
    bool resetISR = false;

private:
    File* file;
    static const uint32_t VGM_BUF_SIZE = 8192;
    volatile int32_t waitSamples = 0;
    volatile bool ready = false;
    bool bufLock = false;
    uint32_t pcmBufferPosition = 0;
    uint32_t loopPos = 0; //The location of the 0x66 command
    uint16_t loopCount = 0;
    MegaStreamContext_t stream;
    uint8_t buf[VGM_BUF_SIZE];
    uint8_t readBufOne(); 
    uint16_t readBuf16();
    uint32_t readBuf32();

    // void timerConfig();
    // void startTimer();

    void setClocks();
    void chipSetup();
    bool storePCM(bool skip = false);
    bool topUp();
    uint16_t parseVGM();
    

    
};

//<3 natalie
const static unsigned char CommandLengthLUT[256] = {
  /*0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f*/
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //0x00
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //0x10
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //0x20
    2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, //0x30
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, //0x40
    2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, //0x50
    0, 3, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, //0x60
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, //0x70
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, //0x80
    5, 5, 6,11, 2, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //0x90
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, //0xa0
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, //0xb0
    4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, //0xc0
    4, 4, 4, 4, 4, 4, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, //0xd0
    5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //0xe0
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //0xf0
};

inline uint8_t VgmCommandLength(uint8_t Command) //including the command itself. only for fixed size commands
{ 
    //Also natalie, tyvm
    uint8_t val = CommandLengthLUT[Command];
    if (val) return val;
    return 0xFF;
}

extern VGMEngineClass VGMEngine;
#endif