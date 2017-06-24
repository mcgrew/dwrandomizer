
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "mt64.h"
#include "map.h"

#define MAX_BLOB 2400
#define MAP_ENCODED_SIZE 2534
#define VIABLE_CONT 250 /* minimum size for a continent to use */

static bool map_encode(dw_rom *rom)
{
    int x, y;
    uint8_t *e, encoded[120 * 120]; 
    uint16_t pointers[121];

    memset(encoded, 0xff, 120 * 120);
    e = encoded;

    pointers[0] = 0x9d5d;
    for (y=0; y < 120; y++) {
        for (x=0; x < 120; x++) {
            if (*e == 0xff) {
                *e = rom->map.tiles[x][y] << 4;
            } else if (*e >> 4 == rom->map.tiles[x][y]) {
                (*e)++;
            } else {
                e++;
                *e = rom->map.tiles[x][y] << 4;
            }
            if ((*e & 0xf) == 0xf) {
                e++;
            }
        }
        e++;
        pointers[y+1] = e - encoded + 0x9d5d;
    }

//    for (x=0; x < MAP_ENCODED_SIZE; x++)
//        printf("%02x ", encoded[x]);
//    printf("\n");

    /* need to optimize the map here */
//    for (y=0; y < 120; y++) {
//        for (x=0; x < 120; x++) {
//            printf("%d ", rom->map.tiles[x][y]);
//        }
//        printf("\n");
//    }
//    printf("\n");

    if (e - encoded > MAP_ENCODED_SIZE) {
        printf("Compressed map is too large (%d)\n", e - encoded);
        return false;
    } else {
        printf("Compressed map size: %d\n", e - encoded);
    }
    memcpy(rom->map.encoded, encoded, MAP_ENCODED_SIZE);
    memcpy(rom->map.pointers, pointers, 240);
    return true;
}

void map_decode(dw_rom *rom)
{
}

static void map_find_land(dw_map *map, int one, int two, uint8_t *x, uint8_t *y)
{
    int i;
    uint8_t cont;

    for (;;) {
        *x = mt_rand(0, 120);
        *y = mt_rand(0, 120);
        cont = map->walkable[*x][*y];
        if (cont && (cont == one || cont == two))
            return;
    }
}

static void place(dw_map *map, dw_warp_index warp_idx, dw_tile tile, 
        int lm_one, int lm_two)
{
    uint8_t x, y;
    dw_warp *warp;
    dw_tile old_tile;

    warp = &map->warps_from[warp_idx];

    do {
        map_find_land(map, lm_one, lm_two, &x, &y);
        old_tile = map->tiles[x][y];
    } while (old_tile == TILE_CASTLE || old_tile == TILE_TOWN ||
             old_tile == TILE_CAVE);

    warp->map = 1;
    warp->x = x;
    warp->y = y;
    map->tiles[x][y] = tile;
//    printf("Placed %d at %d,%d\n", (int)warp_idx, x, y);
}

static bool place_landmarks(dw_map *map, int *cont_sizes, int cont_count)
{
    int i, largest = 0, next = 0;
    dw_warp *warp;
    dw_warp_index caves[6] = {
            WARP_NORTHERN_SHRINE,
            WARP_MOUNTAIN_CAVE,
            WARP_SOUTHERN_SHRINE,
            WARP_ERDRICKS_CAVE,
            WARP_TANTEGEL_BASEMENT,
            WARP_GARINS_GRAVE
    };

    /* find the 2 largest land masses */
    for(i=0; i < cont_count; i++) {
        if (cont_sizes[i] > cont_sizes[largest-1]) {
            next = largest;
            largest = i + 1;
        } else if (cont_sizes[i] > cont_sizes[next-1]) {
            next = i + 1;
        }
    }

    if (cont_sizes[next] < 400) {
        next = 0;
    }

    place(map, WARP_SWAMP_SOUTH, TILE_CAVE, next ? next : largest, next);
    place(map, WARP_SWAMP_NORTH, TILE_CAVE, largest, 0);
    place(map, WARP_NORTHERN_SHRINE, TILE_CAVE, largest, next);
    place(map, WARP_MOUNTAIN_CAVE, TILE_CAVE, largest, next);
    place(map, WARP_SOUTHERN_SHRINE, TILE_CAVE, largest, next);
    place(map, WARP_ERDRICKS_CAVE, TILE_CAVE, largest, next);
    place(map, WARP_TANTEGEL_BASEMENT, TILE_CAVE, largest, next);
    place(map, WARP_GARINS_GRAVE, TILE_CAVE, largest, next);

    place(map, WARP_GARINHAM, TILE_TOWN, largest, next);
    place(map, WARP_KOL, TILE_TOWN, largest, next);
    place(map, WARP_BRECCONARY, TILE_TOWN, largest, next);
    place(map, WARP_RIMULDAR, TILE_TOWN, largest, next);
    place(map, WARP_HAUKSNESS, TILE_TOWN, largest, next);
    place(map, WARP_CANTLIN, TILE_TOWN, largest, next);

    place(map, WARP_TANTEGEL, TILE_CASTLE, largest, next);
    place(map, WARP_CHARLOCK, TILE_CASTLE, largest, next);
}

static int map_fill_point(dw_rom *rom, uint8_t *points, 
        uint8_t x, uint8_t y, dw_tile tile)
{
    uint8_t *p = points;
    if (x >= 120 || y >= 120)
        return 0;

    for (;;) {
        if (p[0] > 120) { break; }
//        if (p[0] == x && p[1] == y) return 0;
        p += 2;
    }
    p[0] = x;
    p[1] = y;
    rom->map.tiles[y][x] = (uint8_t)tile;
    if (tile == TILE_MOUNTAIN)
        return 2; /* not so many mountain tiles */
    return 1;
}

static void map_fill(dw_rom *rom, dw_tile tile, int size)
{
    uint8_t x, y, *points, *p;
    uint64_t directions;

    p = points = malloc(MAX_BLOB * 4 + 1);
    memset(p, 0xff, MAX_BLOB * 4 + 1);
    p[0] = mt_rand(0, 120);
    p[1] = mt_rand(0, 120);
    size = mt_rand(MAX_BLOB/4, MAX_BLOB);

    while (size > 0) {
        x = p[0]; y=p[1];
        directions = mt_rand(0, 16);
        if (directions & 8) {
            size -= map_fill_point(rom, points, x-1, y, tile);
        }
        if (directions & 4) {
            size -= map_fill_point(rom, points, x+1, y, tile);
        }
        if (directions & 2) {
            size -= map_fill_point(rom, points, x, y-1, tile);
        }
        if (directions & 1) {
            size -= map_fill_point(rom, points, x, y+1, tile);
        }
        if (p[2] < 120) {
            p += 2;
        } else break; 
    }
    free(points);

}

/*
 * Smooths the map to clean it up and ensure better compression.
 *
 * Params:
 *  rom : dw_rom*
 *      The rom pointer.
 */
static void map_smooth(dw_rom *rom)
{
    uint8_t x, y;

    /* If a single different tile is between 2 of the same tile, convert it.
       This will help the map compress (and look) better. */
    for (x=1; x < 119; x++) {
        for (y=0; y < 120; y++) {
            if (rom->map.tiles[x-1][y] == rom->map.tiles[x+1][y]) {
                rom->map.tiles[x][y] = rom->map.tiles[x-1][y];
            }
        }
    }
}

static bool tile_is_walkable(dw_tile tile)
{
    switch(tile) {
        case TILE_WATER:
        case TILE_MOUNTAIN:
        case TILE_BLOCK:
            return false;
        default:
            return true;
    }
}

/*
 * Finds the size of a continent given a point on it. Also fills in the 
 * walkable area array.
 *
 * Params:
 *  rom : dw_rom*
 *      The rom pointer.
 *  x : uint8_t
 *      The x coordinate of the tile to start mapping.
 *  y : uint8_t
 *      The y coordinate of the tile to start mapping.
 *  cont_index : int
 *      The index for this continent.
 */
static int map_continent(dw_map *map, uint8_t x, uint8_t y,
        uint8_t cont_index)
{
    int size = 1;

    if (x >= 120 || y >= 120)
        return 0;

    if (map->walkable[x][y])
        return 0;

    if (!tile_is_walkable(map->tiles[x][y]))
        return 0;

    map->walkable[x][y] = cont_index;
    size += map_continent(map, x-1, y, cont_index);
    size += map_continent(map, x+1, y, cont_index);
    size += map_continent(map, x, y-1, cont_index);
    size += map_continent(map, x, y+1, cont_index);

    return size;
}

static bool inline needs_bridge(uint8_t left, uint8_t right, int *cont_sizes)
{
    if (!left || !right || left == right)
        return false;
    if (cont_sizes[left-1] < 100 || cont_sizes[right-1] < 100)
        return false;
    return true;
}

static bool add_bridges(dw_map *map, int *cont_sizes)
{
    /* this could use more work */
    uint8_t x, y, left, right;
    bool added = false;

    for (y=0; y < 120; y++) {
        for (x=2; x < 118; x++) {
            if (map->tiles[x][y] == TILE_WATER) {
                left = map->walkable[x-1][y];
                right = map->walkable[x+1][y];
                if (needs_bridge(left, right, cont_sizes)){
                    map->tiles[x][y] = TILE_BRIDGE;
                    added = true;
                }
            }
        }
    }
    return added; 
}

static inline int find_walkable_area(dw_map *map, int *cont_sizes)
{
    int x, y, size, largest = 0;
    uint8_t continent = 0;

    memset(map->walkable, 0, 120 * 120);
    for (y=0; y < 120; y++) {
        for (x=0; x < 120; x++) {
            if (!map->walkable[x][y] && tile_is_walkable(map->tiles[x][y])) {
                cont_sizes[continent] = 
                        map_continent(map, x, y, continent+1);
                continent++;
            }
        }
    }
//    for (y=0; y < 120; y++) {
//        for (x=0; x < 120; x++) {
//            if (map->walkable[x][y])
//                printf("%x ", map->walkable[x][y]);
//            else
//                printf("  ");
//        }
//        printf("\n");
//    }
//    printf("\n");
//    for (x=0; x < continent; x++) {
//        printf("%d ", cont_sizes[x]);
//    }
//    printf("\n");
    return (int)continent;
}

bool map_generate_terrain(dw_rom *rom)
{
    int i, j, size, continents, continent_sizes[256], viable_continents, *v;

    dw_tile tiles[16] = {
            TILE_GRASS,  TILE_GRASS,  TILE_GRASS, TILE_SWAMP,
            TILE_DESERT, TILE_DESERT, TILE_HILL,  TILE_MOUNTAIN,
            TILE_TREES,  TILE_TREES,  TILE_TREES,  TILE_WATER,
            TILE_WATER,  TILE_WATER,  TILE_WATER, TILE_SWAMP
    };

    /* first fill the map with water */
    memset(rom->map.tiles, TILE_WATER, 120 * 120);

    /* now fill it in with some other tiles */
    for (i=0; i < 57600 / MAX_BLOB; i++) {
        mt_shuffle(tiles, sizeof(tiles) / sizeof(dw_tile), sizeof(dw_tile));
        for (j=0; j < 16 ; j++) {
            map_fill(rom, tiles[j], size);
        }
    }

    map_smooth(rom);

    /* zero out the walkability data */
    continents = find_walkable_area(&rom->map, continent_sizes);
    if (add_bridges(&rom->map, continent_sizes))
        continents = find_walkable_area(&rom->map, continent_sizes);
    place_landmarks(&rom->map, continent_sizes, continents);
    return map_encode(rom);
}

