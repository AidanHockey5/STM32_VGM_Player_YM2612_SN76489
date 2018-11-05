#ifndef SN76489_H_
#define SN76489_H_
#include <Arduino.h>
#include "Bus.h"
class SN76489
{
private:
    Bus * _bus;
public:
    SN76489(Bus * bus);
    void Reset();
    void SendDataPins(unsigned char data);
};
#endif

//Notes
//SN_WE = PB5