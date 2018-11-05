#include <stdint.h>

struct VGMHeader
{
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
    void Reset()
    {
        indent = 0;
        EoF = 0;
        version = 0;
        sn76489Clock = 0;
        ym2413Clock = 0;
        gd3Offset = 0;
        totalSamples = 0;
        loopOffset = 0;
        loopNumSamples = 0;
        rate = 0;
        snX = 0;
        ym2612Clock = 0;
        ym2151Clock = 0;
        vgmDataOffset = 0;
        segaPCMClock = 0;
        spcmInterface = 0;
    }
};

struct GD3
{
    uint32_t size;
    String enTrackName;
    String enGameName;
    String enSystemName;
    String enAuthor;
    String releaseDate;
    void Reset()
    {
        size = 0;
        enTrackName = "";
        enGameName = "";
        enSystemName = "";
        enAuthor = "";
        releaseDate = "";
    }
};

enum FileStrategy {FIRST_START, NEXT, PREV, RND, REQUEST};
enum PlayMode {LOOP, PAUSE, SHUFFLE, IN_ORDER};