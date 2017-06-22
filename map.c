
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "mt64.h"
#include "map.h"

#define MAX_BLOB 2400
#define MAP_ENCODED_SIZE 2534

static bool map_encode(dw_rom *rom)
{
    int i, j;
    uint8_t *e, encoded[120 * 120]; 
    uint16_t pointers[121];

    memset(encoded, 0xff, 120 * 120);
    e = encoded;

    pointers[0] = 0x9d5d;
    for (i=0; i < 120; i++) {
        for (j=0; j < 120; j++) {
            if (*e == 0xff) {
                *e = rom->map.tiles[i][j] << 4;
            } else if (*e >> 4 == rom->map.tiles[i][j]) {
                (*e)++;
            } else {
                e++;
                *e = rom->map.tiles[i][j] << 4;
            }
            if ((*e & 0xf) == 0xf) {
                e++;
            }
        }
        e++;
        pointers[i+1] = e - encoded + 0x9d5d;
    }
    /* need to optimize the map here */
    for (i=0; i < 120; i++) {
        for (j=0; j < 120; j++) {
//            printf("%d ", rom->map.tiles[j][i]);
        }
//        printf("\n");
    }
//    printf("\n");

//    for (i=0; i < MAP_ENCODED_SIZE; i++)
//        printf("%02x ", encoded[i]);
//    printf("\n");

    if (e - encoded > MAP_ENCODED_SIZE) {
        printf("Map is too large\n");
        return false;
    }
    memcpy(rom->map.encoded, encoded, MAP_ENCODED_SIZE);
    memcpy(rom->map.pointers, pointers, 240);
    return true;
}

void map_decode(dw_rom *rom)
{
}

static void map_find_land(dw_rom *rom, int *continents, int cont_count,
        int *coords)
{
    int i;
    uint8_t x, y, cont;

    for (;;) {
        coords[0] = mt_rand(0, 120);
        coords[1] = mt_rand(0, 120);
        cont = rom->map.walkable[x][y];
        for (i=0; i < cont_count; i++) {
            if (cont == continents[i]) {
                return;
            }
        }
    }
}

static bool place_landmarks(dw_rom *rom)
{
}

static int map_fill_point(dw_rom *rom, uint8_t *points, uint8_t x, uint8_t y,
        dw_tile tile)
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

static int points_len(int *p) {
    int ret = -2;
    while (*p >= 0) {
        printf("%d\n", *p);
        p++;
        ret++;
    }
    return ret/2;
}

static void map_fill(dw_rom *rom, dw_tile tile, int size)
{
    uint8_t x, y, *points, *p;
    uint64_t directions;

    p = points = malloc(MAX_BLOB * 2 + 1);
    memset(p, 0xff, MAX_BLOB * 2 + 1);
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
static void smooth_map(dw_rom *rom)
{
    uint8_t *tiles, *point;

    tiles = (uint8_t*)rom->map.tiles;
    /* If a single different tile is between 2 of the same tile, convert it.
       This will help the map compress (and look) better. */
    for (point = tiles + 1; point - tiles < 120*120-1; point++) {
        if (*point != *(point-1) && *(point-1) == *(point+1)) {
            *point = *(point-1);
        }
    }
}

/*
 * Finds the size of a continent given a point on it. Also fills in the 
 * walkable area array.
 *
 * Params:
 *  rom : dw_rom*
 *      The rom pointer.
 *  continent_index : int
 *      The index for this continent.
 */
static int map_continent(dw_rom *rom, int continent_index)
{
}

static bool tile_is_walkable(dw_rom *rom, int x, int y)
{
    dw_tile tile = (dw_tile)rom->map.tiles[x][y];

    switch(tile) {
        case TILE_WATER:
        case TILE_MOUNTAIN:
        case TILE_BLOCK:
            return false;
        default:
            return true;
    }
}

static inline void find_walkable_area(dw_rom *rom, int *continent_sizes)
{
    int largest = 0, next_largest = 0;
}

bool map_generate_terrain(dw_rom *rom)
{
    int i, j, size, continent_sizes[1024];

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

    smooth_map(rom);
    find_walkable_area(rom, continent_sizes);
    return map_encode(rom);
}

