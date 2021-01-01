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
    char* out;
    unsigned short size = wstrlen(in);
    if(size == 0)
        return 0;
    out = (char*)malloc(sizeof(char) * (size));
    for(int i = 0; i<size-1; i++)
    {
        out[i] = in[i].bytes[0];
    }
    out[size-1] = '\0';
    return out;
}