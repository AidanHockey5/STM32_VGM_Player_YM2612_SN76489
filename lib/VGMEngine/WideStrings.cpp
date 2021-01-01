#include "WideStrings.h"

unsigned short wstrlen(wide *in)
{
    unsigned short i = 0;
    while(in[i].wchar != 0x0000) //0x0000 represents terminator
    {
        i++;
    }
    return i+1;
}

char* widetochar(wide* in) 
{
    free(OUTPUT_WIDE_TO_CHAR);
    unsigned short size = wstrlen(in);
    if(size == 0)
        return 0;
    OUTPUT_WIDE_TO_CHAR = (char*)malloc(sizeof(char) * (size));
    for(int i = 0; i<size-1; i++)
    {
        OUTPUT_WIDE_TO_CHAR[i] = in[i].bytes[0];
    }
    OUTPUT_WIDE_TO_CHAR[size-1] = '\0';
    return OUTPUT_WIDE_TO_CHAR;
}