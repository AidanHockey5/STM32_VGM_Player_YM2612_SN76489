#include "GD3.h"

GD3::GD3(){}

GD3::~GD3()
{
    delete &version;
    delete &size;
    delete[] data;
    delete[] enTrackName;
    delete[] jpTrackName;
    delete[] enGameName;
    delete[] jpGameName;
    delete[] enSystemName;
    delete[] jpSystemName;
    delete[] enAuthor;
    delete[] jpAuthor;
    delete[] releaseDate;
    delete[] converterName;
    delete[] notes;
}

bool GD3::read(File *f, uint32_t gd3Offset)
{
    free(data);
    uint32_t prePos = f->curPosition(); 
    f->seek(gd3Offset);
    uint32_t magic = 0; 
    f->read(&magic, 4);
    if(magic != 540238919) //Gd3_
        return false;
    f->read(&version, 4);  //Version info
    f->read(&size, 4);     //GD3 size
    
    //Grab the entire GD3 data block and stash it away
    data = (wide*)malloc(sizeof(wide) * (size+1));
    f->read(data, size);

    //You don't need loops, you need line count. That's what gets you hired!

    //This section jumps through the GD3 data block stored above and dots pointers at the start of every substring
    uint16_t offset = 0;
    uint16_t length = wstrlen(data+offset);

    enTrackName = (wide*)data+offset;
    offset += length;

    length = wstrlen(data+offset);
    jpTrackName = (wide*)data+offset;
    offset += length;

    length = wstrlen(data+offset);
    enGameName = (wide*)data+offset;
    offset += length;

    length = wstrlen(data+offset);
    jpGameName = (wide*)data+offset;
    offset += length;

    length = wstrlen(data+offset);
    enSystemName = (wide*)data+offset;
    offset += length;

    length = wstrlen(data+offset);
    jpSystemName = (wide*)data+offset;
    offset += length;

    length = wstrlen(data+offset);
    enAuthor = (wide*)data+offset;
    offset += length;

    length = wstrlen(data+offset);
    jpAuthor = (wide*)data+offset;
    offset += length;

    length = wstrlen(data+offset);
    releaseDate = (wide*)data+offset;
    offset += length;

    length = wstrlen(data+offset);
    converterName = (wide*)data+offset;
    offset += length;

    length = wstrlen(data+offset);
    notes = (wide*)data+offset;
    offset += length;

    f->seek(prePos);
    return true;
}