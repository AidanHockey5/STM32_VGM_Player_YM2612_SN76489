#include "VGMHeader.h"

VGMHeader::VGMHeader(){}

bool VGMHeader::read(File *f)
{
    f->read(&indent, 4);
    f->read(&EoF, 4);
    f->read(&version, 4);
    f->read(&sn76489Clock, 4);
    f->read(&ym2413Clock, 4);
    f->read(&gd3Offset, 4);
    f->read(&totalSamples, 4);
    f->read(&loopOffset, 4);
    f->read(&loopNumSamples, 4);
    f->read(&rate, 4);
    f->read(&snX, 4);
    f->read(&ym2612Clock, 4);
    f->read(&ym2151Clock, 4);
    f->read(&vgmDataOffset, 4);
    f->read(&segaPCMClock, 4);
    f->read(&spcmInterface, 4);
    f->read(&rf5C68clock, 4);
    f->read(&ym2203clock, 4);
    f->read(&ym2608clock, 4);
    f->read(&ym2610clock, 4);
    f->read(&ym3812clock, 4);
    f->read(&ym3526clock, 4);
    f->read(&y8950clock, 4);
    f->read(&ymf262clock, 4);
    f->read(&ymf278bclock, 4);
    f->read(&ymf271clock, 4);
    f->read(&ymz280Bclock, 4);
    f->read(&rf5C164clock, 4);
    f->read(&pwmclock, 4);
    f->read(&ay8910clock, 4);
    f->read(&ayclockflags, 4);
    f->read(&vmlblm, 4);
    f->read(&gbdgmclock, 4);
    f->read(&nesapuclock, 4);
    f->read(&multipcmclock, 4);
    f->read(&upd7759clock, 4);
    f->read(&okim6258clock, 4);
    f->read(&ofkfcf, 4);
    f->read(&okim6295clock, 4);
    f->read(&k051649clock, 4);
    f->read(&k054539clock, 4);
    f->read(&huc6280clock, 4);
    f->read(&c140clock, 4);
    f->read(&k053260clock, 4);
    f->read(&pokeyclock, 4);
    f->read(&qsoundclock, 4);
    f->read(&scspclock, 4);
    f->read(&extrahdrofs, 4);
    f->read(&wonderswanclock, 4);
    f->read(&vsuClock, 4);
    f->read(&saa1099clock, 4);
    f->read(&es5503clock, 4);
    f->read(&es5506clock, 4);
    f->read(&eschcdxx, 4);
    f->read(&x1010clock, 4);
    f->read(&c352clock, 4);
    f->read(&ga20clock, 4);

    f->seek(0); //Return back to start of file
    return vgmVerify();
}

bool VGMHeader::vgmVerify()
{
    return indent == 0x206D6756;
}

VGMHeader::~VGMHeader(){}