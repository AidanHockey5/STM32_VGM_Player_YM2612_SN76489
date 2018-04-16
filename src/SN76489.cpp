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
    //digitalWrite(_WE, HIGH);
    GPIOB->regs->ODR |= 0b0000000000100000; //_WE HIGH

    //This is the price you pay for bitwise pin toggling on abitrary GPIO busses
    ((data >> 0)&1) == HIGH ? GPIOB->regs->ODR |= 1 << 8 : GPIOB->regs->ODR &= ~(1 << 8); //PB8
    ((data >> 1)&1) == HIGH ? GPIOB->regs->ODR |= 1 << 9 : GPIOB->regs->ODR &= ~(1 << 9); //PB9
    ((data >> 2)&1) == HIGH ? GPIOC->regs->ODR |= 1 << 13 : GPIOC->regs->ODR &= ~(1 << 13); //PC13
    ((data >> 3)&1) == HIGH ? GPIOC->regs->ODR |= 1 << 14 : GPIOC->regs->ODR &= ~(1 << 14); //PC14
    ((data >> 4)&1) == HIGH ? GPIOC->regs->ODR |= 1 << 15 : GPIOC->regs->ODR &= ~(1 << 15); //PC15
    ((data >> 5)&1) == HIGH ? GPIOA->regs->ODR |= 1 << 0 : GPIOA->regs->ODR &= ~(1 << 0); //PA0
    ((data >> 6)&1) == HIGH ? GPIOA->regs->ODR |= 1 << 1 : GPIOA->regs->ODR &= ~(1 << 1); //PA0
    ((data >> 7)&1) == HIGH ? GPIOA->regs->ODR |= 1 << 2 : GPIOA->regs->ODR &= ~(1 << 2); //PA0
    // for(int i=0; i<8; i++)
    // {
    //   digitalWrite(*(_dataPins+i), ((data >> i)&1));
    // }
    GPIOB->regs->ODR &= ~(0b0000000000100000);
    //digitalWrite(_WE, LOW);
    delayMicroseconds(13);
    GPIOB->regs->ODR |= 0b0000000000100000; //_WE HIGH
    //digitalWrite(_WE, HIGH);
}