#include "SPIRAM.h"
#include <SPI.h>

SPIRAM::SPIRAM(int CS)
{
    _CS = CS;
}

void SPIRAM::Init()
{
    pinMode(_CS, OUTPUT);
    digitalWrite(_CS, HIGH);

    //Set read mode to "byte"
    digitalWrite(_CS, LOW);
    SPI.transfer(0x05);
    SPI.transfer(0x00);
    digitalWrite(_CS, HIGH);

    //Set write mode to "byte"
    digitalWrite(_CS, LOW);
    SPI.transfer(0x01);
    SPI.transfer(0x00);
    digitalWrite(_CS, HIGH);
}

unsigned char SPIRAM::ReadByte(uint32_t addr)
{
    digitalWrite(_CS, LOW);
    SPI.transfer(0x03);
    SPI.transfer((uint8_t)(addr >> 16) & 0xff);
    SPI.transfer((uint8_t)(addr >> 8) & 0xff);
    SPI.transfer((uint8_t)addr);
    unsigned char data = SPI.transfer(0x00);
    digitalWrite(_CS, HIGH);
    return data;
}

void SPIRAM::WriteByte(uint32_t addr, unsigned char data)
{
    digitalWrite(_CS, LOW);
    SPI.transfer(0x02);
    SPI.transfer((uint8_t)(addr >> 16) & 0xff);
    SPI.transfer((uint8_t)(addr >> 8) & 0xff);
    SPI.transfer((uint8_t)addr);
    SPI.transfer(data);
    digitalWrite(_CS, HIGH);
}