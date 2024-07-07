#include "end_city.c"
#include "cubiomes.h"
#include "MiLTSU.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

int clamp(const int v, const int mn, const int mx) {
    if (v < mn) return mn;
    if (v > mx) return mx;
    return v;
}

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
int filter_vaults(const uint64_t seed, const int check_trident) {
    Xoroshiro xoro_reward = getRandomSequenceXoro(seed, reward_path);
    Xoroshiro xoro_ominous_reward = getRandomSequenceXoro(seed, ominous_reward_path);

    if (check_trident) {
        return next_ominous_vault_unique(&xoro_ominous_reward) == 9 && next_vault_unique(&xoro_reward) == 11;
    } else {
        return next_ominous_vault_unique(&xoro_ominous_reward) == 9;
    }
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
    setupGenerator(&g, MC_1_21, 0);
    
    applySeed(&g, DIM_OVERWORLD, lower48);
    int got_spawn = 0;
    for (int r_x = -1; r_x <= 0; ++r_x) {
        for (int r_z = -1; r_z <= 0; ++r_z) {
            if (!getStructurePos(Desert_Pyramid, MC_1_21, g.seed, r_x, r_z, &pyramid_pos)) {
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

int check_nether(const uint64_t lower48) {
    const int max_bastion_dist = 160;
    const int max_fortress_dist = 160;
    const int max_structure_dist = 128;

    Pos bastion_pos;
    Pos fortress_pos;
    
    StructureVariant bastion_type;
    
    Generator g;
    setupGenerator(&g, MC_1_21, 0);
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

int main() {
    int buffer_len = 50;

    #pragma omp parallel num_threads(10)
    {
        char buffer[50];
        FILE* input;
        input = fopen("input_60mln.txt", "r");
        if (input == NULL) {
            printf("[ERROR]: Couldn't open the seeds file.\n");
            exit(1);
        }
        
        FILE* output;
        output = fopen("output.txt", "a");
        if (output == NULL) {
            printf("[ERROR]: Couldn't open the seeds file.\n");
            exit(1);
        }
        
        #pragma omp for
        for (int line_to_read = 0; line_to_read < 59729806; ++line_to_read) {
            int line = 0;
            while(fgets(buffer, buffer_len, input)) {
                if (line != line_to_read) {
                    ++line;
                    continue;
                }

                buffer[strcspn(buffer, "\n")] = 0;
                uint64_t structure_seed = strtoull(buffer, NULL, 10);

                if (!check_nether(structure_seed)) continue;
                
                for (uint64_t i = 0; i < 65536; ++i) {
                    uint64_t seed = (i << 48) | structure_seed;

                    if (
                        filter_vaults(seed, 0) && 
                        filter_blaze_rods(seed, 8, 6) && 
                        filter_pearls(seed, 90, 20) && 
                        filter_skulls(seed, 10, 3, 3) &&
                        check_pyramid(seed, 96)
                    ) {
                        printf("Seed: %lld\n", seed);
                        fprintf(output, "%lld\n", seed);
                    }
                }
            }
        }
        fclose(input);
        fclose(output);
    }

    // char buffer[50];
    // FILE* input;
    // input = fopen("input_new.txt", "r");
    // if (input == NULL) {
    //     printf("[ERROR]: Couldn't open the seeds file.\n");
    //     exit(1);
    // }
    
    // FILE* output;
    // output = fopen("output.txt", "a");
    // if (output == NULL) {
    //     printf("[ERROR]: Couldn't open the seeds file.\n");
    //     exit(1);
    // }
    
    // while(fgets(buffer, buffer_len, input)) {
    //     buffer[strcspn(buffer, "\n")] = 0;
    //     uint64_t structure_seed = strtoull(buffer, NULL, 10); 
        
    //     #pragma omp parallel num_threads(12)
    //     #pragma omp for
    //     for (uint64_t i = 0; i < 65536; ++i) {
    //         uint64_t seed = (i << 48) | structure_seed;
            
    //         if (
    //             filter_pearls(seed, 100, 20) == 1 && 
    //             filter_vaults(seed) == 1 && 
    //             filter_skulls(seed, 10, 3, 3) == 1 &&
    //             filter_blaze_rods(seed, 9, 6)
    //         ) {
    //             printf("Seed: %lld\n", seed);
    //             fprintf(output, "%lld\n", seed);
    //         }
    //     }
    // }

    // fclose(input);
    // fclose(output);
    
    // #pragma omp parallel num_threads(8)
    // #pragma omp for
    // for (uint64_t structure_seed = 0; structure_seed < (uint64_t)1 << 47; ++structure_seed) {
    //     if (
    //         check_nether(structure_seed) && 
    //         check_end_city(structure_seed, 112)
    //     ) {
    //         for (uint64_t i = 0; i < UINT16_MAX; ++i) {
    //             uint64_t world_seed = (i << 48) | structure_seed;
    //             if (
    //                 filter_pearls(world_seed, 100, 20) && 
    //                 filter_skulls(world_seed, 10, 3, 3) &&
    //                 filter_vaults(world_seed) &&
    //                 check_pyramid(world_seed, 112)
    //             ) {
    //                 printf("Seed: %lld\n", world_seed);
    //             }
    //         }
    //     }
    // }

    return 0;
}