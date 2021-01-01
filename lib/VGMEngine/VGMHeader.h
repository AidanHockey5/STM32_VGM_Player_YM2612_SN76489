#ifndef _VGM_HEADER_H_
#define _VGM_HEADER_H_
#include <stdint.h>
#include <stdlib.h>
#include <SdFat.h>

class VGMHeader
{
public:
    VGMHeader();
    ~VGMHeader();
    bool read(File *f);
    uint32_t indent;
    uint32_t EoF;
    uint32_t version;
    uint32_t sn76489Clock;
    uint32_t ym2413Clock;
    uint32_t gd3Offset;
    uint32_t totalSamples;
    uint32_t loopOffset;
    uint32_t loopNumSamples;
    uint32_t rate;
    uint32_t snX;
    uint32_t ym2612Clock;
    uint32_t ym2151Clock;
    uint32_t vgmDataOffset;
    uint32_t segaPCMClock;
    uint32_t spcmInterface;
    uint32_t rf5C68clock;
    uint32_t ym2203clock;
    uint32_t ym2608clock;
    uint32_t ym2610clock;
    uint32_t ym3812clock;
    uint32_t ym3526clock;
    uint32_t y8950clock;
    uint32_t ymf262clock;
    uint32_t ymf278bclock;
    uint32_t ymf271clock;
    uint32_t ymz280Bclock;
    uint32_t rf5C164clock;
    uint32_t pwmclock;
    uint32_t ay8910clock;
    uint32_t ayclockflags;
    uint32_t vmlblm;
    uint32_t gbdgmclock;
    uint32_t nesapuclock;
    uint32_t multipcmclock;
    uint32_t upd7759clock;
    uint32_t okim6258clock;
    uint32_t ofkfcf;
    uint32_t okim6295clock;
    uint32_t k051649clock;
    uint32_t k054539clock;
    uint32_t huc6280clock;
    uint32_t c140clock;
    uint32_t k053260clock;
    uint32_t pokeyclock;
    uint32_t qsoundclock;
    uint32_t scspclock;
    uint32_t extrahdrofs;
    uint32_t wonderswanclock;
    uint32_t vsuClock;
    uint32_t saa1099clock;
    uint32_t es5503clock;
    uint32_t es5506clock;
    uint32_t eschcdxx;
    uint32_t x1010clock;
    uint32_t c352clock;
    uint32_t ga20clock;
private:
    bool vgmVerify();
};



#endif 