// #include "end_city.h"
#include "end_city.h"
#include "utils.h"
#include "cubiomes.h"
#include "MiLTSU.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>
#include <inttypes.h>

#define MC_VERSION MC_1_20

#define DESERT_PYRAMID_SALT 40001
#define BASTION_SALT 40000

typedef struct {
    int start_x;
    int start_z;
    int rotation;
} BastionData;


// Code for figurering (I don't know how to spell that word) out decoration salts.
// for (int index = 0; index < 20; index++) {
//     for (int step = 0; step < 10; step++) {
//         for (int offset = 0; offset < 1000; offset++) {
//             int salt_guess = 10000*step + index;
//             uint64_t decorator_seed = get_decorator_seed(seed, x, z, salt_guess);
//             xSetSeed(&xr, decorator_seed);
//             xSkipN(&xr, offset);

//             uint64_t n = xNextLongJ(&xr);
//             if (
//                 n == (uint64_t)-4881671169037925758 ||
//                 n == (uint64_t)1589205113207312453 ||
//                 n == (uint64_t)568076451594299484 ||
//                 n == (uint64_t)5457058396279416995
//             ) {
//                 printf("%d\n", salt_guess);
//             }
//         }
//     }
// }

int next_has_torchflower_seed(Xoroshiro *xr) {
    return !xNextInt(xr, 2);
}

int next_has_rod(Xoroshiro *xr) {
    return xNextInt(xr, 2);
}

int next_has_skull(const int looting_level, Xoroshiro *xr) {
    // Throw out bones and coal random calls (looting, no matter the level, is just 1 extra call for both coal and bones)
    for (int i = 0; i < 2 + (looting_level > 0 ? 2 : 0); i++) {
        xNextInt(xr, 1);
    }

    // Actual skull rng
    return xNextFloat(xr) < 0.025 + (0.01 * looting_level);
}

void next_vault_common(Xoroshiro *xr) {
    int n = xNextInt(xr, 25);
    if (n < 4) {
        // Arrows.
        // Roll count.
        xNextInt(xr, 1);
    } else if (n >= 4 && n < 8) {
        // Tipped arrows.
        // Roll count.
        xNextInt(xr, 1);
    } else if (n >= 8 && n < 12) {
        // Emeralds.
        // Roll count.
        xNextInt(xr, 1);
    } else if (n >= 12 && n < 15) {
        // Wind charges.
        // Roll count.
        xNextInt(xr, 1);
    } else if (n >= 15 && n < 18) {
        // Iron ingots.
        // Roll count.
        xNextInt(xr, 1);
    } else if (n >= 18 && n < 21) {
        // Honey bottles.
        // Roll count.
        xNextInt(xr, 1);
    } else if (n >= 21 && n < 23) {
        // Ominous bottle.
        // Roll amplifier.
        xNextInt(xr, 1);
    } else if (n == 23) {
        // Wind charges (more).
        // Roll count.
        xNextInt(xr, 1);
    } else if (n == 24) {
        // Diamond(s).
        // Roll count.
        xNextInt(xr, 1);
    }
}

void next_vault_rare(Xoroshiro *xr) {
    int n = xNextInt(xr, 23);
    if (n < 3) {
        // Emeralds.
        // Roll count.
        xNextInt(xr, 1);
    } else if (n >= 3 && n < 6) {
        // Shield.
        // Roll damage.
        xNextInt(xr, 1);
    } else if (n >= 6 && n < 9) {
        // Enchanted Bow.
        // Roll enchantment.
        enchant_with_levels(xr, xNextInt(xr, 11) + 5, 1);
    } else if (n >= 9 && n < 11) {
        // Crossbow with a random enchantment.
        enchant_with_levels(xr, xNextInt(xr, 11), 1);
    } else if (n >= 11 && n < 13) {
        // Iron axe.
        // Roll enchantment.
        enchant_with_levels(xr, xNextInt(xr, 11), 14);
    } else if (n >= 13 && n < 15) {
        // Iron chestplate.
        // Roll enchantment.
        enchant_with_levels(xr, xNextInt(xr, 11), 9);
    } else if (n >= 15 && n < 17) {
        // Golden Carrot(s).
        // Roll count.
        xNextInt(xr, 1);
    } else if (n >= 17 && n < 19) {
        // Enchanted book (bad).
        // Roll enchantment.
        int e = xNextInt(xr, 6);
        // Roll enchantment level.
        if (e != 4) xNextInt(xr, 1);
    } else if (n >= 19 && n < 21) {
        // Enchanted book (good - riptide, channeling, mending).
        // Roll enchantment.
        int e = xNextInt(xr, 5);
        // Roll enchantment level.
        if (e != 2 && e != 4) xNextInt(xr, 1);
    } else if (n == 21) {
        // Diamond chestplate.
        // Roll enchantment.
        enchant_with_levels(xr, xNextInt(xr, 11) + 5, 10);
    } else if (n == 22) {
        // Diamond axe.
        // Roll enchantment.
        enchant_with_levels(xr, xNextInt(xr, 11) + 5, 10);
    }
}

int next_vault_unique(Xoroshiro *xr) {
    // Choose a common or a rare roll.
    if (xNextInt(xr, 10) < 8) {
        // Rare.
        next_vault_rare(xr);
    } else {
        // Common.
        next_vault_common(xr);
    }   

    // Common rolls.
    int n = xNextInt(xr, 3) + 1;
    for (int i = 0; i < n; ++i) {
        next_vault_common(xr);
    }

    // Check for unique roll.
    if (xNextFloat(xr) >= 0.25) return -1;
    
    // Return unique roll.
    return xNextInt(xr, 12);
}

void next_ominous_vault_common(Xoroshiro *xr) {
    int n = xNextInt(xr, 15);
    if (n < 5) {
        // Emeralds.
        xNextInt(xr, 1);
    } else if (n >= 5 && n < 9) {
        // Wind charges.
        xNextInt(xr, 1);
    } else if (n >= 9 && n < 12) {
        // Tipped arrows
        xNextInt(xr, 1);
    } else if (n >= 12 && n < 14) {
        // Diamonds
        xNextInt(xr, 1);
    } else if (n == 14) {
        // Ominous bottle.
        // Roll the amplifier.
        xNextInt(xr, 1);
    }
}

void next_ominous_vault_rare(Xoroshiro *xr) {
    int n = xNextInt(xr, 29);
    if (n < 5) {
        // Emerald block.
    } else if (n >= 5 && n < 9) {
        // Iron block.
    } else if (n >= 9 && n < 13) {
        // Crossbow with a random enchantment.
        enchant_with_levels(xr, xNextInt(xr, 16) + 5, 1);
    } else if (n >= 13 && n < 16) {
        // Golden apple.
    } else if (n >= 16 && n < 19) {
        // Diamond axe with a random enchantment.
        enchant_with_levels(xr, xNextInt(xr, 11) + 10, 10);
    } else if (n >= 19 && n < 22) {
        // Diamond chestplate with a random enchantment.
        enchant_with_levels(xr, xNextInt(xr, 11) + 10, 10);
    } else if (n >= 22 && n < 24) {
        // Random enchanted book.
        int e = xNextInt(xr, 5);
        // Random enchantment level.
        if (e != 4) xNextInt(xr, 2);
    } else if (n >= 24 && n < 26) {
        // Braech or Density.
        xNextInt(xr, 2);
        // Random level.
        xNextInt(xr, 2);
    } else if (n >= 28 && n < 28) {
        // Wind burst with a random level.
        xNextInt(xr, 1);
    } else if (n == 28) {
        // Diamond block.
    }
}

int next_ominous_vault_unique(Xoroshiro *xr) {
    // Choose a common or a rare roll.
    if (xNextInt(xr, 10) < 8) {
        // Rare.
        next_ominous_vault_rare(xr);
    } else {
        // Common.
        next_ominous_vault_common(xr);
    }   

    // Common rolls.
    int n = xNextInt(xr, 3) + 1;
    for (int i = 0; i < n; ++i) {
        next_ominous_vault_common(xr);
    }

    // Check for unique roll.
    if (xNextFloat(xr) >= 0.75) return -1;
    
    // Return unique roll.
    return xNextInt(xr, 10);
}

char* reward_path = "minecraft:chests/trial_chambers/reward";
char* ominous_reward_path = "minecraft:chests/trial_chambers/reward_ominous";
int filter_vaults(const uint64_t seed, const int rolls, const int check_trident) {
    Xoroshiro xoro_reward = getRandomSequenceXoro(seed, reward_path);
    Xoroshiro xoro_ominous_reward = getRandomSequenceXoro(seed, ominous_reward_path);

    if (check_trident) return 0;

    int trident = 0;
    int core = 0;
    for (int i = 0; i < rolls; ++i) {
        if (check_trident) {
            if (next_ominous_vault_unique(&xoro_ominous_reward) == 9) trident = 1;
            if (next_vault_unique(&xoro_reward) == 11) core = 1;
            return trident && core; 
        } else {
            if (next_ominous_vault_unique(&xoro_ominous_reward) == 9) {
                return 1;
            }
        }
    }

    return 0;
}

char* bartering_str = "minecraft:gameplay/piglin_bartering";
int filter_pearls(const uint64_t seed, const int gold, const int pearls) {
    if (pearls <= 0 || gold <= 0) return 0;
    Xoroshiro xoro = getRandomSequenceXoro(seed, bartering_str);
    
    int rolled_pearls = 0;  
    for (int i = 0; i < gold; ++i) {
        rolled_pearls += nextPearls(&xoro);
    }

    return rolled_pearls >= pearls;
}

char* wither_skeleton_str = "minecraft:entities/wither_skeleton";
int filter_skulls(const uint64_t seed, const int kills, const int skulls, const int looting_level) {
    if (kills < 0 || skulls <= 0 || kills < skulls || looting_level < 0 || looting_level > 3) return 0;
    Xoroshiro xoro = getRandomSequenceXoro(seed, wither_skeleton_str);

    int rolled_skulls = 0;
    for (int i = 0; i < kills; ++i) {
        rolled_skulls += next_has_skull(looting_level, &xoro);
    }

    return rolled_skulls >= skulls;
}

int filter_skulls_rng_manipulation(const uint64_t seed, const int max_kills, const int skulls) {
    for (uint16_t i = 0; i < pow(2, max_kills); ++i) {
        Xoroshiro xr = getRandomSequenceXoro(seed, wither_skeleton_str);
        int skulls = 0;
        int kills = 0;

        for (int j = 0; j < max_kills; ++j) {
            int looting_level = (i >> j) & 1 == 1 ? 0 : 3;
            skulls += next_has_skull(looting_level, &xr);
            ++kills;

            if (skulls == 3) {
                return 1;
            }
        }
    }

    return 0;
}

char* blaze_str = "minecraft:entities/blaze";
int filter_blaze_rods(const uint64_t seed, const int kills, const int rods) {
    if (kills < 0 || rods <= 0 || kills < rods) return 0;
    Xoroshiro xoro = getRandomSequenceXoro(seed, blaze_str);

    int rolled_rods = 0;
    for (int i = 0; i < kills; ++i) {
        rolled_rods += next_has_rod(&xoro);
    }

    return rolled_rods >= rods;
}

char* sniffer_str = "minecraft:gameplay/sniffer_digging";
int filter_torchflower_seeds(const uint64_t seed, const int sniffs, const int seeds) {
    Xoroshiro xoro = getRandomSequenceXoro(seed, sniffer_str);

    int rolled_seeds = 0;
    for (int i = 0; i < sniffs; ++i) {
        rolled_seeds += next_has_torchflower_seed(&xoro);
    }
    return rolled_seeds >= seeds;
}

int check_pyramid(const uint64_t lower48, const int max_dist) {
    Pos pyramid_pos;
    Pos spawn_pos;
    Generator g;
    setupGenerator(&g, MC_VERSION, 0);
    
    applySeed(&g, DIM_OVERWORLD, lower48);
    int got_spawn = 0;
    for (int r_x = -1; r_x <= 0; ++r_x) {
        for (int r_z = -1; r_z <= 0; ++r_z) {
            if (!getStructurePos(Desert_Pyramid, MC_VERSION, g.seed, r_x, r_z, &pyramid_pos)) {
                continue;
            }
            
            if (isViableStructurePos(Desert_Pyramid, &g, pyramid_pos.x, pyramid_pos.z, 0) && isViableStructureTerrain(Desert_Pyramid, &g, pyramid_pos.x, pyramid_pos.z)) {
                if (!got_spawn) {
                    spawn_pos = getSpawn(&g);   
                    got_spawn = 1;
                }
                if (abs(pyramid_pos.x - spawn_pos.x) <= max_dist && abs(pyramid_pos.z - spawn_pos.z) <= max_dist) {
                    return 1;
                }
            }
        }
    }

    return 0;
}

int check_generation_attempt(const uint64_t lower48, const int max_pyramid_dist, const int max_village_dist, Pos* pyramid_pos, Pos* village_pos) {
    // Piramidy.
    if (!getStructurePos(Desert_Pyramid, MC_VERSION, lower48, 0, 0, pyramid_pos)) {
        return 0;
    }

    if (pyramid_pos->x > max_pyramid_dist || pyramid_pos->z > max_pyramid_dist) {
        return 0;
    }

    // Wioski.
    if (!getStructurePos(Village, MC_VERSION, lower48, 0, 0, village_pos)) {
        return 0;
    }

    if (abs(village_pos->x - pyramid_pos->x) > max_village_dist || abs(village_pos->z - pyramid_pos->z) > max_village_dist) {
        return 0;
    }

    return 1;
}

int check_spawn(const uint64_t seed, Pos* pyramid_pos, Pos* village_pos, Generator g) {
    applySeed(&g, DIM_OVERWORLD, seed);
    
    return (
        isViableStructurePos(Desert_Pyramid, &g, pyramid_pos->x, pyramid_pos->z, 0) && 
        isViableStructureTerrain(Desert_Pyramid, &g, pyramid_pos->x, pyramid_pos->z) &&
        isViableStructurePos(Village, &g, village_pos->x, village_pos->z, 0));
}

int check_pyramid_village_bad(const uint64_t lower48, const int max_pyramid_dist, const int max_village_dist, Generator g) {
    Pos pyramid_pos;
    Pos spawn_pos = {0, 0};
    // Generator g;
    // setupGenerator(&g, MC_VERSION, 0);
    
    applySeed(&g, DIM_OVERWORLD, lower48);
    // int got_spawn = 0;
    for (int r_x = -1; r_x <= 0; ++r_x) {
        for (int r_z = -1; r_z <= 0; ++r_z) {
            if (!getStructurePos(Desert_Pyramid, MC_VERSION, g.seed, r_x, r_z, &pyramid_pos)) {
                continue;
            }
            
            if (isViableStructurePos(Desert_Pyramid, &g, pyramid_pos.x, pyramid_pos.z, 0) && isViableStructureTerrain(Desert_Pyramid, &g, pyramid_pos.x, pyramid_pos.z)) {
                // if (!got_spawn) {
                //     spawn_pos = getSpawn(&g);   
                //     got_spawn = 1;
                // }
                if (abs(pyramid_pos.x - spawn_pos.x) <= max_pyramid_dist && abs(pyramid_pos.z - spawn_pos.z) <= max_pyramid_dist) {
                    goto village_check;
                }
            }
        }
    }

    return 0;

village_check:
    Pos village_pos;
    for (int r_x = -1; r_x <= 0; ++r_x) {
        for (int r_z = -1; r_z <= 0; ++r_z) {
            if (!getStructurePos(Village, MC_VERSION, g.seed, r_x, r_z, &village_pos)) {
                continue;
            }
            
            if (isViableStructurePos(Village, &g, village_pos.x, village_pos.z, 0)) {
                if (abs(pyramid_pos.x - village_pos.x) <= max_village_dist && abs(pyramid_pos.z - village_pos.z) <= max_village_dist) {
                    return 1;
                }
            }
        }
    }

    return 0;
}
int check_nether(const uint64_t lower48, Generator g, BastionData* bastion_data) {
    const int max_bastion_dist = 96; // 96
    const int max_fortress_dist = 96;
    const int max_structure_dist = 96; // 96

    Pos bastion_pos;
    Pos fortress_pos;
    
    StructureVariant bastion_type;
    
    // Generator g;
    // setupGenerator(&g, MC_VERSION, 0);
    applySeed(&g, DIM_NETHER, lower48);

    int found_bastion = 0;
    int found_fortress = 0;

    for (int r_x = -1; r_x <= 0; ++r_x) {
        for (int r_z = -1; r_z <= 0; ++r_z) {
            if (!found_bastion && getStructurePos(Bastion, g.mc, g.seed, r_x, r_z, &bastion_pos)) {
                if (
                    abs(bastion_pos.x) <= max_bastion_dist && 
                    abs(bastion_pos.z) <= max_bastion_dist && 
                    isViableStructurePos(Bastion, &g, bastion_pos.x, bastion_pos.z, 0)
                ) {
                    getVariant(&bastion_type, Bastion, g.mc, g.seed, bastion_pos.x, bastion_pos.z, -1);
                    if (bastion_type.start == 0) { // start = 0 -> Housing.
                        found_bastion = 1;

                        bastion_data->start_x = bastion_pos.x;
                        bastion_data->start_z = bastion_pos.z;
                        bastion_data->rotation = bastion_type.rotation;
                    } 
                }
            }

            if (!found_fortress && getStructurePos(Fortress, g.mc, g.seed, r_x, r_z, &fortress_pos)) {
                if ( 
                    abs(fortress_pos.x) <= max_fortress_dist && 
                    abs(fortress_pos.z) <= max_fortress_dist &&
                    isViableStructurePos(Fortress, &g, fortress_pos.x, fortress_pos.z, 0)
                ) {
                    found_fortress = 1;
                }
            }

            if (found_bastion && found_fortress) {
                if (
                    abs(bastion_pos.x - fortress_pos.x) <= max_structure_dist &&
                    abs(bastion_pos.z - fortress_pos.z) <= max_structure_dist
                ) {
                    return 1;
                } else {
                    return 0;
                }
            }
        }
    }

    return 0;
}

char* fishing_str = "minecraft:gameplay/fishing";
char* treasure_str = "minecraft:gameplay/fishing/treasure";
int next_has_shell(Xoroshiro* xr_fishing, Xoroshiro* xr_treasure, uint64_t seed) {
    int n = xNextInt(xr_fishing, 100) + 1;

    // if (n <= 10) {
    //     int k = xNextInt(xr_fishing, 100);

    //     if ((k >= 17 && k < 27) || (k >= 62 && k < 64)) {
    //         // enchant_with_levels(xr, )
    //         // printf("Wentka\n");
    //         xNextInt(xr_fishing, 1);
    //     }
    // }

    if (n >= 11 && n <= 15) {
        int k = xNextInt(xr_fishing, 6);
        
        if (k == 2 || k == 3) {
            xNextInt(xr_fishing, 1);
            enchant_with_levels(xr_fishing, xNextInt(xr_fishing, 8) + 23, 1);
        }

        if (k == 4) {
            enchant_with_levels(xr_fishing, 30, 1);
            return 2;
        }

        if (k == 5) {
            return 1;
        }
    }
    
    return 0;
}

int next_has_shell2(Xoroshiro* xr_fishing, Xoroshiro* xr_treasure, uint64_t seed) {
    int n = xNextInt(xr_fishing, 100) + 1;

    if (n >= 11 && n <= 15) {
        if (xNextInt(xr_fishing, 6) == 5) {
            return 1;
        }
    }
    
    return 0;
}

char* elder_guardian_str = "minecraft:entities/elder_guardian";
int next_has_tide(int looting_level, Xoroshiro* xr) {
    xNextLong(xr);
    xNextLong(xr);
    if (looting_level >= 1) {
        xNextLong(xr);
        xNextLong(xr);
    }
    float extra_fish = xNextFloat(xr);
    if (extra_fish < 0.025f) {
        xNextLong(xr);
        if (looting_level >= 1) {
            xNextLong(xr);
        }
    }

    return xNextInt(xr, 5) == 4;
}

int filter_elder_guardian(uint64_t seed, int max_kills) {
    for (uint16_t i = 0; i < pow(2, max_kills); ++i) {
        Xoroshiro xr = getRandomSequenceXoro(seed, elder_guardian_str);
        int kills = 0;

        for (int j = 0; j < max_kills; ++j) {
            int looting_level = (i >> j) & 1 == 1 ? 3 : 0;
            ++kills;

            if (next_has_tide(looting_level, &xr)) {
                return 1;
            }
        }
    }
    return 0;
}

int delete_this(uint64_t seed) {
    Xoroshiro xr1 = getRandomSequenceXoro(seed, elder_guardian_str);
    Xoroshiro xr2 = getRandomSequenceXoro(seed, elder_guardian_str);

    if (next_has_tide(0, &xr1) && !next_has_tide(3, &xr1))
        return 1;
    return 0;
}

int next_has_shulker_shell(const int looting_level, Xoroshiro* xr) {
    return xNextFloat(xr) < 0.5f + 0.0625f * (float)looting_level;
}

char* shulker_str = "minecraft:entities/shulker";
int filter_shulker_shells(const uint64_t seed, const int looting_level, const int shells_goal, const int max_kills) {
    Xoroshiro xr = getRandomSequenceXoro(seed, shulker_str);
    int shells = 0;

    for (int i = 0; i < max_kills; ++i) {
        shells += next_has_shulker_shell(looting_level, &xr);
    }

    return shells >= shells_goal;
}

int check_pyramid_loot(const uint64_t seed, const int x, const int z) {
    Xoroshiro xr;
    uint64_t rand;
    uint64_t decorator_seed = get_decorator_seed(seed, x, z, DESERT_PYRAMID_SALT);
    xSetSeed(&xr, decorator_seed);

    int mending = 0;
    int unbreaking = 0;

    xSkipN(&xr, 1);
    for (int i = 0; i < 4; ++i) {
        uint64_t loot_seed = xNextLongJ(&xr);
        setSeed(&rand, loot_seed);

        int rolls = nextInt(&rand, 3) + 2;
        for (int j = 0; j < rolls; ++j) {
            int weight = nextInt(&rand, 232);
            if (weight < 125) {
                nextInt(&rand, 1);
            } else if (weight >= 175 && weight < 195) {
                if (MC_VERSION >= MC_1_21) {
                    int book = nextInt(&rand, 39);
                    if (book != 17 && book != 23 && book != 29 && book != 30 && book != 35 && book != 36 && book != 38 ) {
                        if (book == 18) {
                            int level = nextInt(&rand, 3);
                            if (level == 2) {
                                unbreaking = 1;
                            }
                        } else {
                            nextInt(&rand, 10);
                        }
                    } else {
                        if (book == 38) {
                            mending = 1;
                        }
                    }
                } else {
                    /*   
                    0 - protection
                    1 - fire_protection
                    2 - feather_falling
                    3 - blast_protection
                    4 - projectile_protection
                    5 - respiration
                    6 - aqua_affinity
                    7 - thorns
                    8 - depth_strider
                    9 - frost_walker
                    10 - binding_curse
                    11 - sharpness
                    12 - smite
                    13 - bane_of_arthropods
                    14 - knockback
                    15 - fire_aspect
                    16 - looting
                    17 - sweeping
                    18 - efficiency
                    19 - silk_touch
                    20 - unbreaking
                    21 - fortune
                    22 - power
                    23 - punch
                    24 - flame
                    25 - infinity
                    26 - luck_of_the_sea
                    27 - lure
                    28 - loyalty
                    29 - impaling
                    30 - riptide
                    31 - channeling
                    32 - multishot
                    33 - quick_charge
                    34 - piercing
                    35 - mending
                    36 - vanishing_curse*/

                    int book = nextInt(&rand, 37);
                    if (book != 10 && book != 19 && book != 25 && book != 31 && book != 32 && book != 35 && book != 36) {
                        if (book == 20) {
                            int level = nextInt(&rand, 3);
                            if (level == 2) {
                                unbreaking = 1;
                            }
                        } else {
                            nextInt(&rand, 10);
                        }
                    } else {
                        if (book == 35) {
                            mending = 1;
                        }
                    }
                }
            }
        }
    }

    return mending && unbreaking;
    /*
    // Przed 1.21
    0 - protection
    1 - fire_protection
    2 - feather_falling
    3 - blast_protection
    4 - projectile_protection
    5 - respiration
    6 - aqua_affinity
    7 - thorns
    8 - depth_strider
    9 - frost_walker
    10 - binding_curse
    11 - sharpness
    12 - smite
    13 - bane_of_arthropods
    14 - knockback
    15 - fire_aspect
    16 - looting
    17 - sweeping
    18 - efficiency
    19 - silk_touch
    20 - unbreaking
    21 - fortune
    22 - power
    23 - punch
    24 - flame
    25 - infinity
    26 - luck_of_the_sea
    27 - lure
    28 - loyalty
    29 - impaling
    30 - riptide
    31 - channeling
    32 - multishot
    33 - quick_charge
    34 - piercing
    35 - mending
    36 - vanishing_curse

    // 1.21
    0 - protection,
    1 - fire_protection,
    2 - feather_falling,
    3 - blast_protection,
    4 - projectile_protection,
    5 - respiration,
    6 - aqua_affinity,
    7 - thorns,
    8 - depth_strider,
    9 - sharpness,
    10 - smite,
    11 - bane_of_arthropods,
    12 - knockback,
    13 - fire_aspect,
    14 - looting,
    15 - sweeping_edge,
    16 - efficiency,
    17 - silk_touch,
    18 - unbreaking,
    19 - fortune,
    20 - power,
    21 - punch,
    22 - flame,
    23 - infinity,
    24 - luck_of_the_sea,
    25 - lure,
    26 - loyalty,
    27 - impaling,
    28 - riptide,
    29 - channeling,
    30 - multishot,
    31 - quick_charge,
    32 - piercing,
    33 - density,
    34 - breach,
    35 - binding_curse,
    36 - vanishing_curse,
    37 - frost_walker,
    38 - mending
    */
}

int bastion_check_loot_check(uint64_t* rand) {
    int has_looting = 0;

    int weight = nextInt(rand, 89);
    // printf("weight1: %d\n", weight);
    if (weight >= 0 && weight < 6) {
        // kilof
        // 16 - efficiency,
        // 17 - silk_touch,
        // 18 - unbreaking,
        // 19 - fortune,
        // 36 - vanishing_curse,
        // 38 - mending
        int n = nextInt(rand, 6);
        if (n == 0 || n == 2 || n == 3) {
            nextInt(rand, 1);
        }
    } else if (weight >= 6 && weight < 12) {
        // łopata
    } else if (weight >= 12 && weight < 18) {
        // zniszczona trochę
        nextFloat(rand);
        // kusza
        // 18 - unbreaking,
        // 30 - multishot,
        // 31 - quick_charge,
        // 32 - piercing,
        // 36 - vanishing_curse,
        // 38 - mending
        int n = nextInt(rand, 6);
        if (n == 0 || n == 2 || n == 3) {
            nextInt(rand, 1);
        }
    } else if (weight >= 18 && weight < 30) {
        // to stare takie
    } else if (weight >= 30 && weight < 34) {
        // Złom
    } else if (weight >= 34 && weight < 44) {
        // Strzały
        nextInt(rand, 1);
    } else if (weight >= 44 && weight < 53) {
        // Banner
    } else if (weight >= 53 && weight < 58) {
        // Pigstep
    } else if (weight >= 58 && weight < 70) {
        // Marachewki
        nextInt(rand, 1);
    } else if (weight >= 70 && weight < 79) {
        // Jabłko
    } else if (weight >= 79 && weight < 89) {
        // Książka
        nextInt(rand, 1);
    }
    
    // What we actually care about.
    for (int i = 0; i < 2; ++i) {
        int weight = nextInt(rand, 20);
        // printf("weight%d: %d\n", i+2, weight);

        if (weight >= 0 && weight < 2) {
            // Miecz!!!
            // "sharpness", 
            // "smite", 
            // "bane_of_arthropods", 
            // "knockback", 
            // "fire_aspect", 
            // "looting", 
            // "sweeping_edge", 
            // "unbreaking", 
            // "vanishing_curse", 
            // "mending",
            nextFloat(rand);
            int n = nextInt(rand, 10);
            if (n <= 7) {
                if (n == 5) {
                    int level = nextInt(rand, 3);
                    if (level == 2) {
                        has_looting = 1;
                    }
                } else {
                    nextInt(rand, 1);
                }
            }
        } else if (weight >= 2 && weight < 4) {
            // Block żealaza
        } else if (weight >= 4 && weight < 5) {
            // Złote buty (zaklęte)
            nextInt(rand, 1);
        } else if (weight >= 5 && weight < 6) {
            // Złota siekiera
            // 9 - sharpness,
            // 10 - smite,
            // 11 - bane_of_arthropods,
            // 16 - efficiency,
            // 17 - silk_touch,
            // 18 - unbreaking,
            // 19 - fortune,
            // 36 - vanishing_curse,
            // 38 - mending
            int n = nextInt(rand, 9);
            if (n <= 3 || n == 5 || n == 6) {
                nextInt(rand, 1);
            }
        } else if (weight >= 6 && weight < 8) {
            // Blok złota
        } else if (weight >= 8 && weight < 10) {
            // Kusza (goła)
        } else if (weight >= 10 && weight < 12) {
            // Sztabki złota
            nextInt(rand, 1);
        } else if (weight >= 12 && weight < 14) {
            // Sztabki żelaza
            nextInt(rand, 1);
        } else if (weight >= 14 && weight < 15) {
            // Złoty miecz
        } else if (weight >= 15 && weight < 16) {
            // Złoty napierśnik (goły)
        } else if (weight >= 16 && weight < 17) {
            // Złoty hełm (goły)
        } else if (weight >= 17 && weight < 18) {
            // Złote spodnie (gołe)
        } else if (weight >= 18 && weight < 19) {
            // Złote buty (gołe)
        } else if (weight >= 19 && weight < 20) {
            // Płaczący obsydian
            nextInt(rand, 1);
        }
    }

    return has_looting;
}

int check_bastion_looting(const uint64_t seed, const int x, const int z, const int rotation) {
    Xoroshiro xr;
    uint64_t rand;
    uint64_t decorator_seed;
    uint64_t loot_seed;

    int has_looting = 0;
    
    int five_chests_x = x - 16;
    int five_chests_z = z + 16;

    int one_chest_x = x + 16;
    int one_chest_z = z + 16;

    switch (rotation) {
        case 0: {
            five_chests_x = x - 16;
            five_chests_z = z + 16;

            one_chest_x = x + 16;
            one_chest_z = z + 16;
        } break;

        case 1: {
            five_chests_x = x - 32;
            five_chests_z = z - 16;

            one_chest_x = x - 32;
            one_chest_z = z + 16;
        } break;
        
        case 2: {
            five_chests_x = x;
            five_chests_z = z - 32;

            one_chest_x = x - 32;
            one_chest_z = z - 32;
        } break;
        
        case 3: {
            five_chests_x = x + 16;
            five_chests_z = z;

            one_chest_x = x + 16;
            one_chest_z = z - 32;
        } break;
        
        default:
            break;
    }

    // printf("(%d, %d); (%d, %d); %d\n", one_chest_x, one_chest_z, five_chests_x, five_chests_z, rotation);

    decorator_seed = get_decorator_seed(seed, one_chest_x, one_chest_z, BASTION_SALT);
    xSetSeed(&xr, decorator_seed);

    loot_seed = xNextLongJ(&xr);
    // printf("%lld\n", loot_seed);
    setSeed(&rand, loot_seed);

    if (bastion_check_loot_check(&rand)) return 1;

    decorator_seed = get_decorator_seed(seed, five_chests_x, five_chests_z, BASTION_SALT);
    xSetSeed(&xr, decorator_seed);
    for (int i = 0; i < 5; ++i) {
        loot_seed = xNextLongJ(&xr);
        // printf("%lld\n", loot_seed);
        setSeed(&rand, loot_seed);

        if (bastion_check_loot_check(&rand)) return 1;
    }

    return 0;
}

/*
-- BASTIONY --
rotation 0 -> z+16 (x-16 -> 5 skrzynek; x+16 -> 1 skrzynka)
rotation 1 -> x-32 (z-16 -> 5 skrzynek; z+16 -> 1 skrzynka)
rotation 2 -> z-32 & x-16 (x+16 -> 5 skrzynek; x-16 -> 1 skrzynka)
rotation 3 -> x+16 & z-16 (z+16 -> 5 skrzynek; z-16 -> 1 skrzynka)
*/

// int main() {
//     for (uint64_t seed = 0; seed < UINT64_MAX; ++seed) {
//         Xoroshiro xr_fishing = getRandomSequenceXoro(seed, fishing_str);
//     Xoroshiro xr_treasure = getRandomSequenceXoro(seed, treasure_str);
//         if (next_has_shell2(&xr_fishing, &xr_treasure, seed) &&
//             next_has_shell2(&xr_fishing, &xr_treasure, seed) &&
//             next_has_shell2(&xr_fishing, &xr_treasure, seed)) {
//                 printf("%lld\n", seed);
//             }
//     }
// }

int main11111() {
    Pos pyramid_pos;
    Pos village_pos;
    Generator g;
    setupGenerator(&g, MC_VERSION, 0);

    for (uint64_t seed = 0;; ++seed) {
        if (delete_this(seed)) {
            printf("%lld\n", seed);
        }

        // applySeed(&g, DIM_OVERWORLD, seed);
        // if (check_generation_attempt(seed, 256, 256, &pyramid_pos, &village_pos) && check_spawn(seed, &pyramid_pos, &village_pos, g)) {
        //     if (check_pyramid_loot(seed, pyramid_pos.x, pyramid_pos.z)) {
        //         printf("Seed: %lld; Coordinates: (%d, %d)\n", seed, pyramid_pos.x, pyramid_pos.z);
        //     }
        // }
    }
}

int main87428934() {
    Pos pyramid_pos;
    Pos village_pos;
    Pos city_pos;
    BastionData bastion_data;

    Generator g;
    setupGenerator(&g, MC_VERSION, 0);

    FILE* input = fopen("VERSION_AASSG_NEU.txt", "r");
    if (input == NULL) {
        printf("[ERROR]: Couldn't open the input file.\n");
        exit(1);
    }

    char chunk[128];

    //  || check_bastion_looting(seed, bastion_data.start_x, bastion_data.start_z, bastion_data.rotation)

    while(fgets(chunk, sizeof(chunk), input) != NULL) {
        uint64_t seed = strtoull(chunk, NULL, 10);
        // uint64_t seed = 2380152399079092633;
        // -6599087034744729177

        // if (
        //     filter_elder_guardian(seed, 3) &&
        //     filter_shulker_shells(seed, 3, 4, 6) &&
        //     check_generation_attempt(seed, 96, 96, &pyramid_pos, &village_pos) &&
        //     // check_spawn()
        //     check_pyramid_loot(seed, pyramid_pos.x, pyramid_pos.z) &&
        //     check_nether(seed, g, &bastion_data) &&
        //     check_end_city(seed, 256, &city_pos) &&
        //     check_end_city_for_trim(seed, city_pos.x, city_pos.z) &&
        //     (check_bastion_looting(seed, bastion_data.start_x, bastion_data.start_z, bastion_data.rotation) || check_end_city_for_looting(seed, city_pos.x, city_pos.z)) &&
        //     // check_end_city_chest_number(seed, city_pos.x, city_pos.z) >= 3 &&
        //     filter_vaults(seed, 1, 1)
            
        // ) {
        //     printf("%lld\n", seed);
        // }
    }

    return 0;
}

/// ⚠️ BIG MESS WARNING ⚠️ ///
 
// 14 - third_floor_2 (1 skrzynka)
// rot = 0 -> x + 5 i z + 1 
// rot = 1 -> x - 1 i z + 5 
// rot = 2 -> x - 5 i z - 1 
// rot = 3 -> x + 1 i z - 5 

// 8 - fat_tower_top (2 skrzynki)
// 12, 4; 14, 5
// rot = 0 -> dalsza skrzynka: x + 5 i z + 14; bliższa skrzynka: x + 4 i z + 12; 
// rot = 1 -> dalsza skrzynka: x - 14 i z + 5; bliższa skrzynka: x - 12 i z + 4; 
// rot = 2 -> dalsza skrzynka: x - 5 i z - 14; bliższa skrzynka: x - 4 i z - 14; 
// rot = 3 -> dalsza skrzynka: x + 14 i z - 5; bliższa skrzynka: x + 14 i z - 4; 

// 12 - ship (2 skrzynki)
// rot = 0 -> dalsza skrzynka: x + 8 i z + 6; bliższa skrzynka: x + 6 i z + 6; 
// rot = 1 -> dalsza skrzynka: x - 6 i z + 8; bliższa skrzynka: x - 6 i z + 6; 
// rot = 2 -> dalsza skrzynka: x - 8 i z - 6; bliższa skrzynka: x - 6 i z - 6; 
// rot = 3 -> dalsza skrzynka: x + 6 i z - 8; bliższa skrzynka: x + 6 i z - 6; 

// int main2() {
    // const uint64_t seed = 2306408978509825121;
    // uint64_t seed = 0;
    // Pos pos;
    // for (uint64_t seed = 0;; ++seed) {
    //     if (check_end_city(seed, 128, &pos)) {
    //         // for (uint64_t n = 0; n < UINT16_MAX; ++n) {
    //             // uint64_t world_seed = (n << 48) | seed;
    //             if (check_end_city_for_trim(seed, pos.x, pos.z)) {
    //                 printf("seed: %lld\n", seed);

    //                 Piece pieces[END_CITY_PIECES_MAX];
    //                 int pieces_n = getEndCityPieces(pieces, seed, pos.x, pos.z);
    //                 for (int i = 0; i < pieces_n; ++i) {
    //                     if (pieces[i].type == 8 || pieces[i].type == 12 || pieces[i].type == 14)
    //                         printf("Name: %s, rot: %d; (%d, %d, %d)\n", pieces[i].name, pieces[i].rot, pieces[i].pos.x, pieces[i].pos.z, pieces[i].pos.y);
    //                 }
    //             }
    //         // }
    //     }
    // }
    // uint64_t loot_seeds[2];
    // EndCityLootOut loot[7];
    // Xoroshiro xr;

    // uint64_t decorator_seed = get_decorator_seed(seed, -8608, 1008, END_CITY_SALT);
    // xSetSeed(&xr, decorator_seed);

    // for (int s = 0; s < 2; ++s) {
    //     printf("// SKRZYNKA //\n");
    //     get_loot_seeds(&xr, loot_seeds, 1);
    //     int entries = get_end_city_chest_loot(loot_seeds[0], loot);
        
    //     for (int i = 0; i < entries; ++i) {
    //         print_end_city_entry(&loot[i]);
    //     }
    // }
// }

// 1000 -> 10 s

// int main() {
//     uint64_t seed = -4457095916096276322;
//     filter_elder_guardian(seed, 3);
// }

int main() {
    FILE* output = fopen("1_20_bastion.txt", "a");
    if (output == NULL) {
        printf("[ERROR]: Couldn't open the output file.\n");
        exit(1);
    }

    FILE* input = fopen("1_20.txt", "r");
    if (input == NULL) {
        printf("[ERROR]: Couldn't open the input file.\n");
        exit(1);
    }

    char chunk[128];
    Generator g;
    setupGenerator(&g, MC_VERSION, 0);

    BastionData bastion_data;

    while(fgets(chunk, sizeof(chunk), input) != NULL) {
        uint64_t seed = strtoull(chunk, NULL, 10);
        // uint64_t seed = strtoull(chunk, NULL, 10);

        // if (check_end_city(seed, 128, &city_pos)) {
            // if (check_end_city_chest_number(seed, city_pos.x, city_pos.z) >= 10) {
                // printf("%lld\n", seed);
            // }
        // }
        // if (filter_elder_guardian(seed, 3)) {
        //     printf("seed: %lld\n", seed);
        // }

        // if (check_end_city(seed, 256, &city_pos)) {
        //     if (check_end_city_for_trim(seed, city_pos.x, city_pos.z)) {
        //         printf("%lld\n", seed);
        //         fprintf(output, "%lld\n", seed);
        //     }
        // }

        if (check_nether(seed, g, &bastion_data)) {
            if (check_bastion_looting(seed, bastion_data.start_x, bastion_data.start_z, bastion_data.rotation)) {
                printf("Seed: %lld\n", seed);
                fprintf(output, "%lld\n", seed);
            }
        }
    
        // if (
        //     check_nether(structure_seed, g, &bastion_data) &&
        //     check_generation_attempt(structure_seed, 96, 96, &pyramid_pos, &village_pos)
        // ) {
        //     #pragma omp parallel num_threads(10)
        //     #pragma omp for
        //     for (uint64_t i = 0; i < UINT16_MAX; ++i) {
        //         uint64_t world_seed = (i << 48) | structure_seed;
        //         if (
        //             filter_pearls(world_seed, 100, 20) &&
        //             filter_shulker_shells(world_seed, 3, 4, 6) &&
        //             filter_elder_guardian(world_seed, 2) &&
        //             check_end_city(world_seed, 256, &city_pos) &&
        //             check_end_city_loot(world_seed, city_pos.x, city_pos.z) &&
        //             check_spawn(world_seed, &pyramid_pos, &village_pos, g) &&
        //             filter_skulls_rng_manipulation(world_seed, 8, 3)
        //         ) {
        //             printf("Seed: %lld\n", world_seed);
        //             fprintf(output, "%lld\n", world_seed);
        //         }
        //     }
        // }
    }
    
    fclose(output);
    fclose(input);

    return 0;
    // AA_structure_seeds.txt
}
/*
int main22222222() {
    #pragma omp parallel num_threads(8)
    {
        Pos city_pos;

        #pragma omp for
        for (uint64_t structure_seed = 0; structure_seed < (uint64_t)1 << 48; ++structure_seed) {
            if (
                check_nether(structure_seed) &&
                check_end_city(structure_seed, 400, &city_pos)
            ) {
                for (uint64_t i = 0; i < UINT16_MAX; ++i) {
                    uint64_t world_seed = (i << 48) | structure_seed;
                    if (
                        filter_pearls(world_seed, 110, 20) && 
                        filter_vaults(world_seed, 1, 0) &&
                        check_pyramid_village(world_seed, 208, 208) && 
                        filter_skulls_rng_manipulation(world_seed, 8, 3) &&
                        check_end_city_for_trim(world_seed, city_pos.x, city_pos.z)
                    ) {
                        printf("Seed: %lld\n", world_seed);
                    }
                }
            }
        }
    }
}

int main10() {
    int max_dist = 32;
    Pos chamber_pos;
    Pos spawn_pos;
    Generator g;
    setupGenerator(&g, MC_VERSION, 0);
    
    for (uint64_t seed = 0; seed < (uint64_t)2<<32; ++seed) {        
        applySeed(&g, DIM_OVERWORLD, seed);
        int got_spawn = 0;
        for (int r_x = -1; r_x <= 0; ++r_x) {
            for (int r_z = -1; r_z <= 0; ++r_z) {
                if (!getStructurePos(Trial_Chambers, MC_VERSION, g.seed, r_x, r_z, &chamber_pos)) {
                    continue;
                }
                
                if (isViableStructurePos(Trial_Chambers, &g, chamber_pos.x, chamber_pos.z, 0)) {
                    if (!got_spawn) {
                        spawn_pos = getSpawn(&g);   
                        got_spawn = 1;
                    }
                    
                    if (abs(chamber_pos.x - spawn_pos.x) <= max_dist && abs(chamber_pos.z - spawn_pos.z) <= max_dist) {
                        printf("Seed: %lld\n", seed);
                    }
                }
            }
        }
    }


    return 0;
}

int main222() {
    // LARGE_INTEGER start, finish, freq;
    // QueryPerformanceFrequency(&freq);
    // QueryPerformanceCounter(&start);

    // check_nether(structure_seed);
    // check_end_city_for_trim(structure_seed, city_pos.x, city_pos.z);
    // filter_vaults(structure_seed, 0);
    // filter_skulls(structure_seed, 10, 3, 3);
    // filter_pearls(structure_seed, 100, 20);
    // QueryPerformanceCounter(&finish);
    // printf("Czas: %f\n", ((finish.QuadPart - start.QuadPart) / (double)freq.QuadPart));

    // #pragma omp parallel num_threads(10)
    {
        // FILE* output;
        // output = fopen("output.txt", "a");
        // if (output == NULL) {
        //     printf("[ERROR]: Couldn't open the output file.\n");
        //     exit(1);
        // }

        Pos city_pos;

        // for (uint64_t structure_seed = 0; structure_seed < (uint64_t)1 << 48; ++structure_seed) {
        // for (uint64_t structure_seed = (uint64_t)1 << 48; structure_seed > 0; --structure_seed) {
        // #pragma omp for
        int x = 0;
        for (uint64_t structure_seed = 1; structure_seed < (uint64_t)1 << 48; ++structure_seed) {
            // uint64_t structure_seed = nextLong(&i) << 16 >> 16;
            
            if (check_pyramid_village(structure_seed, 256, 256)) {
                ++x;
                printf("~%f%%\n", (float)x/structure_seed*100.0);
            }

            // if (
            //     check_nether(structure_seed) &&
            //     check_end_city(structure_seed, 208, &city_pos)
            // ) {
            //     for (uint64_t i = 0; i < UINT16_MAX; ++i) {
            //         uint64_t world_seed = (i << 48) | structure_seed;
            //         if (
            //             filter_pearls(world_seed, 100, 20) && 
            //             filter_vaults(world_seed, 1, 0) &&
            //             filter_blaze_rods(world_seed, 10, 6) &&
            //             check_end_city_for_trim(world_seed, city_pos.x, city_pos.z) &&
            //             check_pyramid_village(world_seed, 128, 128) && 
            //             filter_skulls_rng_manipulation(world_seed, 10, 3)
            //             // filter_skulls(world_seed, 10, 3, 3)
            //         ) {
            //             printf("Seed: %lld\n", world_seed);
            //             // fprintf(output, "%lld\n", world_seed);
            //         }
            //     }
            // }
        }
        // fclose(output);
    }

    return 0;
}
*/