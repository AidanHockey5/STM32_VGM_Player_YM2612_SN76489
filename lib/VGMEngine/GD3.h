#ifndef _GD3_H_
#define _GD3_H_
#include <stdint.h>
#include <stdlib.h> 
#include <SdFat.h>
#include "WideStrings.h"

//GD3 uses wide chars (16-bit chars)

class GD3
{
public:
    uint32_t version;
    uint32_t size;
    wide* data;
    wide* enTrackName;
    wide* jpTrackName;
    wide* enGameName;
    wide* jpGameName;
    wide* enSystemName;
    wide* jpSystemName;
    wide* enAuthor;
    wide* jpAuthor;
    wide* releaseDate;
    wide* converterName;
    wide* notes;
    GD3();
    bool read(File *f, uint32_t gd3Offset);
    ~GD3();
private:
};


#endif