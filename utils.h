#pragma once

#ifndef UTILS_H
#define UTILS_H

#include <inttypes.h>
#include "cubiomes/rng.h"

int clamp(const int v, const int mn, const int mx);
uint64_t get_population_seed(const uint64_t world_seed, const int block_x, const int block_z);
uint64_t get_decorator_seed(const uint64_t world_seed, const int block_x, const int block_z, const uint32_t salt);
void get_loot_seeds(Xoroshiro* xr, uint64_t* loot_seeds, const int chests);
void enchant_with_levels(Xoroshiro *xr, int level, const int ench_val);
void enchant_with_levels_java(uint64_t* rand, int level, const int ench_val);

#endif