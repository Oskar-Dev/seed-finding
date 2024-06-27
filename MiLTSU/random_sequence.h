#ifndef RANDOM_SEQUENCE_H
#define RANDOM_SEQUENCE_H

#include "../cubiomes/rng.h"
#include "md5.h"
#include <string.h>

#ifdef __cplusplus
extern "C"
{
#endif

    Xoroshiro getRandomSequenceXoro(uint64_t worldSeed, char lootIdentifier[]);

    uint64_t ullFromBytes(
        uint8_t b1, uint8_t b2, uint8_t b3, uint8_t b4,
        uint8_t b5, uint8_t b6, uint8_t b7, uint8_t b8);

    uint64_t mixStafford13(uint64_t seed);

#ifdef __cplusplus
}
#endif

#endif /* RANDOM_SEQUENCE_H */