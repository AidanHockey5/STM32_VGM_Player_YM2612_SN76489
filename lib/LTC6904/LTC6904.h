#ifndef _LTC6904_H_
#define _LTC6904_H_
#include <stdint.h>
#include <Arduino.h>
#include <Wire.h>
class LTC6904
{
private:
  uint16_t _oct;
  uint16_t _dac;
  uint8_t _ADR; //ADR (I2C address) pin set LOW = 0010111, HIGH = 0010110
public:
  LTC6904(bool ADR);
  void SetManual(uint16_t oct, uint16_t dac);
  void SetFrequency(uint32_t freq);
};
#endif
