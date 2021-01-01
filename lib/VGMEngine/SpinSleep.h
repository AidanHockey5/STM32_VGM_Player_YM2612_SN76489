#include <Arduino.h>

#ifndef _SPIN_SLEEP_H_
#define _SPIN_SLEEP_H_

#define DRIVER_CLOCK_RATE 72000000

#define DEMCR (*((volatile uint32_t *)0xE000EDFC))
#define DWT_CTRL (*(volatile uint32_t *)0xe0001000)
#define CYCCNTENA (1<<0)
#define DWT_CYCCNT ((volatile uint32_t *)0xE0001004)
#define CPU_CYCLES *DWT_CYCCNT
#define DEMCR_TRCENA    0x01000000


inline static void resetSleepSpin()
{
    DEMCR |= DEMCR_TRCENA;
    *DWT_CYCCNT = 0;
    DWT_CTRL |= CYCCNTENA;
}

inline static void sleepSpin(uint32_t us) //quick and dirty spin sleep
{ 
    uint32_t s = CPU_CYCLES; //DWT_CYCCNT
    uint32_t c = us*(DRIVER_CLOCK_RATE/1000000);
    while (CPU_CYCLES  - s < c);
}

inline void sleepClocks(uint32_t f, uint32_t clks) //same dirty spin sleep, but timed relative to a certain clock freq and number of clocks
{ 
    uint32_t s = CPU_CYCLES;
    uint64_t c = (DRIVER_CLOCK_RATE*(uint64_t)clks)/f;
    while (CPU_CYCLES - s < c);
}

#endif