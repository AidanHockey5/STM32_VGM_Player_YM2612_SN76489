#include "SerialUtils.h"

void BeginSerial(uint32_t baud)
{
    Serial.begin(baud);
}

void printw(wide* str)
{
    uint16_t size = wstrlen(str);
    if(size == 0)
        return;
    for(int i = 0; i<size-1; i++)
    {
        Serial.print(str[i].bytes[0]);
    }
}

void printlnw(wide* str)
{
    printw(str);
    Serial.println();
}