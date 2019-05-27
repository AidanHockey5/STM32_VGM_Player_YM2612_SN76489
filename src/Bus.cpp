#include "Bus.h"

Bus::Bus()
{
    //Set data direction
    GPIOA->regs->CRL = (GPIOA->regs->CRL & 0xFFFFF000) | 0x00000333; //PA0, PA1, PA2 OUTPUT
    GPIOB->regs->CRH = (GPIOB->regs->CRH & 0xFFFFFF00) | 0x00000033; //PB9, PB9 OUTPUT
    GPIOC->regs->CRH = (GPIOC->regs->CRH & 0x000FFFFF) | 0x33300000; //PC13, PC14, PC15 OUTPUT;
}

void Bus::Write(unsigned char data)
{
    //data = ~data;
    GPIOB->regs->BSRR = (1U << 8) << (16 * ((data >> 0)&1));
    GPIOB->regs->BSRR = (1U << 9) << (16 * ((data >> 1)&1));
    GPIOC->regs->BSRR = (1U << 13) << (16 * ((data >> 2)&1));
    GPIOC->regs->BSRR = (1U << 14) << (16 * ((data >> 3)&1));
    GPIOC->regs->BSRR = (1U << 15) << (16 * ((data >> 4)&1));
    GPIOA->regs->BSRR = (1U << 0) << (16 * ((data >> 5)&1));
    GPIOA->regs->BSRR = (1U << 1) << (16 * ((data >> 6)&1));
    GPIOA->regs->BSRR = (1U << 2) << (16 * ((data >> 7)&1));
}

void Bus::Reset()
{
    Write(0x00);
}