#ifndef SN76489_H_
#define SN76489_H_
#include <Arduino.h>
#include "Bus.h"
#include "TrackStructs.h"
class SN76489
{
private:
    Bus * _bus;
    unsigned char psgFrqLowByte = 0;
    void Write(unsigned char data);
public:
    SN76489(Bus * bus);
    void Reset();
    void Send(unsigned char data);
};
#endif

//Notes
//SN_WE = PB5