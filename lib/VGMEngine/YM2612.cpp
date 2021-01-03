#include "YM2612.h"

YM2612::YM2612(Bus* _bus, uint8_t _CS, uint8_t _RD, uint8_t _WR, uint8_t _A0, uint8_t _A1, uint8_t _IC)
{
    //clk = _clk;
    bus = _bus;
    CS = _CS;
    RD = _RD;
    WR = _WR;
    A0 = _A0;
    A1 = _A1;
    IC = _IC;
    //setClock(7670454UL); //Set default clock

    pinMode(CS, OUTPUT);
    if(RD != NULL)
        pinMode(RD, OUTPUT);
    pinMode(WR, OUTPUT);
    pinMode(A0, OUTPUT);
    pinMode(A1, OUTPUT);
    pinMode(IC, OUTPUT);

    digitalWrite(CS, HIGH);
    if(RD != NULL)
        digitalWrite(RD, HIGH);
    digitalWrite(WR, HIGH);
    digitalWrite(IC, HIGH);
    digitalWrite(A0, LOW);
    digitalWrite(A1, LOW);
    //reset();
}

void YM2612::write(uint8_t addr, uint8_t data, bool a1)
{
    // digitalWrite(A1, a1);
    // digitalWrite(A0, LOW);
    // digitalWrite(CS, LOW);
    // bus->write(addr);
    // digitalWrite(WR, LOW);
    // digitalWrite(WR, HIGH);
    // digitalWrite(A0, HIGH);
    // bus->write(data);
    // digitalWrite(WR, LOW);
    // digitalWrite(WR, HIGH);
    // digitalWrite(CS, HIGH);
    // delayMicroseconds(1);

    GPIOB->regs->BSRR = (1U << 0) << (16 * !a1); //_A1 PB0
    nop;
    GPIOA->regs->ODR &= ~(0x0800); //_A0 LOW
    nop;
    bus->write(addr);
        nop;
    GPIOB->regs->ODR &= ~(0x0808); //_CS LOW
    GPIOA->regs->ODR &= ~(0x1000); //_WR LOW
    nop;
    GPIOA->regs->ODR |= 0x1000;    //_WR HIGH 
    GPIOA->regs->ODR |= 0x0800;    //_A0 HIGH
    bus->write(data);
        nop;
    GPIOA->regs->ODR &= ~(0x1000); //_WR LOW
    nop;
    GPIOA->regs->ODR |= 0x1000;    //_WR HIGH
    GPIOB->regs->ODR |= 0x0808;    //_CS HIGH
    nop;nop;
}

void YM2612::reset()
{
    digitalWrite(IC, LOW);
    delayMicroseconds(25);
    digitalWrite(IC, HIGH);
    delayMicroseconds(25);
}

void YM2612::setClock(uint32_t frq)
{
    //clk->SetFreq(frq);
    clkfrq = frq;
}

YM2612::~YM2612(){}