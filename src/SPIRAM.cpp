#include "SPIRAM.h"
#include <SPI.h>

SPIClass SPI_2(2);

SPIRAM::SPIRAM(int CS)
{
    _CS = CS;
}

void SPIRAM::Init()
{
    SPI_2.begin();
    SPI_2.setBitOrder(MSBFIRST); // Set the SPI-2 bit order (*) 
    SPI_2.setDataMode(SPI_MODE0);
    SPI_2.setClockDivider(SPI_CLOCK_DIV2);
    //pinMode(_CS, OUTPUT);
    //digitalWrite(_CS, HIGH);

    //Set read mode to "byte"
    ////digitalWrite(_CS, LOW);
    SPI_2.transfer(0x05);
    SPI_2.transfer(0x00);
    //digitalWrite(_CS, HIGH);

    //Set write mode to "byte"
    //digitalWrite(_CS, LOW);
    SPI_2.transfer(0x01);
    SPI_2.transfer(0x00);
    //digitalWrite(_CS, HIGH);
}

unsigned char SPIRAM::ReadByte(uint32_t addr)
{
    //digitalWrite(_CS, LOW);
    SPI_2.transfer(0x03);
    SPI_2.transfer((uint8_t)(addr >> 16)); //&0xff
    SPI_2.transfer((uint8_t)(addr >> 8));
    SPI_2.transfer((uint8_t)addr);
    unsigned char data = SPI.transfer(0x00);
    //digitalWrite(_CS, HIGH);
    return data;
}

void SPIRAM::WriteByte(uint32_t addr, unsigned char data)
{
    //digitalWrite(_CS, LOW);
    SPI_2.transfer(0x02);
    SPI_2.transfer((uint8_t)(addr >> 16));
    SPI_2.transfer((uint8_t)(addr >> 8));
    SPI_2.transfer((uint8_t)addr);
    SPI_2.transfer(data);
    //digitalWrite(_CS, HIGH);
}