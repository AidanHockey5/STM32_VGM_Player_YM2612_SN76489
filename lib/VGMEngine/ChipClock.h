#ifndef _CHIP_CLOCK_H_
#define _CHIP_CLOCK_H_
#include "LTC6904.h"
#include <stdlib.h>

class ChipClock
{
public:
    ChipClock();
    ChipClock(LTC6904 *ltc);
    ~ChipClock();
    void SetFreq(uint32_t frq);
    //uint32_t GetFreq();
private:
    LTC6904 *ltc = NULL;
    //uint32_t frequency;
};



#endif