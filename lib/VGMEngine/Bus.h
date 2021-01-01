#ifndef _BUS_H_
#define _BUS_H_
#include <Arduino.h>

class Bus
{
public:
    Bus(uint8_t _D0, uint8_t _D1, uint8_t _D2, uint8_t _D3, uint8_t _D4, uint8_t _D5, uint8_t _D6, uint8_t _D7);
    Bus(uint8_t _pins[8]);
    void write(uint8_t data);
private:
    void prepare();
    uint8_t pins[8];
};


#endif