#include <stdio.h>
#include <math.h>
#include "MiLTSU/bartering.c"
#include "MiLTSU/random_sequence.c"

int nextRods(int looting, Xoroshiro *xr) {
    int setCount = xNextInt(xr, 2);

    if (looting == 0) {
        return setCount;
    }
    return setCount + round(xNextFloat(xr) * looting);
}

int nextHasSkull(int lootingLevel, Xoroshiro *xr) {
    // Throw out bones and coal random calls (looting, no matter the level, is just 1 extra call for both coal and bones)
    for (int i = 0; i < 2 + (lootingLevel > 0 ? 2 : 0); i++) {
        xNextInt(xr, 1);
    }

    // Actual skull rng
    return xNextFloat(xr) < 0.025 + (0.01 * lootingLevel);
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
        xNextInt(xr, 1);
    } else if (n >= 9 && n < 11) {
        // Crossbow.
        // Roll enchant.
        xNextInt(xr, 1);
    } else if (n >= 11 && n < 13) {
        // Iron axe.
        // Roll enchantment.
        xNextInt(xr, 1);
    } else if (n >= 13 && n < 15) {
        // Iron chestplate.
        // Roll enchantment.
        xNextInt(xr, 1);
    } else if (n >= 15 && n < 17) {
        // Golden Carrot(s).
        // Roll count.
        xNextInt(xr, 1);
    } else if (n >= 17 && n < 19) {
        // Enchanted book (bad).
        // Roll enchantment.
        xNextInt(xr, 1);
        // Roll enchantment level.
        xNextInt(xr, 1);
    } else if (n >= 19 && n < 21) {
        // Enchanted book (good - riptide, channeling, mending).
        // Roll enchantment.
        xNextInt(xr, 1);
        // Roll enchantment level.
        xNextInt(xr, 1);
    } else if (n == 21) {
        // Diamond chestplate.
        // Roll enchantment.
        xNextInt(xr, 1);
    } else if (n == 22) {
        // Diamond axe.
        // Roll enchantment.
        xNextInt(xr, 1);
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
        xNextInt(xr, 1);
    } else if (n >= 13 && n < 16) {
        // Golden apple.
    } else if (n >= 16 && n < 19) {
        // Diamond axe with a random enchantment.
        xNextInt(xr, 1);
    } else if (n >= 19 && n < 22) {
        // Diamond chestplate with a random enchantment.
        xNextInt(xr, 1);
        // xNextInt(xr, 1);
        // xNextInt(xr, 1);
        // xNextInt(xr, 1);
        // xNextInt(xr, 1);
    } else if (n >= 22 && n < 24) {
        // Random enchanted book.
        xNextInt(xr, 2);
        // Random enchantment level. (Maybe don't do this if it is multishot?)
        xNextInt(xr, 2);
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

int main() {
//     char* str = "minecraft:gameplay/piglin_bartering";
//     uint64_t seed = 0;
//     int pearls;
//     for (;; ++seed) {
//         Xoroshiro xoro = getRandomSequenceXoro(seed, str);
        
//         for (int i = 0; i < 5; ++i) {
//             pearls = nextPearls(&xoro);
//             if (pearls == 0) {
//                 break;
//             }
            
//             if (pearls > 0 && i == 5 - 1) { 
//                 goto end;
//             }
//         } 
//     }

// end:
//     printf("seed: %lld; count: %d\n", seed, pearls);

//     char* str = "minecraft:chests/trial_chambers/reward_ominous";
//     uint64_t seed = 0;
//     int x;
//     for (;; ++seed) {
//         Xoroshiro xoro = getRandomSequenceXoro(seed, str);
        
//         x = nextHasHeavycore(&xoro);
//         if (x == 1) {
//             printf("seed: %lld;\n", seed);
//             // goto end;
//         }
//     }

// end:
//     printf("seed: %lld;\n", seed);

    // Heavy core, 5 emeralds, 2 diamonds, 7 emeralds, Dichte I Book.
    // Unique, common x3, rare.

    // char* str1 = "minecraft:chests/trial_chambers/reward_ominous";
    // uint64_t seed1 = -3557589012648597516;
    // uint64_t seed2 = 12;
    // uint64_t seed3 = 46;
    // Xoroshiro xoro1 = getRandomSequenceXoro(seed1, str1);
    // Xoroshiro xoro2 = getRandomSequenceXoro(seed2, str1);
    // Xoroshiro xoro3 = getRandomSequenceXoro(seed3, str1);
    // for (int i = 0; i < 20; ++i) {
    //     int x = next_has_heavy_core(&xoro1);
    //     int y = next_has_heavy_core(&xoro2);
    //     int z = next_has_heavy_core(&xoro3);
        
    //     printf("%d, %d, %d\n", x, y, z);
    // }

    char* reward_path = "minecraft:chests/trial_chambers/reward";
    char* ominous_reward_path = "minecraft:chests/trial_chambers/reward_ominous";
    uint64_t seed = 0;
    int x;
    for (;; ++seed) {
        Xoroshiro xoro_reward = getRandomSequenceXoro(seed, reward_path);
        Xoroshiro xoro_ominous_reward = getRandomSequenceXoro(seed, ominous_reward_path);
        
        if (next_ominous_vault_unique(&xoro_ominous_reward) == 9 && next_vault_unique(&xoro_reward) == 11) {
            printf("Seed: %lld\n", seed);
        }
    }

// end:
//     printf("seed: %lld;\n", seed);

    return 0;
}