#include "random_sequence.h"

uint64_t mixStafford13(uint64_t seed)
{
    seed = (seed ^ (seed >> 30)) * (uint64_t)(-4658895280553007687);
    seed = (seed ^ (seed >> 27)) * (uint64_t)(-7723592293110705685);
    return seed ^ (seed >> 31);
}

uint64_t ullFromBytes(
    uint8_t b1, uint8_t b2, uint8_t b3, uint8_t b4,
    uint8_t b5, uint8_t b6, uint8_t b7, uint8_t b8)
{
    return ((uint64_t)b1 & 0xFFUL) << 56 | ((uint64_t)b2 & 0xFFUL) << 48 | ((uint64_t)b3 & 0xFFUL) << 40 | ((uint64_t)b4 & 0xFFUL) << 32 | ((uint64_t)b5 & 0xFFUL) << 24 | ((uint64_t)b6 & 0xFFUL) << 16 | ((uint64_t)b7 & 0xFFUL) << 8 | ((uint64_t)b8 & 0xFFUL);
}

Xoroshiro getRandomSequenceXoro(uint64_t worldSeed, char *lootIdentifier)
{
    // Create unmixed Xoroshiro seed
    uint64_t unmixedLow = worldSeed ^ 0x6A09E667F3BCC909ULL;
    uint64_t unmixedHi = unmixedLow - 7046029254386353131ULL;

    // Create identifier Xoroshiro seed
    uint8_t bs[16];
    md5String(lootIdentifier, bs);
    uint64_t identifierLow = ullFromBytes(bs[0], bs[1], bs[2], bs[3], bs[4], bs[5], bs[6], bs[7]);
    uint64_t identifierHi = ullFromBytes(bs[8], bs[9], bs[10], bs[11], bs[12], bs[13], bs[14], bs[15]);

    // Create split Xoroshiro seed
    uint64_t splitLow = unmixedLow ^ identifierLow;
    uint64_t splitHi = unmixedHi ^ identifierHi;

    Xoroshiro xoro;
    // Create final mixed Xoroshiro seed
    xoro.lo = mixStafford13(splitLow);
    xoro.hi = mixStafford13(splitHi);

    return xoro;
}