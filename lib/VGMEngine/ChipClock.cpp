#include "ChipClock.h"

ChipClock::ChipClock(){}

ChipClock::ChipClock(LTC6904 *_ltc)
{
    ltc = _ltc;
}

void ChipClock::SetFreq(uint32_t frq)
{
    ltc->SetFrequency(frq);
    //frequency = frq;
}

// uint32_t ChipClock::GetFreq()
// {
//     return frequency;
// }

ChipClock::~ChipClock(){}