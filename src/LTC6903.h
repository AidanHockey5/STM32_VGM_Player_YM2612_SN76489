#ifndef LTC6903_H_
#define LTC6903_H_
#include <stdint.h>
class LTC6903
{
private:
  uint16_t _oct;
  uint16_t _dac;
  unsigned char _target;
public:
  LTC6903(int target);
  void SetManual(uint16_t oct, uint16_t dac);
  void SetFrequency(uint32_t freq);
};
#endif
