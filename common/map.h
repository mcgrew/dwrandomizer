

#ifndef _MAP_H_
#define _MAP_H_

#include <stdint.h>

/** Map tile indexes */
typedef enum {
    TILE_NOTSWAMP = -2,
    TILE_NONE     = -1,
    TILE_GRASS    = 0x0,
    TILE_DESERT   = 0x1,
    TILE_HILL     = 0x2,
    TILE_MOUNTAIN = 0x3,
    TILE_WATER    = 0x4,
    TILE_BLOCK    = 0x5,
    TILE_TREES    = 0x6,
    TILE_SWAMP    = 0x7,
    TILE_TOWN     = 0x8,
    TILE_CAVE     = 0x9,
    TILE_CASTLE   = 0xa,
    TILE_BRIDGE   = 0xb,
    TILE_STAIRS   = 0xc
} dw_tile;

/** Town border tile indexes */
typedef enum {
    BORDER_GRASS  = 0x0,
    BORDER_DESERT = 0x1,
    BORDER_HILL   = 0x2,
    BORDER_BRICK  = 0x4,
    BORDER_SWAMP  = 0x6,
    BORDER_TREES  = 0xb,
    BORDER_WATER  = 0xf,
} dw_border_tile;

/** Warp indices */
typedef enum {
    WARP_GARINHAM,
    WARP_STAFF_SHRINE,
    WARP_KOL,
    WARP_BRECCONARY,
    WARP_TANTEGEL,
    WARP_SWAMP_NORTH,
    WARP_CHARLOCK,
    WARP_SWAMP_SOUTH,
    WARP_MOUNTAIN_CAVE,
    WARP_RIMULDAR,
    WARP_HAUKSNESS,
    WARP_CANTLIN, 
    WARP_JERK_CAVE,
    WARP_ERDRICKS_CAVE,
    WARP_CHARLOCK_SURFACE_1,
    WARP_CHARLOCK_SURFACE_2,
    WARP_CHARLOCK_SURFACE_3,
    WARP_TANTEGEL_BASEMENT,
    WARP_GARINS_GRAVE = 19,
    WARP_CHARLOCK_CHEST = 21,
    WARP_CHARLOCK_POINTLESS_1 = 24,
    WARP_CHARLOCK_POINTLESS_2,
    WARP_MOUNTAIN_CAVE_POINTLESS = 39,
    WARP_GARINS_GRAVE_POINTLESS = 45
} dw_warp_index;

#include "dwr_types.h"
#include "dwr.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Decodes the in-game map into a 2 dimentional array of tiles
 *
 * @param map The map struct
 */
void map_decode(dw_map *map);

/**
 * Generates a new in-game map.
 *
 * @param rom The rom struct
 * @return A boolean indicating whether terrain generation was successful or not
 */
void map_generate_terrain(dw_rom *rom);

#ifdef __cplusplus
}
#endif
#endif


