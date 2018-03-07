#ifndef SPIRAM_H_
#define SPIRAM_H_
#include <Arduino.h>
class SPIRAM
{
public:
    SPIRAM(int CS);
    unsigned char ReadByte(uint32_t addr);
    void WriteByte(uint32_t addr, unsigned char data);
    void Init();
private:
    int _CS;
};
#endif