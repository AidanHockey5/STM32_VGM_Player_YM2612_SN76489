#ifndef _WIDE_STRINGS_H_
#define _WIDE_STRINGS_H_
#include <stdlib.h> 

//wchar_t types allocate as 32 bits, which throws things off a little.
//So, we'll just use this union to make our own custom wide char type.
union wide
{
    char bytes[2];
    unsigned short wchar;
};

//Iterates through wide string pointer until function hits 0x0000 (Terminator)
unsigned short wstrlen(wide *in); 
//Converts wide chars to standard chars. GD3 specific, not for general usage.
char* widetochar(wide* in);
static char* OUTPUT_WIDE_TO_CHAR;

#endif