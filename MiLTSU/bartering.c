#include "bartering.h"

Entry entries[] = {
    {0, 0, 0, 5, false, true},     // 0:  minecraft:book
    {1, 0, 0, 8, false, true},     // 1:  minecraft:iron_boots
    {2, 0, 0, 8, false, false},    // 2:  minecraft:potion
    {3, 0, 0, 8, false, false},    // 3:  minecraft:splash_potion
    {4, 0, 0, 10, false, false},   // 4:  minecraft:potion
    {5, 10, 36, 10, true, false},  // 5:  minecraft:iron_nugget
    {6, 2, 4, 10, true, false},    // 6:  minecraft:ender_pearl
    {7, 3, 9, 20, true, false},    // 7:  minecraft:string
    {8, 5, 12, 20, true, false},   // 8:  minecraft:quartz
    {9, 1, 1, 40, false, false},   // 9:  minecraft:obsidian
    {10, 1, 3, 40, true, false},   // 10: minecraft:crying_obsidian
    {11, 1, 1, 40, false, false},  // 11: minecraft:fire_charge
    {12, 2, 4, 40, true, false},   // 12: minecraft:leather
    {13, 2, 8, 40, true, false},   // 13: minecraft:soul_sand
    {14, 2, 8, 40, true, false},   // 14: minecraft:nether_brick
    {15, 6, 12, 40, true, false},  // 15: minecraft:spectral_arrow
    {16, 8, 16, 40, true, false},  // 16: minecraft:gravel
    {17, 8, 16, 40, true, false}}; // 17: minecraft:blackstone

BarteringOut rollBarteringEntry(Xoroshiro *xr, Entry entry)
{
    int amount = 1;
    if (entry.rollAmount)
    {
        if (entry.min >= entry.max)
            amount = entry.min;
        else
            amount = xNextInt(xr, entry.max - entry.min + 1) + entry.min;
    }

    if (entry.rollSS)
    {
        xNextInt(xr, 1);
        amount = xNextInt(xr, 3) + 1;
    }

    BarteringOut out;
    out.id = entry.id;
    out.amount = amount;
    return out;
}

BarteringOut nextBarteringLoot(Xoroshiro *xr)
{
    int j = xNextInt(xr, 459);

    for (int i = 0; i < 18; i++)
    {
        Entry entry = entries[i];
        j -= entry.weight;
        if (j >= 0)
            continue;
        return rollBarteringEntry(xr, entry);
    }

    BarteringOut out;
    out.id = -1;
    out.amount = 0;
    return out;
}

// Rolls next, if pearls, return amount
int nextPearls(Xoroshiro *xr)
{
    BarteringOut out = nextBarteringLoot(xr);
    if (out.id == 6)
        return out.amount;
    return 0;
}