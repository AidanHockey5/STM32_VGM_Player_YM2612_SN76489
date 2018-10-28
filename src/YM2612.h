#ifndef YM2612_H_
#define YM2612_H_
#include <Arduino.h>
#include "Bus.h"
class YM2612
{
private:
    Bus * _bus;
    void WriteDataPins(unsigned char data);
public:
    YM2612(Bus * bus);
    void Reset();
    void SendDataPins(unsigned char addr, unsigned char data, bool setA1);
};
#endif

//Notes
//YM_CS = PB3
//YM_RD = PA15
//YM_WR = PA12
//YM_A0 = PA11
//YM_A1 = PB0
//YM_IC = PB4 
