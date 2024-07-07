#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "end_city.h"

int check_end_city(uint64_t seed, int max_distance) {
    SurfaceNoise noise;
    Generator g;
    setupGenerator(&g, MC_1_21, 0);

    applySeed(&g, DIM_END, seed);
    initSurfaceNoise(&noise, DIM_END, seed);

    Pos3 gateway_pos = linkedGateway(seed);
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
                        if (pieces[i].type == END_SHIP) 
                            return 1;
                    }
                }
            }
        }
    }
    
    return 0;
}