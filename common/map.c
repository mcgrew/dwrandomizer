
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "mt64.h"
#include "map.h"

#define MAX_BLOB 3600
#define MAP_ENCODED_SIZE 2294
#define VIABLE_CONT 250 /* minimum size for a land_mass to use */

static void shift_bytes(uint8_t *start, uint8_t *end)
{
    uint8_t *p;
    --end;

    for (p = start; p < end; p++) {
        *p = *(p + 1);
    }
}

static int optimize_map(dw_map *map)
{
    int i, j, saved = 0;
    uint8_t *start, *end;

    end = &map->encoded[MAP_ENCODED_SIZE];

    for (i=1; i < 120; i++) {
        start = &map->encoded[map->pointers[i] - 0x9d5d - 1];
        if (*start >> 4 == *(start+1) >> 4 && *start < *(start+1)) {
            ++saved;
            shift_bytes(start, end);
            for (j=i; j < 120; j++) {
                --map->pointers[j];
            }
        }
    }
    return saved;
}

static bool map_encode(dw_map *map)
{
    int x, y;
    uint8_t *e, encoded[120 * 120]; 
    uint16_t pointers[120];

    memset(encoded, 0xff, 120 * 120);
    e = encoded;

    for (y=0; y < 120; y++) {
        pointers[y] = e - encoded + 0x9d5d;
        for (x=0; x < 120; x++) {
            if (*e == 0xff) {
                *e = map->tiles[x][y] << 4;
            } else if (*e >> 4 == map->tiles[x][y]) {
                (*e)++;
            } else {
                e++;
                *e = map->tiles[x][y] << 4;
            }
            if ((*e & 0xf) == 0xf) {
                e++;
            }
        }
        if (*e != 0xff) {
            e++;
        }
    }

    if (e - encoded > MAP_ENCODED_SIZE) {
        printf("Compressed map is too large (%d)\n", (int)(e - encoded));
        return false;
    } else {
        printf("Compressed map size: %d\n", (int)(e - encoded));
    }
    memcpy(map->encoded, encoded, MAP_ENCODED_SIZE);
    memcpy(map->pointers, pointers, 240);
    optimize_map(map);
    return true;
}

void map_decode(dw_map *map)
{
    uint8_t x, y, *e, current;

    for (y=0; y < 120; y++) {
        e = &map->encoded[map->pointers[y] - 0x9d5d];
        current = *e;
        for (x=0; x < 120; x++) {
            current--;
            map->tiles[x][y] = current >> 4;
            if (!(current & 0xf)) {
                current = *(++e);
            }
        }
    }
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

static void destroy_land_mass(dw_map *map, int land_mass)
{
    uint8_t x, y;

    for (x = 0; x < 120; x++) {
        for (y = 0; y < 120; y++) {
            if (map->walkable[x][y] == land_mass) {
                map->tiles[x][y] = TILE_WATER;
            }
        }
    }
}

/*
 * Finds the size of a land_mass given a point on it. Also fills in the 
 * walkable area array.
 *
 * Params:
 *  rom : dw_rom*
 *      The rom pointer.
 *  x : uint8_t
 *      The x coordinate of the tile to start mapping.
 *  y : uint8_t
 *      The y coordinate of the tile to start mapping.
 *  lm_index : int
 *      The index for this land_mass.
 */
static int map_land_mass(dw_map *map, uint8_t x, uint8_t y,
        uint8_t lm_index)
{
    int size = 1;

    if (x >= 120 || y >= 120)
        return 0;

    if (map->walkable[x][y])
        return 0;

    if (!tile_is_walkable(map->tiles[x][y]))
        return 0;

    map->walkable[x][y] = lm_index;
    size += map_land_mass(map, x-1, y, lm_index);
    size += map_land_mass(map, x+1, y, lm_index);
    size += map_land_mass(map, x, y-1, lm_index);
    size += map_land_mass(map, x, y+1, lm_index);

    return size;
}

static inline bool needs_bridge(uint8_t left, uint8_t right, int *lm_sizes)
{
    if (!left || !right || left == right)
        return false;
    if (lm_sizes[left-1] < 100 || lm_sizes[right-1] < 100)
        return false;
    return true;
}

static bool add_bridges(dw_map *map, int *lm_sizes)
{
    /* this could use more work */
    uint8_t x, y, left, right, x_candidate[50], y_candidate[50];
    uint64_t which;
    int count = 0;

    for (y=0; y < 120; y++) {
        for (x=2; x < 118; x++) {
            if (map->tiles[x][y] == TILE_WATER) {
                left = map->walkable[x-1][y];
                right = map->walkable[x+1][y];
                if (needs_bridge(left, right, lm_sizes)){
                    x_candidate[count]   = x;
                    y_candidate[count++] = y;
                }
            }
        }
    }
    if (count) {
        which = mt_rand(0, count-1);
        map->tiles[x_candidate[which]][y_candidate[which]] = TILE_BRIDGE;
//        printf("Added bridge\n");
        return true;
    }
    return false;
}


static void map_find_land(dw_map *map, int one, int two, uint8_t *x, uint8_t *y)
{
    uint8_t cont;

    for (;;) {
        *x = mt_rand(0, 119);
        *y = mt_rand(0, 119);
        cont = map->walkable[*x][*y];
        if (cont && (cont == one || cont == two))
            return;
    }
}

static dw_border_tile border_for(dw_tile tile)
{
    switch(tile) {
        case TILE_DESERT: return BORDER_DESERT;
        case TILE_HILL:   return BORDER_HILL;
        case TILE_SWAMP:  return BORDER_SWAMP;
        case TILE_TREES:  return BORDER_TREES;
        default:          return BORDER_GRASS;
    }
}

static dw_border_tile place(dw_map *map, dw_warp_index warp_idx, dw_tile tile,
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
    return border_for(old_tile);
}

static void place_charlock(dw_map *map, int largest, int next)
{
    uint8_t x = 0, y = 0;
    int  i, j;
    dw_warp *warp = &map->warps_from[WARP_CHARLOCK];

    while(x < 7 || x > 118 || y < 3 || y > 116) {
        map_find_land(map, largest, next, &x, &y);
    }
    warp->x = x-3;
    warp->y = y;
    for (i=-3; i <= 3; i++) {
        for (j=-3; j <= 3; j++) {
            if (i < 0 || j != 0)
                map->tiles[x-3+i][y+j] = TILE_BLOCK;
        }
    }
    for (i=-2; i <= 2; i++) {
        for (j=-2; j <= 2; j++) {
            map->tiles[x-3+i][y+j] = TILE_WATER;
        }
    }
    for (i=-1; i <= 1; i++) {
        for (j=-1; j <= 1; j++) {
            map->tiles[x-3+i][y+j] = TILE_SWAMP;
        }
    }
    if (map->flags & FLAG_b) {
        map->tiles[x-3+2][y] = TILE_BRIDGE;
	}
    map->tiles[x-3][y] = TILE_CASTLE;
    map->rainbow_drop->x = warp->x + 3;
    map->rainbow_bridge->x = warp->x + 2;
    map->rainbow_bridge->y = map->rainbow_drop->y = warp->y;
}

static uint8_t place_tantegel(dw_map *map, int largest, int next)
{
    int x, y;
    dw_warp *warp;
    dw_border_tile old_tile;

    old_tile = place(map, WARP_TANTEGEL, TILE_CASTLE, largest, next);
    warp = &map->warps_from[WARP_TANTEGEL];
    x = warp->x;
    y = warp->y;

    map->love_calc->x = x;
    map->love_calc->y = y;
    if (tile_is_walkable(map->tiles[x][y+1])) { 
        map->return_point->x = x;
        map->return_point->y = y+1;
    } else if (tile_is_walkable(map->tiles[x][y-1])) { 
        map->return_point->x = x;
        map->return_point->y = y-1;
    } else if (tile_is_walkable(map->tiles[x-1][y])) { 
        map->return_point->x = x-1;
        map->return_point->y = y;
    } else if (tile_is_walkable(map->tiles[x+1][y])) { 
        map->return_point->x = x+1;
        map->return_point->y = y;
    }
    /* do some stuff */
    map->meta[TANTEGEL].border = old_tile;
    return map->walkable[x][y];
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

static void map_fill(dw_rom *rom, dw_tile tile)
{
    uint8_t x, y, *points, *p;
    uint64_t directions;
    int size;

    p = points = malloc(MAX_BLOB * 4 + 1);
    memset(p, 0xff, MAX_BLOB * 4 + 1);
    p[0] = mt_rand(0, 119);
    p[1] = mt_rand(0, 119);
    size = mt_rand(MAX_BLOB/4, MAX_BLOB);

    while (size > 0) {
        x = p[0]; y=p[1];
        directions = mt_rand(0, 15);
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

static inline void find_largest_lm(dw_map *map, int *lm_sizes, int lm_count,
                            int *largest, int *next)
{
    int i;

    *largest = *next = 0;
    for(i=0; i < lm_count; i++) {
        if (!(*largest) || lm_sizes[i] > lm_sizes[(*largest)-1]) {
            *next = *largest;
            *largest = i + 1;
        } else if (!(*next) || lm_sizes[i] > lm_sizes[(*next)-1]) {
            *next = i + 1;
        }
    }

    if (lm_sizes[(*next)-1] < MAX_BLOB / 6) {
        *next = *largest;
    }
//    printf("Largest: %d(%d), Next: %d(%d)\n", *largest, lm_sizes[(*largest)-1],
//           *next, lm_sizes[(*next)-1]);
}

static int find_walkable_area(dw_map *map, int *lm_sizes, int *largest,
                                     int *next)
{
    int x, y;
    uint8_t land_mass = 0;

    memset(map->walkable, 0, 120 * 120);
    for (y=0; y < 120; y++) {
        for (x=0; x < 120; x++) {
            if (!map->walkable[x][y] && tile_is_walkable(map->tiles[x][y])) {
                lm_sizes[land_mass] = 
                        map_land_mass(map, x, y, land_mass+1);
                land_mass++;
            }
        }
    }
    find_largest_lm(map, lm_sizes, land_mass, largest, next);
    return (int)land_mass;
}

static bool place_landmarks(dw_map *map)
{
    int i, largest = 0, next = 0, lm_count, lm_sizes[256];
    uint8_t tantegel_lm, charlock_lm;
    dw_map_index swamp_north, swamp_south;
    bool swamp_placed = false;
    dw_warp *warp;
    dw_warp_index caves[8] = {
            WARP_SWAMP_NORTH,
            WARP_SWAMP_SOUTH,
            WARP_STAFF_SHRINE,
            WARP_MOUNTAIN_CAVE,
            WARP_JERK_CAVE,
            WARP_ERDRICKS_CAVE,
            WARP_TANTEGEL_BASEMENT,
            WARP_GARINS_GRAVE
    };


    /* find the 2 largest land masses */
    lm_count = find_walkable_area(map, lm_sizes, &largest, &next);


    warp = &map->warps_from[WARP_CHARLOCK];
    place_charlock(map, largest, next);
    lm_count = find_walkable_area(map, lm_sizes, &largest, &next);
    charlock_lm = map->walkable[warp->x+3][warp->y];

    if (charlock_lm != largest && charlock_lm != next) {
        printf("Charlock is obstructed, retrying...\n");
        return false;
    }

    mt_shuffle(caves, 8, sizeof(dw_warp_index));

    /* place the first two caves in tantegel & garinham */
    warp = &map->warps_from[caves[0]];
    warp->map = TANTEGEL;
    warp->x = 29;
    warp->y = 29;
    warp = &map->warps_from[caves[1]];
    warp->map = GARINHAM;
    warp->x = 19;
    warp->y = 0;

    
    /* place the remaining caves */
    for (i=2; i < 8; i++) {
        if (caves[i] == WARP_SWAMP_NORTH || caves[i] == WARP_SWAMP_SOUTH) {
            if (!swamp_placed) {
                place(map, caves[i], TILE_CAVE, next, 0);
                swamp_placed = true;
            } else {
                place(map, caves[i], TILE_CAVE, largest, 0);
            }
        } else {
            place(map, caves[i], TILE_CAVE, largest, next);
        }
    }
    swamp_north = map->warps_from[WARP_SWAMP_NORTH].map;
    swamp_south = map->warps_from[WARP_SWAMP_SOUTH].map;
    
    /* check for swamp cave to be in tantegel and/or garinham */
    if (swamp_north == TANTEGEL || swamp_south == TANTEGEL) {
        if (!swamp_placed) {
            tantegel_lm = place_tantegel(map, next, 0);
        } else {
            tantegel_lm = place_tantegel(map, largest, 0);
        }
        map->meta[RIMULDAR].border =
                place(map, WARP_RIMULDAR, TILE_TOWN, tantegel_lm, 0);
    } else {
        place_tantegel(map, largest, next);
        map->meta[RIMULDAR].border =
                place(map, WARP_RIMULDAR, TILE_TOWN, largest, next);
    }

    if (swamp_north == GARINHAM || swamp_south == GARINHAM) {
        map->meta[GARINHAM].border =
                place(map, WARP_GARINHAM, TILE_TOWN, largest, 0);
    } else {
        map->meta[GARINHAM].border =
                place(map, WARP_GARINHAM, TILE_TOWN, largest, next);
    }


    map->meta[KOL].border =
            place(map, WARP_KOL, TILE_TOWN, largest, next);
    map->meta[BRECCONARY].border =
            place(map, WARP_BRECCONARY, TILE_TOWN, largest, next);
    map->meta[HAUKSNESS].border =
            place(map, WARP_HAUKSNESS, TILE_TOWN, largest, next);
    map->meta[CANTLIN].border =
            place(map, WARP_CANTLIN, TILE_TOWN, largest, next);

    return true;
}


bool map_generate_terrain(dw_rom *rom)
{
    int i, j, lm_count, lm_sizes[256];
    int largest, next, total_area;

    dw_tile tiles[16] = {
            TILE_GRASS, TILE_GRASS, TILE_GRASS, TILE_SWAMP,
            TILE_DESERT, TILE_DESERT, TILE_HILL, TILE_MOUNTAIN,
            TILE_TREES, TILE_TREES, TILE_TREES, TILE_WATER,
            TILE_WATER, TILE_WATER, TILE_WATER, TILE_SWAMP
    };

    /* first fill the map with water */
    memset(rom->map.tiles, TILE_WATER, 120 * 120);

    /* now fill it in with some other tiles */
    for (i = 0; i < 57600 / MAX_BLOB; i++) {
        mt_shuffle(tiles, sizeof(tiles) / sizeof(dw_tile), sizeof(dw_tile));
        for (j = 0; j < 16; j++) {
            map_fill(rom, tiles[j]);
        }
    }

    map_smooth(rom);

    /* zero out the walkability data */
    lm_count = find_walkable_area(&rom->map, lm_sizes, &largest, &next);
    /* make sure the walkable area is large enough */
    total_area = lm_sizes[largest - 1];
    if (largest != next)
        total_area += lm_sizes[next - 1];
    if (total_area < 5000) {
        printf("Total map area is too small, retrying...");
        return false;
    }

    while (add_bridges(&rom->map, lm_sizes)) {
        lm_count = find_walkable_area(&rom->map, lm_sizes, &largest, &next);
    }
    for (i=0; i < lm_count; i++) {
        if (lm_sizes[i] < 200)
            destroy_land_mass(&rom->map, i+1);
    }
    lm_count = find_walkable_area(&rom->map, lm_sizes, &largest, &next);
    if (!place_landmarks(&rom->map)) {
        return false;
    }
    /* place the token */
    lm_count = find_walkable_area(&rom->map, lm_sizes, &largest, &next);
    map_find_land(&rom->map, largest, next, &rom->token->x, &rom->token->y);

    return map_encode(&rom->map);
}

