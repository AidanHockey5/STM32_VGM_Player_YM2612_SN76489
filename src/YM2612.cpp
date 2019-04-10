#include "YM2612.h"

YM2612::YM2612(Bus * bus)
{
    _bus = bus;
    GPIOA->regs->CRH = (GPIOA->regs->CRH & 0x0FF00FFF) | 0x30033000; //PA11-YM_A0, PA12-YM_WR, PA15-YM_RD OUTPUT
    GPIOB->regs->CRL = (GPIOB->regs->CRL & 0xFFF00FF0) | 0x00033003; //PB0-YM_A1, PB3-YM_CS, PB4-YM_IC OUTPUT

    GPIOB->regs->BSRR = (1U << 3) << (16 * 0); //_CS HIGH
    GPIOA->regs->BSRR = (1U << 15) << (16 * 0); //_RD HIGH
    GPIOA->regs->BSRR = (1U << 12) << (16 * 0); //_WR HIGH
    GPIOB->regs->BSRR = (1U << 4) << (16 * 0); //_IC HIGH
    GPIOA->regs->BSRR = (1U << 11) << (16 * 1); //_A0 LOW
    GPIOB->regs->BSRR = (1U << 0) << (16 * 1); //_A1 LOW
}

void YM2612::Reset()
{
    GPIOB->regs->BSRR = (1U << 4) << (16 * 1); //_IC LOW
    delayMicroseconds(25);
    GPIOB->regs->BSRR = (1U << 4) << (16 * 0); //_IC HIGH
}

void YM2612::Send(unsigned char addr, unsigned char data, bool setA1) //0x52 = A1 LOW, 0x53 = A1 HIGH
{
    GPIOB->regs->BSRR = (1U << 0) << (16 * !setA1); //_A1 PB0
    GPIOA->regs->ODR &= ~(0x0800); //_A0 LOW
    GPIOB->regs->ODR &= ~(0x0808); //_CS LOW
    _bus->Write(addr);
    GPIOA->regs->ODR &= ~(0x1000); //_WR LOW
    GPIOA->regs->ODR |= 0x1000;    //_WR HIGH
    GPIOB->regs->ODR |= 0x0808;    //_CS HIGH
    GPIOA->regs->ODR |= 0x0800;    //_A0 HIGH
    GPIOB->regs->ODR &= ~(0x0808); //_CS LOW
    _bus->Write(data);
    GPIOA->regs->ODR &= ~(0x1000); //_WR LOW
    GPIOA->regs->ODR |= 0x1000;    //_WR HIGH
    GPIOB->regs->ODR |= 0x0808;    //_CS HIGH
}