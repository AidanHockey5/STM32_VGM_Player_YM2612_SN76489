#include "SN76489.h"

SN76489::SN76489(Bus * bus)
{
    _bus = bus;
    GPIOB->regs->CRL = (GPIOB->regs->CRL & 0xFF0FFFFF) | 0x00300000; //_WE OUTPUT
    GPIOB->regs->ODR |= 0x20; //_WE HIGH
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
    GPIOB->regs->ODR |= 0x20; //_WE HIGH
    _bus->Write(data);
    GPIOB->regs->ODR &= ~(0x20); //_WE_LOW
    delayMicroseconds(14);
    GPIOB->regs->ODR |= 0x20; //_WE HIGH
}