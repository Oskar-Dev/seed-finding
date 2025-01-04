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

int get_available_enchantments(int level, int* list) {
    int slot = 0;

    // Sharpness, list[0,1]
    if (level >= 1 && level <= 65) {
        if (level >= 45) {
            list[slot+1] = 5;
        } else if (level >= 34) {
            list[slot+1] = 4;
        } else if (level >= 23) {
            list[slot+1] = 3;
        } else if (level >= 12) {
            list[slot+1] = 2;
        } else if (level >= 1) {
            list[slot+1] = 1;
        }
        list[slot] = 0;
        slot += 2;
    }

    // Smite, list[2,3]
    if (level >= 5 && level <= 57) {
        if (level >= 37) {
            list[slot+1] = 5;
        } else if (level >= 29) {
            list[slot+1] = 4;
        } else if (level >= 21) {
            list[slot+1] = 3;
        } else if (level >= 13) {
            list[slot+1] = 2;
        } else if (level >= 5) {
            list[slot+1] = 1;
        }
        list[slot] = 1;
        slot += 2;
    }

    // Bane of Arthropods, list[4,5]
    if (level >= 5 && level <= 57) {
        if (level >= 37) {
            list[slot+1] = 5;
        } else if (level >= 29) {
            list[slot+1] = 4;
        } else if (level >= 21) {
            list[slot+1] = 3;
        } else if (level >= 13) {
            list[slot+1] = 2;
        } else if (level >= 5) {
            list[slot+1] = 1;
        }
        list[slot] = 2;
        slot += 2;
    }

    // Knockback, list[6,7]
    if (level >= 5 && level <= 75) {
        if (level >= 25) {
            list[slot+1] = 2;
        } else if (level >= 5) {
            list[slot+1] = 1;
        }
        list[slot] = 3;
        slot += 2;
    }

    // Fire Aspect, list[8,9]
    if (level >= 10 && level <= 80) {
        if (level >= 30) {
            list[slot+1] = 2;
        } else if (level >= 10) {
            list[slot+1] = 1;
        }
        list[slot] = 4;
        slot += 2;
    }

    // Looting, list[10,11]
    if (level >= 15 && level <= 83) {
        if (level >= 33) {
            list[slot+1] = 3;
        } else if (level >= 24) {
            list[slot+1] = 2;
        } else if (level >= 15) {
            list[slot+1] = 1;
        }
        list[slot] = 5; // !!!!!
        slot += 2;
    }

    // Sweeping Edge, list[12,13]
    if (level >= 5 && level <= 38) {
        if (level >= 23) {
            list[slot+1] = 3;
        } else if (level >= 14) {
            list[slot+1] = 2;
        } else if (level >= 5) {
            list[slot+1] = 1;
        }
        list[slot] = 6;
        slot += 2;
    }

    // Unbreaking, list[14,15]
    if (level >= 5 && level <= 71) {
        if (level >= 21) {
            list[slot+1] = 3;
        } else if (level >= 13) {
            list[slot+1] = 2;
        } else if (level >= 5) {
            list[slot+1] = 1;
        }
        list[slot] = 7;
        slot += 2;
    }

    // Mending, list[16,17]
    if (level >= 25 && level <= 75) {
        list[slot+1] = 1;
        list[slot] = 8;
        slot += 2;
    }

    // Curse of Vanishing, list[18,19]
    if (level >= 25 && level <= 50) {
        list[slot+1] = 1;
        list[slot] = 9;
        slot += 2;
    }

    return slot / 2;
}

int get_random_enchantment(uint64_t* rand, int* list, int length) {
    int total_weight = 0;
    for (int i = 0; i < length; ++i) {
        switch (list[2*i]) {
        case 0: {
            total_weight += 10;
            break;
        }
        case 1: {
            total_weight += 5;
            break;
        }
        case 2: {
            total_weight += 5;
            break;
        }
        case 3: {
            total_weight += 5;
            break;
        }
        case 4: {
            total_weight += 2;
            break;
        }
        case 5: {
            total_weight += 2;
            break;
        }
        case 6: {
            total_weight += 2;
            break;
        }
        case 7: {
            total_weight += 5;
            break;
        }
        case 8: {
            total_weight += 1;
            break;
        }
        case 9: {
            total_weight += 2;
            break;
        }
        
        default:
            break;
        }
    }
    
    int roll = nextInt(rand, total_weight);

    for (int i = 0; i < length; ++i) {
        switch (list[2*i]) {
            case 0: {
                roll -= 10;
                break;
            }
            case 1: {
                roll -= 5;
                break;
            }
            case 2: {
                roll -= 5;
                break;
            }
            case 3: {
                roll -= 5;
                break;
            }
            case 4: {
                roll -= 2;
                break;
            }
            case 5: {
                roll -= 2;
                break;
            }
            case 6: {
                roll -= 2;
                break;
            }
            case 7: {
                roll -= 5;
                break;
            }
            case 8: {
                roll -= 1;
                break;
            }
            case 9: {
                roll -= 2;
                break;
            }
            
            default:
                break;
        }

        if (roll < 0) {
            return i;
        }
    }

}

void remove_from_list(int to_remove, int* list, int length) {
    for (int i = 0; i < length; ++i) {
        if (list[i*2] == to_remove) {
            list[i*2] = -1;
            list[i*2+1] = -1;
        }
    }
}

// "sharpness", "smite", "bane_of_arthropods", "knockback", "fire_aspect", "looting", "sweeping_edge", "unbreaking", "vanishing_curse", "mending",
int enchant_with_levels_looting_check(uint64_t* rand, int level, const int ench_val) {
    int has_looting_3 = 0;
    int available_enchantments[20] = { 0 };

    level += 1 + nextInt(rand, ench_val / 4 + 1) + nextInt(rand, ench_val / 4 + 1);
    float amplifier = (nextFloat(rand) + nextFloat(rand) - 1.0f) * 0.15f;
    level = clamp((int)round((float)level + (float)level * amplifier), 1, INT_MAX);
    
    int length = get_available_enchantments(level, &available_enchantments[0]);
    if (length != 0) {

        int enchantment_index = get_random_enchantment(rand, &available_enchantments[0], length);
        if (available_enchantments[enchantment_index*2] == 5 && available_enchantments[enchantment_index*2 + 1] == 3) {
            has_looting_3 = 1;
        }
        remove_from_list(enchantment_index, &available_enchantments[0], length);
        if (enchantment_index == 0) { remove_from_list(1, &available_enchantments[0], length); remove_from_list(2, &available_enchantments[0], length); }
        if (enchantment_index == 1) { remove_from_list(0, &available_enchantments[0], length); remove_from_list(2, &available_enchantments[0], length); }
        if (enchantment_index == 2) { remove_from_list(0, &available_enchantments[0], length); remove_from_list(1, &available_enchantments[0], length); }

        while (nextInt(rand, 50) <= level) {

            int enchantment_index = get_random_enchantment(rand, &available_enchantments[0], length);
            if (available_enchantments[enchantment_index*2] == 5 && available_enchantments[enchantment_index*2 + 1] == 3) {
                has_looting_3 = 1;
            }  
            remove_from_list(enchantment_index, &available_enchantments[0], length);
            if (enchantment_index == 0) { remove_from_list(1, &available_enchantments[0], length); remove_from_list(2, &available_enchantments[0], length); }
            if (enchantment_index == 1) { remove_from_list(0, &available_enchantments[0], length); remove_from_list(2, &available_enchantments[0], length); }
            if (enchantment_index == 2) { remove_from_list(0, &available_enchantments[0], length); remove_from_list(1, &available_enchantments[0], length); }

            level /= 2;
        }
    }

    return has_looting_3;
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