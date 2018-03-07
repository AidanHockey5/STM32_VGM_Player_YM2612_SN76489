#include "LTC6903.h"
#include <SPI.h>
LTC6903::LTC6903(int target)
{
  _target = target;
}

void LTC6903::SetManual(uint16_t oct, uint16_t dac)
{
  pinMode(_target, OUTPUT);
  SPI.begin();
  //SPI.beginTransaction(SPISettings(20000000, MSBFIRST, SPI_MODE0));
  unsigned char CNF = 0b00000000;
  uint16_t BitMap = (oct << 12) | (dac << 2) | CNF;
  byte Byte1=(byte)(BitMap >> 8 );
  byte Byte2=(byte)BitMap;
  digitalWrite(_target, LOW);
  SPI.transfer(Byte1);
  SPI.transfer(Byte2);
  digitalWrite(_target, HIGH);
  SPI.endTransaction();
}

void LTC6903::SetFrequency(uint32_t freq)
{
  _oct = 3.322 * log10(freq/1039);
  _dac = round(2048 - ((2078 * pow(2, 10+_oct)) / freq));
  SetManual(_oct, _dac);
}
