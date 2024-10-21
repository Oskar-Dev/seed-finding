#pragma once

#ifndef END_CITY_H
#define END_CITY_H

#include "gateway.c"

typedef struct {
    int id;
    int min;
    int max;
    int weight;
    bool enchant;
    int enchant_min;
    int enchant_max;
    int enchant_value;
    bool is_trim;
} EndCityLootEntry;

typedef struct {
    int id;
    int ammount;
    bool enchanted;
} EndCityLootOut;

int check_end_city(uint64_t seed, int max_distance, Pos* pos);
int get_end_city_chest_loot(const uint64_t loot_seed, EndCityLootOut* loot);
int end_city_loot_check(const uint64_t seed, const int chests, const int x, const int z);
int check_end_city_for_trim(const uint64_t world_seed, const int chunk_x, const int chunk_z);
void print_end_city_entry(EndCityLootOut* entry);

#endif