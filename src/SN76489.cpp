#include "SN76489.h"

SN76489::SN76489(Bus * bus)
{
    _bus = bus;
    GPIOB->regs->CRL = (GPIOB->regs->CRL & 0xFF0FFFFF) | 0x00300000; //_WE OUTPUT
    GPIOB->regs->ODR |= 0x20; //_WE HIGH
}

void SN76489::Reset()
{
  Write(0x9F);
  Write(0xBF);
  Write(0xDF);
  Write(0xFF);  
}

void SN76489::Send(unsigned char data)
{
  //PSG noise channel fix
  //A bunny fixed the hi-hat for ya'll.
  if((data & 0x80) == 0)
  {
    if((psgFrqLowByte & 0x0F) == 0)
    {
      if((data & 0x3F) == 0)
        psgFrqLowByte |= 1;
    }
    Write(psgFrqLowByte);
    Write(data);
  }
  else if((data & 0x90) == 0x80 && (data & 0x60)>>5 != 3)
    psgFrqLowByte = data;
  else
    Write(data);
}

void SN76489::Write(unsigned char data)
{
  GPIOB->regs->ODR |= 0x20; //_WE HIGH
  _bus->Write(data);
  GPIOB->regs->ODR &= ~(0x20); //_WE_LOW
  delayMicroseconds(14);
  GPIOB->regs->ODR |= 0x20; //_WE HIGH
}