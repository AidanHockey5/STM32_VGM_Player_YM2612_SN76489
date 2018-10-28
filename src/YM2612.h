#ifndef YM2612_H_
#define YM2612_H_
#include <Arduino.h>
#include "Bus.h"
class YM2612
{
private:
    int * _dataPins; //Digital I/O pins
    int _CS;
    int _RD;
    int _WR;
    int _A0;
    int _A1;
    int _IRQ;
    int _IC;
    void WriteDataPins(unsigned char data);
public:
    YM2612(int * dataPins, int CS, int RD, int WR, int A0, int A1, int IRQ, int IC);
    void Reset();
    void SendDataPins(unsigned char addr, unsigned char data, bool setA1);
};
#endif