#include "YM2612.h"
YM2612::YM2612(int * dataPins, int CS, int RD, int WR, int A0, int A1, int IRQ, int IC)
{
    _dataPins = dataPins;
    _CS = CS;
    _RD = RD;
    _WR = WR;
    _A0 = A0;
    _A1 = A1;
    _IRQ = IRQ;
    _IC = IC;
    for(int i = 0; i<8; i++)
    {
        pinMode(*(_dataPins+i), OUTPUT);
        digitalWrite(*(_dataPins+i), LOW);
    }

    pinMode(_CS, OUTPUT);
    pinMode(_RD, OUTPUT);
    pinMode(_WR, OUTPUT);
    pinMode(_A0, OUTPUT);
    pinMode(_A1, OUTPUT);
    pinMode(_IC, OUTPUT);

    if(_IRQ != NULL)
    {
        pinMode(_IRQ, INPUT);
        digitalWrite(_IRQ, LOW);
    }

    digitalWrite(_CS, HIGH);
    digitalWrite(_RD, HIGH);
    digitalWrite(_WR, HIGH);
    digitalWrite(_A0, LOW);
    digitalWrite(_A1, LOW);
    digitalWrite(_IC, HIGH);
}

void YM2612::Reset()
{
    digitalWrite(_IC, LOW);
    delayMicroseconds(25);
    digitalWrite(_IC, HIGH);
}

void YM2612::WriteDataPins(unsigned char data) //Digital I/O
{
    for(int i=0; i<8; i++)
    {
      digitalWrite(*(_dataPins+i), ((data >> i)&1));
    }
}

void YM2612::SendDataPins(unsigned char addr, unsigned char data, bool setA1) //0x52 = A1 LOW, 0x53 = A1 HIGH
{
      digitalWrite(_A1, setA1);
      digitalWrite(_A0, LOW);
      digitalWrite(_CS, LOW);
      WriteDataPins(addr);
      digitalWrite(_WR, LOW);
              delayMicroseconds(1);
      digitalWrite(_WR, HIGH);
      digitalWrite(_CS, HIGH);
              delayMicroseconds(1);
      digitalWrite(_A0, HIGH);
      digitalWrite(_CS, LOW);
      WriteDataPins(data);
      digitalWrite(_WR, LOW);
              delayMicroseconds(1);
      digitalWrite(_WR, HIGH);
      digitalWrite(_CS, HIGH);
}