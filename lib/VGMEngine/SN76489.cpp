#include "SN76489.h"

SN76489::SN76489(Bus* _bus, uint8_t _WE)
{
    bus = _bus;
    WE = _WE;
    pinMode(WE, OUTPUT);
    digitalWrite(WE, HIGH);
}

void SN76489::setClock(uint32_t frq)
{
    //clk->SetFreq(frq);
    clkfrq = frq;
}

void SN76489::reset()
{
    writeRaw(0x9F);
    writeRaw(0xBF);
    writeRaw(0xDF);
    writeRaw(0xFF);
}

void SN76489::write(uint8_t data)
{
    if((data & 0x80) == 0)
    {
        if((psgFrqLowByte & 0x0F) == 0)
        {
            if((data & 0x3F) == 0)
            psgFrqLowByte |= 1;
        }
        writeRaw(psgFrqLowByte);
        writeRaw(data);
    }
    else if((data & 0x90) == 0x80 && (data & 0x60)>>5 != 3)
        psgFrqLowByte = data;
    else
        writeRaw(data);
}

void SN76489::writeRaw(uint8_t data)
{
    digitalWrite(WE, HIGH);
    bus->write(data);
    digitalWrite(WE, LOW);
    delay_us(11);
    digitalWrite(WE, HIGH);
}

SN76489::~SN76489(){}