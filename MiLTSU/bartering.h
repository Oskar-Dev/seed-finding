#ifndef BARTERING_H
#define BARTERING_H

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "../cubiomes/rng.h"

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct
    {
        int id;
        int amount;
    } BarteringOut;

    typedef struct
    {
        int id;
        int min;
        int max;
        int weight;
        bool rollAmount;
        bool rollSS;
    } Entry;

    // Rolls next barter
    BarteringOut nextBarteringLoot(Xoroshiro *xr);

    // Rolls next barter, if pearls, return amount
    int nextPearls(Xoroshiro *xr);

#ifdef __cplusplus
}
#endif

#endif /* BARTERING_H */