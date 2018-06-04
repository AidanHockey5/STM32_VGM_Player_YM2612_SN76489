#include "LTC6904.h"
#include <Arduino.h>
#include <Wire.h>
LTC6904::LTC6904(bool ADR)
{
  if(ADR)
    _ADR = 0x16;
  else
    _ADR = 0x17;
}

void LTC6904::SetManual(uint16_t oct, uint16_t dac)
{
  unsigned char CNF = 0b00000010;
  uint16_t BitMap = (oct << 12) | (dac << 2) | CNF;
  byte Byte1=(byte)(BitMap >> 8 );
  byte Byte2=(byte)BitMap;
  Wire.beginTransmission(_ADR);
  Wire.write(Byte1);
  Wire.write(Byte2);
  Wire.endTransmission();
}

void LTC6904::SetFrequency(uint32_t freq)
{
  _oct = 3.322 * log10(freq/1039);
  _dac = round(2048 - ((2078 * pow(2, 10+_oct)) / freq));
  SetManual(_oct, _dac);
}
