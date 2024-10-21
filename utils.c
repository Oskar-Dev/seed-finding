#include "utils.h"
#include <math.h>

int clamp(const int v, const int mn, const int mx) {
    if (v < mn) return mn;
    if (v > mx) return mx;
    return v;
}

uint64_t get_population_seed(const uint64_t world_seed, const int block_x, const int block_z) {
    Xoroshiro xr;
    xSetSeed(&xr, world_seed);
    uint64_t a = xNextLongJ(&xr) | 1;
    uint64_t b = xNextLongJ(&xr) | 1;
    
    uint64_t population_seed = (uint64_t)block_x * a + (uint64_t)block_z * b ^ world_seed;
    return population_seed;
}

uint64_t get_decorator_seed(const uint64_t world_seed, const int block_x, const int block_z, const uint32_t salt) {
    uint64_t population_seed = get_population_seed(world_seed, block_x, block_z);
    return population_seed + salt;
}

void get_loot_seeds(Xoroshiro* xr, uint64_t* loot_seeds, const int chests) {
    // xSetSeed(xr, get_decorator_seed(world_seed, block_x, block_z, salt));
    // xNextLongJ(&xr); // Wywołaj to tyle razy, ile jest skrzynek w tym samym pomieszczeniu.
    // return xNextLongJ(&xr);
    // Powtórz dwa ostatnie kroki dla każdego pomieszczenia.

    for (int i = 0; i < chests; ++i) {
        xNextLongJ(xr);
    }

    for (int i = 0; i < chests; ++i) {
        loot_seeds[i] = xNextLongJ(xr);
    }
}

void enchant_with_levels(Xoroshiro *xr, int level, const int ench_val) {
    level += 1 + xNextInt(xr, ench_val / 4 + 1) + xNextInt(xr, ench_val / 4 + 1);
    float amplifier = (xNextFloat(xr) + xNextFloat(xr) - 1.0f) * 0.15f;
    level = clamp((int)round((float)level + (float)level * amplifier), 1, INT_MAX);
    
    xNextInt(xr, 1);
    while (xNextInt(xr, 50) <= level) {
        xNextInt(xr, 1);
        level /= 2;
    }
}

void enchant_with_levels_java(uint64_t* rand, int level, const int ench_val) {
    level += 1 + nextInt(rand, ench_val / 4 + 1) + nextInt(rand, ench_val / 4 + 1);
    float amplifier = (nextFloat(rand) + nextFloat(rand) - 1.0f) * 0.15f;
    level = clamp((int)round((float)level + (float)level * amplifier), 1, INT_MAX);
    
    nextInt(rand, 1);
    while (nextInt(rand, 50) <= level) {
        nextInt(rand, 1);
        level /= 2;
    }
}