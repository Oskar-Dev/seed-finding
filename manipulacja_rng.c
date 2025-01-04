#include <stdio.h>
#include "MiLTSU.h"
#include <math.h>
#include <time.h>

#define SKULL "minecraft:entities/wither_skeleton"

void binprintf(int v) {
    unsigned int mask=1<<((sizeof(uint16_t)<<3)-1);
    while(mask) {
        printf("%d", (v&mask ? 1 : 0));
        mask >>= 1;
    }
}

int next_has_skull(const int looting_level, Xoroshiro *xr) {
    // Throw out bones and coal random calls (looting, no matter the level, is just 1 extra call for both coal and bones)
    for (int i = 0; i < 2 + (looting_level > 0 ? 2 : 0); i++) {
        xNextInt(xr, 1);
    }

    // Actual skull rng
    return xNextFloat(xr) < 0.025 + (0.01 * looting_level);
}
// 0101111110000
// 0000111111010
int main() {
    // 

    // int skulls = 0;
    // int kills = 0;
    // int looting_level = 3;

    // for (int i = 0;; ++i) {
    //     skulls += next_has_skull(looting_level, &xr);
    //     ++kills;

    //     if (skulls == 3) break;
    // }

    // printf("3/%d\n", kills);
    // time_t seconds = time(NULL);
    // int succ = 0;

    // 0.988370
    // 0.458423
    // 0.916819
    // 0.973774
    // 0.041539
    // 0.014959

    const int max_kills = 9;
    // for (uint64_t seed = 1; seed < UINT64_MAX; ++seed) {
    uint64_t seed = -4788054439990235065
;
        for (uint16_t i = 0; i < pow(2, max_kills); ++i) {
            Xoroshiro xr = getRandomSequenceXoro(seed, SKULL);
            int skulls = 0;
            int kills = 0;

            // xSkipN(&xr, 2);
            // xSkipN(&xr, 2);

            for (int j = 0; j < max_kills; ++j) {
                int looting_level = (i >> j) & 1 == 1 ? 3 : 0;
                
                skulls += next_has_skull(looting_level, &xr);
                ++kills;

                if (skulls == 3 && kills <= max_kills) {
                    // ++succ;
                    // float p = (float)succ / seed * 100;
                    // time_t passed_time = time(NULL) - seconds;
                    // float seeds_per_second = succ / (float)passed_time;

                    // printf("Seed: %lld; Seeds/s: %f; Chance: %f%\n", seed, seeds_per_second, p);
                    // printf("Seed: %lld; Kills: %d; ID: ", seed, kills);
                    printf("Seed: %lld; Kills: %d; ID: ", seed, kills);
                    binprintf(i);
                    printf("\n");
                    goto next;
                }
            }
        // }
    next:
    }

    return 0;
}