#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "end_city.h"
#include "MiLTSU/random_sequence.h"
#include "utils.h"

// typedef struct {
//     int id;
//     int min;
//     int max;
//     int weight;
//     bool enchant;
//     int enchant_min;
//     int enchant_max;
//     bool is_trim;
// } EndCityLootEntry;

#define WEIGHT_SUM 85
#define MAX_ENTRIES 7
#define TOTAL_LOOT_ENTRIES 23
#define END_CITY_SALT 40002

EndCityLootEntry loot_entries[] = {
    { 0, 2, 7, 5, false, 0, 0, 0, false }, // minecraft:diamond
    { 1, 4, 8, 10, false, 0, 0, 0, false }, // minecraft:iron_ingot
    { 2, 2, 7, 15, false, 0, 0, 0, false }, // minecraft:gold_ingot
    { 3, 2, 6, 2, false, 0, 0, 0, false }, // minecraft:emerald
    { 4, 1, 10, 5, false, 0, 0, 0, false }, // minecraft:beetroot_seeds
    { 5, 1, 1, 3, false, 0, 0, 0, false }, // minecraft:saddle
    { 6, 1, 1, 1, false, 0, 0, 0, false }, // minecraft:iron_horse_armor
    { 7, 1, 1, 1, false, 0, 0, 0, false }, // minecraft:golden_horse_armor
    { 8, 1, 1, 1, false, 0, 0, 0, false }, // minecraft:diamond_horse_armor
    { 9, 1, 1, 3, true, 20, 39, 10, false }, // minecraft:diamond_sword
    { 10, 1, 1, 3, true, 20, 39, 10, false }, // minecraft:diamond_boots
    { 11, 1, 1, 3, true, 20, 39, 10, false }, // minecraft:diamond_chestplate
    { 12, 1, 1, 3, true, 20, 39, 10, false }, // minecraft:diamond_leggings
    { 13, 1, 1, 3, true, 20, 39, 10, false }, // minecraft:diamond_helmet
    { 14, 1, 1, 3, true, 20, 39, 10, false }, // minecraft:diamond_pickaxe
    { 15, 1, 1, 3, true, 20, 39, 10, false }, // minecraft:diamond_shovel
    { 16, 1, 1, 3, true, 20, 39, 14, false }, // minecraft:iron_sword
    { 17, 1, 1, 3, true, 20, 39, 9, false }, // minecraft:iron_boots
    { 18, 1, 1, 3, true, 20, 39, 9, false }, // minecraft:iron_chestplate
    { 19, 1, 1, 3, true, 20, 39, 9, false }, // minecraft:iron_leggings
    { 20, 1, 1, 3, true, 20, 39, 9, false }, // minecraft:iron_helmet
    { 21, 1, 1, 3, true, 20, 39, 14, false }, // minecraft:iron_pickaxe
    { 22, 1, 1, 3, true, 20, 39, 14, false }, // minecraft:iron_shovel
    // { 23, 1, 1, 14, false, 0, 0, true } // minecraft:spire_armor_trim_smithing_template
};

void fast_linked_gateway(uint64_t lower48, Pos3* gateway_pos) {
    Pos mainGateway = getMainGateway(lower48);

    double rootlen = sqrt(mainGateway.x * mainGateway.x + mainGateway.z * mainGateway.z);

    Vec normalizedVector = {mainGateway.x / rootlen, mainGateway.z / rootlen};

    gateway_pos->x = normalizedVector.x * 1024.0;
    gateway_pos->z = normalizedVector.z * 1024.0;
} 

int check_end_city(uint64_t seed, int max_distance, Pos* pos) {
    SurfaceNoise noise;
    Generator g;
    setupGenerator(&g, MC_1_21, 0);

    applySeed(&g, DIM_END, seed);
    initSurfaceNoise(&noise, DIM_END, seed);

    Pos3 gateway_pos;
    fast_linked_gateway(seed, &gateway_pos);
    // Pos3 gateway_pos = linkedGateway(seed);
    int reg_x = gateway_pos.x / 320;
    int reg_z = gateway_pos.z / 320;
    Pos city_pos;
    Piece pieces[END_CITY_PIECES_MAX];

    for (int reg_mod_x = -1; reg_mod_x <= 1; ++reg_mod_x) {
        for (int reg_mod_z = -1; reg_mod_z <= 1; ++reg_mod_z) {
            if (getStructurePos(End_City, MC_1_21, seed, reg_x + reg_mod_x, reg_z + reg_mod_z, &city_pos) == 0) {
                continue;
            }
            
            if (
                isViableStructurePos(End_City, &g, city_pos.x, city_pos.z, 0) && 
                isViableEndCityTerrain(&g, &noise, city_pos.x, city_pos.z)
            ) {
                if (abs(city_pos.x - gateway_pos.x) <= max_distance && abs(city_pos.z - gateway_pos.z) <= max_distance) {
                    int pieces_n = getEndCityPieces((Piece*)pieces, seed, city_pos.x >> 4, city_pos.z >> 4);
                    for (int i = 0; i < pieces_n; ++i) {
                        if (pieces[i].type == END_SHIP) {
                            pos->x = city_pos.x >> 4;
                            pos->z = city_pos.z >> 4;
                            return 1;
                        }
                    }
                }
            }
        }
    }
    
    return 0;
}

int get_end_city_chest_loot(const uint64_t loot_seed, EndCityLootOut *loot) {
    uint64_t rand;
    setSeed(&rand, loot_seed);

    int rolls = nextInt(&rand, 5) + 2;
    for (int i = 0; i < rolls; ++i) {
        int w = nextInt(&rand, WEIGHT_SUM);

        for (int j = 0; j < TOTAL_LOOT_ENTRIES; ++j) {
            EndCityLootEntry entry = loot_entries[j];
            w -= entry.weight;

            if (w >= 0) continue;

            int ammount = entry.min == entry.max ? entry.max : nextInt(&rand, entry.max - entry.min + 1) + entry.min;
            if (entry.enchant) {
                int enchant_level = nextInt(&rand, entry.enchant_max - entry.enchant_min + 1) + entry.enchant_min;
                enchant_with_levels_java(&rand, enchant_level, entry.enchant_value);
                loot[i].enchanted = true;
            } else {
                loot[i].enchanted = false;
            }

            loot[i].ammount = ammount;
            loot[i].id = entry.id;

            break;
        }
    }

    // Do trim roll.
    if (nextInt(&rand, 15) == 14) {
        loot[rolls].ammount = 1;    
        loot[rolls].enchanted = false;    
        loot[rolls].id = 23;
        ++rolls;
    }

    return rolls;
}

int end_city_loot_check(const uint64_t seed, const int chests, const int x, const int z) {
    uint64_t loot_seeds[2];
    EndCityLootOut loot[7];
    Xoroshiro xr;

    uint64_t decorator_seed = get_decorator_seed(seed, x, z, END_CITY_SALT);
    xSetSeed(&xr, decorator_seed);

    get_loot_seeds(&xr, loot_seeds, chests);

    for (int j = 0; j < chests; ++j) {
        int entries = get_end_city_chest_loot(loot_seeds[j], loot);
        
        for (int i = 0; i < entries; ++i) {
            // Check for the trim.
            if (loot[i].id == 23) {
                return 1;
            }
        }
    }

    return 0;
}

int check_end_city_chest_number(const uint64_t world_seed, const int chunk_x, const int chunk_z) {
    int chests = 0;
    Piece pieces[END_CITY_PIECES_MAX];
    int pieces_n = getEndCityPieces(pieces, world_seed, chunk_x, chunk_z);

    for (int i = 0; i < pieces_n; ++i) {
        switch (pieces[i].type) {
            case THIRD_FLOOR_2: {
                chests += 1;
            } break;
            case END_SHIP: {
                chests += 2;
            } break;
            case FAT_TOWER_TOP: {
             chests += 2;
            } break;
            default:
                break;
        }
    }

    return chests;
}

int check_end_city_for_trim(const uint64_t world_seed, const int chunk_x, const int chunk_z) {
    Piece pieces[END_CITY_PIECES_MAX];
    int pieces_n = getEndCityPieces(pieces, world_seed, chunk_x, chunk_z);

    for (int i = 0; i < pieces_n; ++i) {
        switch (pieces[i].type) {
            case THIRD_FLOOR_2: {
                Pos chest;

                switch (pieces[i].rot) {
                    case 0: { // ok
                        chest.x = (int)floor((pieces[i].pos.x + 5) / 16.0) * 16; 
                        chest.z = (int)floor((pieces[i].pos.z + 1) / 16.0) * 16; 
                    } break;
                    
                    case 1: { // ok
                        chest.x = (int)floor((pieces[i].pos.x - 3) / 16.0) * 16; 
                        chest.z = (int)floor((pieces[i].pos.z + 5) / 16.0) * 16; 
                    } break;
                
                    case 2: { // ok
                        chest.x = (int)floor((pieces[i].pos.x - 7) / 16.0) * 16; 
                        chest.z = (int)floor((pieces[i].pos.z - 3) / 16.0) * 16; 
                    } break;
                
                    case 3: { // ok
                        chest.x = (int)floor((pieces[i].pos.x + 1) / 16.0) * 16; 
                        chest.z = (int)floor((pieces[i].pos.z - 7) / 16.0) * 16; 
                    } break;
                
                    default:
                        break;
                }

                if (end_city_loot_check(world_seed, 1, chest.x, chest.z)) return 1;
            } break;

            case END_SHIP: {
                Pos chest1;
                Pos chest2;

                switch (pieces[i].rot) {
                    case 0: { //ok
                        chest1.x = (int)floor((pieces[i].pos.x + 6) / 16.0) * 16; 
                        chest1.z = (int)floor((pieces[i].pos.z + 6) / 16.0) * 16; 
                        chest2.x = (int)floor((pieces[i].pos.x + 4) / 16.0) * 16; 
                        chest2.z = (int)floor((pieces[i].pos.z + 6) / 16.0) * 16; 
                    } break;
                    
                    case 1: { // ok
                        chest1.x = (int)floor((pieces[i].pos.x - 8) / 16.0) * 16; 
                        chest1.z = (int)floor((pieces[i].pos.z + 6) / 16.0) * 16; 
                        chest2.x = (int)floor((pieces[i].pos.x - 8) / 16.0) * 16; 
                        chest2.z = (int)floor((pieces[i].pos.z + 4) / 16.0) * 16; 
                    } break;
                
                    case 2: { // ok
                        chest1.x = (int)floor((pieces[i].pos.x - 6) / 16.0) * 16; 
                        chest1.z = (int)floor((pieces[i].pos.z - 8) / 16.0) * 16; 
                        chest2.x = (int)floor((pieces[i].pos.x - 8) / 16.0) * 16; 
                        chest2.z = (int)floor((pieces[i].pos.z - 8) / 16.0) * 16; 
                    } break;
                
                    case 3: { // ok
                        chest1.x = (int)floor((pieces[i].pos.x + 6) / 16.0) * 16; 
                        chest1.z = (int)floor((pieces[i].pos.z - 8) / 16.0) * 16; 
                        chest2.x = (int)floor((pieces[i].pos.x + 6) / 16.0) * 16; 
                        chest2.z = (int)floor((pieces[i].pos.z - 6) / 16.0) * 16; 
                    } break;
                
                    default:
                        break;
                }
                
                if (chest1.x == chest2.x && chest1.z == chest2.z) {
                    if (end_city_loot_check(world_seed, 2, chest1.x, chest1.z)) return 1;
                } else {
                    if (end_city_loot_check(world_seed, 1, chest1.x, chest1.z)) return 1;
                    if (end_city_loot_check(world_seed, 1, chest2.x, chest2.z)) return 1;
                }

            } break;
            
            case FAT_TOWER_TOP: {
                Pos chest1;
                Pos chest2;

                switch (pieces[i].rot) {
                    case 0: { //
                        chest1.x = floor((pieces[i].pos.x + 2) / 16.0) * 16; 
                        chest1.z = floor((pieces[i].pos.z + 10) / 16.0) * 16; 
                        chest2.x = floor((pieces[i].pos.x + 4) / 16.0) * 16; 
                        chest2.z = floor((pieces[i].pos.z + 12) / 16.0) * 16; 
                    } break;
                    
                    case 1: { // ok
                        chest1.x = floor((pieces[i].pos.x - 12) / 16.0) * 16; 
                        chest1.z = floor((pieces[i].pos.z + 2) / 16.0) * 16; 
                        chest2.x = floor((pieces[i].pos.x - 14) / 16.0) * 16; 
                        chest2.z = floor((pieces[i].pos.z + 4) / 16.0) * 16; 
                    } break;
                
                    case 2: { // ok
                        chest1.x = floor((pieces[i].pos.x - 6) / 16.0) * 16; 
                        chest1.z = floor((pieces[i].pos.z - 14) / 16.0) * 16; 
                        chest2.x = floor((pieces[i].pos.x - 4) / 16.0) * 16; 
                        chest2.z = floor((pieces[i].pos.z - 12) / 16.0) * 16; 
                    } break;
                 
                    case 3: { // ok
                        chest1.x = floor((pieces[i].pos.x + 10) / 16.0) * 16; 
                        chest1.z = floor((pieces[i].pos.z - 4) / 16.0) * 16; 
                        chest2.x = floor((pieces[i].pos.x + 12) / 16.0) * 16; 
                        chest2.z = floor((pieces[i].pos.z - 6) / 16.0) * 16; 
                    } break;
                
                    default:
                        break;
                }
                
                if (chest1.x == chest2.x && chest1.z == chest2.z) {
                    if (end_city_loot_check(world_seed, 2, chest1.x, chest1.z)) return 1;
                } else {
                    if (end_city_loot_check(world_seed, 1, chest1.x, chest1.z)) return 1;
                    if (end_city_loot_check(world_seed, 1, chest2.x, chest2.z)) return 1;
                }
            } break;
            
            default:
                break;
        }
    }

    return 0;
}

void print_end_city_entry(EndCityLootOut* entry) {
    char name[45];

    switch (entry->id) {
        case 0: { strcpy(name, "minecraft:diamond"); } break;
        case 1: { strcpy(name, "minecraft:iron_ingot"); } break;
        case 2: { strcpy(name, "minecraft:gold_ingot"); } break;
        case 3: { strcpy(name, "minecraft:emerald"); } break;
        case 4: { strcpy(name, "minecraft:beetroot_seeds"); } break;
        case 5: { strcpy(name, "minecraft:saddle"); } break;
        case 6: { strcpy(name, "minecraft:iron_horse_armor"); } break;
        case 7: { strcpy(name, "minecraft:golden_horse_armor"); } break;
        case 8: { strcpy(name, "minecraft:diamond_horse_armor"); } break;
        case 9: { strcpy(name, "minecraft:diamond_sword"); } break;
        case 10: { strcpy(name, "minecraft:diamond_boots"); } break;
        case 11: { strcpy(name, "minecraft:diamond_chestplate"); } break;
        case 12: { strcpy(name, "minecraft:diamond_leggings"); } break;
        case 13: { strcpy(name, "minecraft:diamond_helmet"); } break;
        case 14: { strcpy(name, "minecraft:diamond_pickaxe"); } break;
        case 15: { strcpy(name, "minecraft:diamond_shovel"); } break;
        case 16: { strcpy(name, "minecraft:iron_sword"); } break;
        case 17: { strcpy(name, "minecraft:iron_boots"); } break;
        case 18: { strcpy(name, "minecraft:iron_chestplate"); } break;
        case 19: { strcpy(name, "minecraft:iron_leggings"); } break;
        case 20: { strcpy(name, "minecraft:iron_helmet"); } break;
        case 21: { strcpy(name, "minecraft:iron_pickaxe"); } break;
        case 22: { strcpy(name, "minecraft:iron_shovel"); } break;
        case 23: { strcpy(name, "minecraft:spire_armor_trim_smithing_template"); } break;
    
        default:
            break;
    }

    if (entry->enchanted) {
        printf("(enchanted) %s (%d)\n", name, entry->ammount);
    } else {
        printf("%s (%d)\n", name, entry->ammount);
    }
}

// FIX THIS!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1 //
int get_end_city_chest_loot_looting(const uint64_t loot_seed, EndCityLootOut *loot) {
    uint64_t rand;
    setSeed(&rand, loot_seed);

    int rolls = nextInt(&rand, 5) + 2;
    for (int i = 0; i < rolls; ++i) {
        int w = nextInt(&rand, WEIGHT_SUM);

        for (int j = 0; j < TOTAL_LOOT_ENTRIES; ++j) {
            EndCityLootEntry entry = loot_entries[j];
            w -= entry.weight;

            if (w >= 0) continue;

            int ammount = entry.min == entry.max ? entry.max : nextInt(&rand, entry.max - entry.min + 1) + entry.min;
            loot[i].enchanted = false;
            if (entry.enchant) {
                int enchant_level = nextInt(&rand, entry.enchant_max - entry.enchant_min + 1) + entry.enchant_min;

                if (entry.id == 9 || entry.id == 16) {
                    int looting = enchant_with_levels_looting_check(&rand, enchant_level, entry.enchant_value);
                
                    if (looting) {
                        loot[i].enchanted = true;
                    }
                } else {
                    enchant_with_levels_java(&rand, enchant_level, entry.enchant_value);
                }
            }

            loot[i].ammount = ammount;
            loot[i].id = entry.id;

            break;
        }
    }

    // Do trim roll.
    if (nextInt(&rand, 15) == 14) {
        loot[rolls].ammount = 1;    
        loot[rolls].enchanted = false;    
        loot[rolls].id = 23;
        ++rolls;
    }

    return rolls;
}

int end_city_looting_check(const uint64_t seed, const int chests, const int x, const int z) {
    uint64_t loot_seeds[2];
    EndCityLootOut loot[7];
    Xoroshiro xr;

    uint64_t decorator_seed = get_decorator_seed(seed, x, z, END_CITY_SALT);
    xSetSeed(&xr, decorator_seed);

    get_loot_seeds(&xr, loot_seeds, chests);

    for (int j = 0; j < chests; ++j) {
        int entries = get_end_city_chest_loot_looting(loot_seeds[j], loot);
        
        for (int i = 0; i < entries; ++i) {
            // Check for the trim.
            if ((loot[i].id == 9 || loot[i].id == 16) && loot[i].enchanted == true) {
                return 1;
            }
        }
    }

    return 0;
}

int check_end_city_for_looting(const uint64_t world_seed, const int chunk_x, const int chunk_z) {
    Piece pieces[END_CITY_PIECES_MAX];
    int pieces_n = getEndCityPieces(pieces, world_seed, chunk_x, chunk_z);

    for (int i = 0; i < pieces_n; ++i) {
        switch (pieces[i].type) {
            case THIRD_FLOOR_2: {
                Pos chest;

                switch (pieces[i].rot) {
                    case 0: { // ok
                        chest.x = (int)floor((pieces[i].pos.x + 5) / 16.0) * 16; 
                        chest.z = (int)floor((pieces[i].pos.z + 1) / 16.0) * 16; 
                    } break;
                    
                    case 1: { // ok
                        chest.x = (int)floor((pieces[i].pos.x - 3) / 16.0) * 16; 
                        chest.z = (int)floor((pieces[i].pos.z + 5) / 16.0) * 16; 
                    } break;
                
                    case 2: { // ok
                        chest.x = (int)floor((pieces[i].pos.x - 7) / 16.0) * 16; 
                        chest.z = (int)floor((pieces[i].pos.z - 3) / 16.0) * 16; 
                    } break;
                
                    case 3: { // ok
                        chest.x = (int)floor((pieces[i].pos.x + 1) / 16.0) * 16; 
                        chest.z = (int)floor((pieces[i].pos.z - 7) / 16.0) * 16; 
                    } break;
                
                    default:
                        break;
                }

                if (end_city_looting_check(world_seed, 1, chest.x, chest.z)) return 1;
            } break;

            case END_SHIP: {
                Pos chest1;
                Pos chest2;

                switch (pieces[i].rot) {
                    case 0: { //ok
                        chest1.x = (int)floor((pieces[i].pos.x + 6) / 16.0) * 16; 
                        chest1.z = (int)floor((pieces[i].pos.z + 6) / 16.0) * 16; 
                        chest2.x = (int)floor((pieces[i].pos.x + 4) / 16.0) * 16; 
                        chest2.z = (int)floor((pieces[i].pos.z + 6) / 16.0) * 16; 
                    } break;
                    
                    case 1: { // ok
                        chest1.x = (int)floor((pieces[i].pos.x - 8) / 16.0) * 16; 
                        chest1.z = (int)floor((pieces[i].pos.z + 6) / 16.0) * 16; 
                        chest2.x = (int)floor((pieces[i].pos.x - 8) / 16.0) * 16; 
                        chest2.z = (int)floor((pieces[i].pos.z + 4) / 16.0) * 16; 
                    } break;
                
                    case 2: { // ok
                        chest1.x = (int)floor((pieces[i].pos.x - 6) / 16.0) * 16; 
                        chest1.z = (int)floor((pieces[i].pos.z - 8) / 16.0) * 16; 
                        chest2.x = (int)floor((pieces[i].pos.x - 8) / 16.0) * 16; 
                        chest2.z = (int)floor((pieces[i].pos.z - 8) / 16.0) * 16; 
                    } break;
                
                    case 3: { // ok
                        chest1.x = (int)floor((pieces[i].pos.x + 6) / 16.0) * 16; 
                        chest1.z = (int)floor((pieces[i].pos.z - 8) / 16.0) * 16; 
                        chest2.x = (int)floor((pieces[i].pos.x + 6) / 16.0) * 16; 
                        chest2.z = (int)floor((pieces[i].pos.z - 6) / 16.0) * 16; 
                    } break;
                
                    default:
                        break;
                }
                
                if (chest1.x == chest2.x && chest1.z == chest2.z) {
                    if (end_city_looting_check(world_seed, 2, chest1.x, chest1.z)) return 1;
                } else {
                    if (end_city_looting_check(world_seed, 1, chest1.x, chest1.z)) return 1;
                    if (end_city_looting_check(world_seed, 1, chest2.x, chest2.z)) return 1;
                }

            } break;
            
            case FAT_TOWER_TOP: {
                Pos chest1;
                Pos chest2;

                switch (pieces[i].rot) {
                    case 0: { //
                        chest1.x = floor((pieces[i].pos.x + 2) / 16.0) * 16; 
                        chest1.z = floor((pieces[i].pos.z + 10) / 16.0) * 16; 
                        chest2.x = floor((pieces[i].pos.x + 4) / 16.0) * 16; 
                        chest2.z = floor((pieces[i].pos.z + 12) / 16.0) * 16; 
                    } break;
                    
                    case 1: { // ok
                        chest1.x = floor((pieces[i].pos.x - 12) / 16.0) * 16; 
                        chest1.z = floor((pieces[i].pos.z + 2) / 16.0) * 16; 
                        chest2.x = floor((pieces[i].pos.x - 14) / 16.0) * 16; 
                        chest2.z = floor((pieces[i].pos.z + 4) / 16.0) * 16; 
                    } break;
                
                    case 2: { // ok
                        chest1.x = floor((pieces[i].pos.x - 6) / 16.0) * 16; 
                        chest1.z = floor((pieces[i].pos.z - 14) / 16.0) * 16; 
                        chest2.x = floor((pieces[i].pos.x - 4) / 16.0) * 16; 
                        chest2.z = floor((pieces[i].pos.z - 12) / 16.0) * 16; 
                    } break;
                 
                    case 3: { // ok
                        chest1.x = floor((pieces[i].pos.x + 10) / 16.0) * 16; 
                        chest1.z = floor((pieces[i].pos.z - 4) / 16.0) * 16; 
                        chest2.x = floor((pieces[i].pos.x + 12) / 16.0) * 16; 
                        chest2.z = floor((pieces[i].pos.z - 6) / 16.0) * 16; 
                    } break;
                
                    default:
                        break;
                }
                
                if (chest1.x == chest2.x && chest1.z == chest2.z) {
                    if (end_city_looting_check(world_seed, 2, chest1.x, chest1.z)) return 1;
                } else {
                    if (end_city_looting_check(world_seed, 1, chest1.x, chest1.z)) return 1;
                    if (end_city_looting_check(world_seed, 1, chest2.x, chest2.z)) return 1;
                }
            } break;
            
            default:
                break;
        }
    }

    return 0;
}
