#ifndef SN76489_H_
#define SN76489_H_
#include <Arduino.h>
#include "Bus.h"
class SN76489
{
private:
    int * _dataPins; //Digital I/O pins
    int _WE;
public:
    SN76489(int * dataPins, int WE);
    void Reset();
    void SendDataPins(unsigned char data);
};
#endif