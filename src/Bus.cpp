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
    //uint16_t d =  (data>>2) | 000 | (data>>0) | 00000 | (data >> 5);
    uint16_t d = 0x00;
    GPIOB->regs->BSRR = (1U << 8) << (16 * ((data >> 0)&1));
    GPIOB->regs->BSRR = (1U << 9) << (16 * ((data >> 1)&1));
    GPIOC->regs->BSRR = (1U << 13) << (16 * ((data >> 2)&1));
    GPIOC->regs->BSRR = (1U << 14) << (16 * ((data >> 3)&1));
    GPIOC->regs->BSRR = (1U << 15) << (16 * ((data >> 4)&1));
    GPIOA->regs->BSRR = (1U << 0) << (16 * ((data >> 5)&1));
    GPIOA->regs->BSRR = (1U << 1) << (16 * ((data >> 6)&1));
    GPIOA->regs->BSRR = (1U << 2) << (16 * ((data >> 7)&1));
}



/*
((data >> 0)&1) == HIGH ? GPIOB->regs->ODR |= 1 << 8 : GPIOB->regs->ODR &= ~(1 << 8); //PB8
((data >> 1)&1) == HIGH ? GPIOB->regs->ODR |= 1 << 9 : GPIOB->regs->ODR &= ~(1 << 9); //PB9
((data >> 2)&1) == HIGH ? GPIOC->regs->ODR |= 1 << 13 : GPIOC->regs->ODR &= ~(1 << 13); //PC13
((data >> 3)&1) == HIGH ? GPIOC->regs->ODR |= 1 << 14 : GPIOC->regs->ODR &= ~(1 << 14); //PC14
((data >> 4)&1) == HIGH ? GPIOC->regs->ODR |= 1 << 15 : GPIOC->regs->ODR &= ~(1 << 15); //PC15
((data >> 5)&1) == HIGH ? GPIOA->regs->ODR |= 1 << 0 : GPIOA->regs->ODR &= ~(1 << 0); //PA0
((data >> 6)&1) == HIGH ? GPIOA->regs->ODR |= 1 << 1 : GPIOA->regs->ODR &= ~(1 << 1); //PA1
((data >> 7)&1) == HIGH ? GPIOA->regs->ODR |= 1 << 2 : GPIOA->regs->ODR &= ~(1 << 2); //PA2
*/