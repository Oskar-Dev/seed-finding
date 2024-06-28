#include <stdio.h>
#include <math.h>
#include "MiLTSU/bartering.c"
#include "MiLTSU/random_sequence.c"
#include "MiLTSU/md5.c"

int next_has_torchflower_seed(Xoroshiro *xr) {
    return !xNextInt(xr, 2);
}

int next_has_rod(Xoroshiro *xr) {
    return xNextInt(xr, 2);
}

int next_has_skull(int looting_level, Xoroshiro *xr) {
    // Throw out bones and coal random calls (looting, no matter the level, is just 1 extra call for both coal and bones)
    for (int i = 0; i < 2 + (looting_level > 0 ? 2 : 0); i++) {
        xNextInt(xr, 1);
    }

    // Actual skull rng
    return xNextFloat(xr) < 0.025 + (0.01 * looting_level);
}

void enchantment_fix(Xoroshiro *xr, int level, int ench_val) {

    // DOesn't work, lol.
    level += xNextInt(xr, ench_val / 4 + 1) + xNextInt(xr, ench_val / 4 + 1);
    xNextFloat(xr);
    xNextFloat(xr);
    while (xNextInt(xr, 50) <= level) {
        level = level * 4 / 5 + 1;
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

int next_vault_rare(Xoroshiro *xr) {
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
        enchantment_fix(xr, xNextInt(xr, 11) + 5, 1);
        return 0;
    } else if (n >= 9 && n < 11) {
        // Crossbow.
        // Roll enchant.
        xNextInt(xr, 1);
    } else if (n >= 11 && n < 13) {
        // Iron axe.
        // Roll enchantment.
        enchantment_fix(xr, xNextInt(xr, 11), 14);
        return 0;
    } else if (n >= 13 && n < 15) {
        // Iron chestplate.
        // Roll enchantment.
        enchantment_fix(xr, xNextInt(xr, 11), 9);
        return 0;
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
        enchantment_fix(xr, xNextInt(xr, 11) + 5, 10);
        return 0;
    } else if (n == 22) {
        // Diamond axe.
        // Roll enchantment.
        enchantment_fix(xr, xNextInt(xr, 11) + 5, 10);
        return 0;
    }
    return 1;
}

int next_vault_unique(Xoroshiro *xr) {
    // Choose a common or a rare roll.
    if (xNextInt(xr, 10) < 8) {
        // Rare.
        if (!next_vault_rare(xr)) return -1;
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

int next_ominous_vault_rare(Xoroshiro *xr) {
    int n = xNextInt(xr, 29);
    if (n < 5) {
        // Emerald block.
    } else if (n >= 5 && n < 9) {
        // Iron block.
    } else if (n >= 9 && n < 13) {
        // Crossbow with a random enchantment.
        xNextInt(xr, 1);
        // enchantment_fix(xr, xNextInt(xr, 16) + 5, 10);
        return 0;
    } else if (n >= 13 && n < 16) {
        // Golden apple.
    } else if (n >= 16 && n < 19) {
        // Diamond axe with a random enchantment.
        enchantment_fix(xr, xNextInt(xr, 11) + 10, 10);
        return 0;
    } else if (n >= 19 && n < 22) {
        // Diamond chestplate with a random enchantment.
        enchantment_fix(xr, xNextInt(xr, 11) + 10, 10);
        return 0;
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
    return 1;
}

int next_ominous_vault_unique(Xoroshiro *xr) {
    // Choose a common or a rare roll.
    if (xNextInt(xr, 10) < 8) {
        // Rare.
        if (!next_ominous_vault_rare(xr)) return -1;
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

int filter_vaults(uint64_t seed) {
    char* reward_path = "minecraft:chests/trial_chambers/reward";
    char* ominous_reward_path = "minecraft:chests/trial_chambers/reward_ominous";

    Xoroshiro xoro_reward = getRandomSequenceXoro(seed, reward_path);
    Xoroshiro xoro_ominous_reward = getRandomSequenceXoro(seed, ominous_reward_path);

    return next_ominous_vault_unique(&xoro_ominous_reward) == 9 && next_vault_unique(&xoro_reward) == 11;
}

int filter_pearls(uint64_t seed, int gold, int pearls) {
    if (pearls <= 0 || gold <= 0) return 0;

    char* str = "minecraft:gameplay/piglin_bartering";
    Xoroshiro xoro = getRandomSequenceXoro(seed, str);
    
    int rolled_pearls = 0;  
    for (int i = 0; i < gold; ++i) {
        rolled_pearls += nextPearls(&xoro);
    }

    return rolled_pearls >= pearls;
}

int filter_skulls(uint64_t seed, int kills, int skulls, int looting_level) {
    if (kills < 0 || skulls <= 0 || kills < skulls || looting_level < 0 || looting_level > 3) return 0;

    char* str = "minecraft:entities/wither_skeleton";
    Xoroshiro xoro = getRandomSequenceXoro(seed, str);

    int rolled_skulls = 0;
    for (int i = 0; i < kills; ++i) {
        rolled_skulls += next_has_skull(looting_level, &xoro);
    }

    return rolled_skulls >= skulls;
}

int filter_blaze_rods(uint64_t seed, int kills, int rods) {
    if (kills < 0 || rods <= 0 || kills < rods) return 0;

    char* str = "minecraft:entities/blaze";
    Xoroshiro xoro = getRandomSequenceXoro(seed, str);

    int rolled_rods = 0;
    for (int i = 0; i < kills; ++i) {
        rolled_rods += next_has_rod(&xoro);
    }

    return rolled_rods >= rods;
}

int filter_torchflower_seeds(uint64_t seed, int sniffs, int seeds) {
    char* str = "minecraft:gameplay/sniffer_digging";
    Xoroshiro xoro = getRandomSequenceXoro(seed, str);

    int rolled_seeds = 0;
    for (int i = 0; i < sniffs; ++i) {
        rolled_seeds += next_has_torchflower_seed(&xoro);
    }
    return rolled_seeds >= seeds;
}

// Seed: 738925
// Seed: 1411341
// Seed: 3405489
// Seed: 3747813
// Seed: 3995650
// Seed: 4292664
// Seed: 4355679
// Seed: 4560515
// Seed: 4757130
// Seed: 5799821

int main() {
    uint64_t seed = 0;
    for (;; ++seed) {
        if (filter_blaze_rods(seed, 8, 6) && filter_pearls(seed, 80, 20) && filter_skulls(seed, 10, 3, 3) && filter_vaults(seed))
            printf("Seed: %lld\n", seed);
    }

    return 0;
}