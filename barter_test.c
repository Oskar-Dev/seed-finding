#include <stdio.h>
#include <stdlib.h>
#include "MiLTSU.h"

    // {0, 0, 0, 5, false, true},     // 0:  minecraft:book
    // {1, 0, 0, 8, false, true},     // 1:  minecraft:iron_boots
    // {2, 0, 0, 8, false, false},    // 2:  minecraft:potion
    // {3, 0, 0, 8, false, false},    // 3:  minecraft:splash_potion
    // {4, 0, 0, 10, false, false},   // 4:  minecraft:potion
    // {5, 10, 36, 10, true, false},  // 5:  minecraft:iron_nugget
    // {6, 2, 4, 10, true, false},    // 6:  minecraft:ender_pearl
    // {7, 3, 9, 20, true, false},    // 7:  minecraft:string
    // {8, 5, 12, 20, true, false},   // 8:  minecraft:quartz
    // {9, 1, 1, 40, false, false},   // 9:  minecraft:obsidian
    // {10, 1, 3, 40, true, false},   // 10: minecraft:crying_obsidian
    // {11, 1, 1, 40, false, false},  // 11: minecraft:fire_charge
    // {12, 2, 4, 40, true, false},   // 12: minecraft:leather
    // {13, 2, 8, 40, true, false},   // 13: minecraft:soul_sand
    // {14, 2, 8, 40, true, false},   // 14: minecraft:nether_brick
    // {15, 6, 12, 40, true, false},  // 15: minecraft:spectral_arrow
    // {16, 8, 16, 40, true, false},  // 16: minecraft:gravel
    // {17, 8, 16, 40, true, false}; // 17: minecraft:blackstone

void print_loot(int n, BarteringOut* loot) {
    switch (loot->id) {
        case 0: { printf("%d: book (%d)\n", n, loot->amount); } break;
        case 1: { printf("%d: iron_boots (%d)\n", n, loot->amount); } break;
        case 2: { printf("%d: potion (%d)\n", n, loot->amount); } break;
        case 3: { printf("%d: splash_potion (%d)\n", n, loot->amount); } break;
        case 4: { printf("%d: potion (%d)\n", n, loot->amount); } break;
        case 5: { printf("%d: iron_nugget (%d)\n", n, loot->amount); } break;
        case 6: { printf("%d: *ENDER PEARL* (%d)\n", n, loot->amount); } break;
        case 7: { printf("%d: string (%d)\n", n, loot->amount); } break;
        case 8: { printf("%d: quartz (%d)\n", n, loot->amount); } break;
        case 9: { printf("%d: obsidian (%d)\n", n, loot->amount); } break;
        case 10: { printf("%d: crying_obsidian (%d)\n", n, loot->amount); } break;
        case 11: { printf("%d: fire_charge (%d)\n", n, loot->amount); } break;
        case 12: { printf("%d: leather (%d)\n", n, loot->amount); } break;
        case 13: { printf("%d: soul_sand (%d)\n", n, loot->amount); } break;
        case 14: { printf("%d: nether_brick (%d)\n", n, loot->amount); } break;
        case 15: { printf("%d: spectral_arrow (%d)\n", n, loot->amount); } break;
        case 16: { printf("%d: gravel (%d)\n", n, loot->amount); } break;
        case 17: { printf("%d: blackstone (%d)\n", n, loot->amount); } break;
    
        default:
            break;
    }
}

char* bartering_str = "minecraft:gameplay/piglin_bartering";
int main() {
    Xoroshiro xr = getRandomSequenceXoro(-1850838705525738103, bartering_str);
    int n = 200;
    
    for (int i = 1; i <= n; ++i) {
        BarteringOut loot = nextBarteringLoot(&xr);
        print_loot(i, &loot);
    }

    return 0;
}