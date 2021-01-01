#ifndef _SERIAL_UTILS_H_
#define _SERIAL_UTILS_H_

#include <Arduino.h>
#include "WideStrings.h"

void BeginSerial(uint32_t baud);
void printw(wide* str);
void printlnw(wide* str);

#endif