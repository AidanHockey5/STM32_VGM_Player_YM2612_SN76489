#ifndef _SN76489_H_
#define _SN76489_H_
#include "Bus.h"
#include "ChipClock.h"
#include "SpinSleep.h"

class SN76489
{
public:
    SN76489(Bus* _bus, uint8_t _WE);
    ~SN76489();
    void reset();
    void write(uint8_t data);
    void setClock(uint32_t frq);
private:
    //ChipClock* clk;
    uint32_t clkfrq;
    Bus* bus;
    uint8_t WE;
    unsigned char psgFrqLowByte = 0;
    void writeRaw(uint8_t data);
};

//clock bus we






#endif