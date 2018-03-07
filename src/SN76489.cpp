#include "SN76489.h"

SN76489::SN76489(int * dataPins, int WE)
{
    _dataPins = dataPins;
    _WE = WE;
    for(int i = 0; i<8; i++)
    {
        pinMode(*(_dataPins+i), OUTPUT);
        digitalWrite(*(_dataPins+i), LOW);
    }
    pinMode(_WE, OUTPUT);
    digitalWrite(_WE, HIGH);
}

void SN76489::Reset()
{
  SendDataPins(0x9f);
  SendDataPins(0xbf);
  SendDataPins(0xdf);
  SendDataPins(0xff);  
}

void SN76489::SendDataPins(unsigned char data)
{
    digitalWrite(_WE, HIGH);
    for(int i=0; i<8; i++)
    {
      digitalWrite(*(_dataPins+i), ((data >> i)&1));
    }
    digitalWrite(_WE, LOW);
    delayMicroseconds(13);
    digitalWrite(_WE, HIGH);
}