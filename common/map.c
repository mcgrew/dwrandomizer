
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "mt64.h"
#include "map.h"

#define FILL_ITERATIONS 12
#define MAX_FILL_BLOB   7000
#define MIN_LM_SIZE     400   /* minimum size for a land_mass to use */
#define RIVER_COUNT     25
#define MIN_WALKABLE    5000
#define MAP_ENCODED_SIZE 2294

#define VAN_SPOT_COUNT 13
static const uint8_t vanilla_spots[VAN_SPOT_COUNT][2] = {
    /* vanilla place locations */
    { 43, 43 },
    { 48, 41 },
    {  2,  2 },
    {104, 10 },
    { 81,  1 },
    { 28, 12 },
    { 25, 89 },
    { 73,102 },
    {102, 72 },
    {108,109 },
    {104, 44 },
    { 29, 57 },
    {104, 49 },
    /* extra locations 
    { 83,113 },
    { 40, 82 },
    { 13,107 },
    { 67, 86 },
    { 68, 82 },
    { 75, 82 },
    { 67, 77 },
    {112, 26 },
    {114, 37 }, */
};

static inline int map_ob(int x) {
    return (x < 0 || x > 119);
}

/**
 * Shifts bytes in memory 1 to the left. Shifts [start+1, end) to [start, end-1)
 * This is used for map encoding optimization.
 *
 * @param start A pointer to the location of the new location of the bytes
 * @param end A pointer to the end of the old bytes
 */
static void shift_bytes(uint8_t *start, uint8_t *end)
{
    uint8_t *p;
    --end;

    for (p = start; p < end; p++) {
        *p = *(p + 1);
    }
}

/**
 * Optimizes the map encoding to remove unneeded bytes.
 *
 * @param map The map struct
 * @return The number of bytes saved
 */
static int optimize_map(uint8_t *encoded_map, uint16_t *pointers)
{
    int i, j, saved = 0;
    uint8_t *start, *end;

    end = &encoded_map[120 * 120];

    for (i=1; i < 120; i++) {
        start = &encoded_map[pointers[i] - 0x9d5d - 1];
        if (*start >> 4 == *(start+1) >> 4 && *start <= *(start+1)) {
            ++saved;
            shift_bytes(start, end);
            for (j=i; j < 120; j++) {
                --pointers[j];
            }
        }
    }
    return saved;
}

/**
 * Encodes the newly generated map into one that can be used in-game.
 *
 * @param map The map struct
 * @return A boolean indicating success or failure of map generation
 */
static BOOL map_encode(dw_map *map)
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

    e -= optimize_map(encoded, pointers);
    if (e - encoded > MAP_ENCODED_SIZE) {
        printf("Compressed map is too large (%d)\n", (int)(e - encoded));
        return FALSE;
    } else {
        printf("Compressed map size: %d\n", (int)(e - encoded));
    }
    memcpy(map->encoded, encoded, MAP_ENCODED_SIZE);
    memcpy(map->pointers, pointers, 240);
    return TRUE;
}

/**
 * Decodes the in-game map into a 2 dimentional array of tiles
 *
 * @param map The map struct
 */
void map_decode(dw_map *map)
{
    uint8_t x, y, *e, tile, count;

    for (y=0; y < 120; y++) {
        /* get a pointer to the beginning of the row */
        e = &map->encoded[map->pointers[y] - 0x9d5d];
        /* assign the pointer value to current */
        tile = *e >> 4;
        count = *e & 0xf;
        for (x=0; x < 120; x++) {
            /* fill in each column in the row */
            map->tiles[x][y] = tile;
            if (!count) {
                ++e;
                tile = *e >> 4;
                count = *e & 0xf;
            } else {
                count--;
            }
        }
    }
}

/**
 * Smooths the map to clean it up and ensure better compression.
 *
 * @param map The map struct
 */
static void map_smooth(dw_map *map)
{
    uint8_t x, y;

    /* If a single different tile is between 2 of the same tile, convert it.
       This will help the map compress (and look) better. */
    for (x=1; x < 119; x++) {
        for (y=0; y < 120; y++) {
            if (map->tiles[x-1][y] == map->tiles[x+1][y]) {
                map->tiles[x][y] = map->tiles[x-1][y];
            }
        }
    }
}

/**
 * Determines whether or not an overworld tile can be walked on.
 *
 * @param tile The tile type to check
 * @return A boolean indicating whether the tile can be walked on
 */
static BOOL tile_is_walkable(dw_tile tile)
{
    switch(tile) {
        case TILE_WATER:
        case TILE_MOUNTAIN:
        case TILE_BLOCK:
            return FALSE;
        default:
            return TRUE;
    }
}

/**
 * Removes a land mass from the map, replacing it with water.
 * @param map The map struct
 * @param land_mass The number of the land mass to destroy.
 */
//static void destroy_land_mass(dw_map *map, int land_mass)
//{
//    uint8_t x, y;
//
//    for (x = 0; x < 120; x++) {
//        for (y = 0; y < 120; y++) {
//            if (map->walkable[x][y] == land_mass) {
//                map->tiles[x][y] = TILE_WATER;
//            }
//        }
//    }
//}

/**
 * Finds the size of a land_mass given a point on it. Also fills in the 
 * walkable area array.
 *
 * @param rom The rom pointer.
 * @param x The x coordinate of the tile to start mapping.
 * @param y The y coordinate of the tile to start mapping.
 * @param lm_index The index for this land_mass.
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

/**
 * Determines whether a bridge needs to be placed at this location in order to
 * connect the land on each side.
 *
 * @param left The land mass on the left
 * @param right The land mass on the right
 * @param lm_sizes The sizes of all land masses
 * @return A boolean indicating if a bridge is needed
 */
static inline BOOL needs_bridge(uint8_t left, uint8_t right)
{
    if (!left || !right || left == right)
        return FALSE;
    return TRUE;
}

/**
 * Adds bridges where necessary to connect continents
 * @param map The map struct
 * @return A boolean indicating if a bridge was placed.
 */
static BOOL add_bridges(dw_map *map)
{
    /* this could use more work */
    uint8_t x, y, left, right, x_candidate[120], y_candidate[120];
    uint64_t which;
    int count = 0;
    dw_tile tile;

    for (y=0; y < 120; y++) {
        for (x=2; x < 118; x++) {
            tile = map->tiles[x][y];
            if (tile == TILE_WATER) {
                left = map->walkable[x-1][y];
                right = map->walkable[x+1][y];
                if (needs_bridge(left, right)){
                    x_candidate[count]   = x;
                    y_candidate[count++] = y;
                }
            }
            if (count >= 120) {
                printf("Maximum bridge candidates reached\n");
                goto max_candidates;
            }
        }
    }
max_candidates:
    if (count) {
        which = mt_rand(0, count-1);
        x = x_candidate[which];
        y = y_candidate[which];
        if (tile_is_walkable(map->tiles[x][y-1])) {
            map->tiles[x][y] = map->tiles[x][y-1];

        } else if (tile_is_walkable(map->tiles[x][y+1])) {
            map->tiles[x][y] = map->tiles[x][y+1];
        } else {
            map->tiles[x][y] = TILE_BRIDGE;
        }
        return TRUE;
    }
    return FALSE;
}

/**
 * Finds a land area where something can be placed and will be accessible.
 *
 * @param map The map struct
 * @param one The first land mass that is acceptable.
 * @param two The second land mass that is acceptable. May be the same as 'one',
 *      or may be zero to indicate no mass.
 * @param x A pointer to a uint8_t which will be filled with the x coordinate
 * @param y A pointer to a uint8_t which will be filled with the y coordinate
 */
static void map_find_land(dw_map *map, int one, int two, uint8_t *x, uint8_t *y,
        BOOL ignore_vanilla)
{
    uint8_t cont;
    int spot_index, loop = 0;

    for (;;) {
        if (!ignore_vanilla && !RANDOM_MAP(map) && loop < 50) {
            spot_index = mt_rand(0, VAN_SPOT_COUNT-1);
            *x = vanilla_spots[spot_index][0];
            *y = vanilla_spots[spot_index][1];
            loop++;
        } else {
            *x = mt_rand(0, 119);
            *y = mt_rand(0, 119);
        }
        cont = map->walkable[*x][*y];
        if (cont && (cont == one || cont == two))
            return;
    }
}

/**
 * Translates an overworld map tile into a border tile for a town or dungeon.
 *
 * @param tile The overworld tile index
 * @return The border tile index
 */
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

/**
 * Places a town or cave on the map.
 *
 * @param map The map struct
 * @param warp_idx The index of the warp for this town or dungeon
 * @param tile The tile type to place (town, castle, or cave)
 * @param lm_one The first acceptable land mass for the location
 * @param lm_two The second acceptable land mass for the location
 * @return The tile type on the overworld which was replaced by the new tile
 */
static dw_border_tile place(dw_map *map, dw_warp_index warp_idx, dw_tile tile,
        int lm_one, int lm_two)
{
    uint8_t x, y;
    dw_warp *warp;
    dw_tile old_tile;

    warp = &map->warps_from[warp_idx];

    do {
        map_find_land(map, lm_one, lm_two, &x, &y, FALSE);
        old_tile = map->tiles[x][y];
    } while (old_tile == TILE_CASTLE || old_tile == TILE_TOWN ||
             old_tile == TILE_CAVE);

    warp->map = 1;
    warp->x = x;
    warp->y = y;
    map->tiles[x][y] = tile;
    return border_for(old_tile);
}

/**
 * Places Charlock castle on the map and updates the rainbow drop code
 *
 * @param map The map struct
 * @param largest The first acceptable land mass
 * @param next The second acceptable land mass
 */
static void place_charlock(dw_map *map, int largest, int next)
{

        uint8_t x = 0, y = 0;
        int  i, j;
        dw_warp *warp;

    if (!RANDOM_MAP(map)) {
        map->rainbow_drop->x = x = 65;
        map->rainbow_bridge->x = 64;
        map->rainbow_bridge->y = map->rainbow_drop->y = y = 49;

    } else {
        warp = &map->warps_from[WARP_CHARLOCK];

        while(x < 7 || x > 118 || y < 3 || y > 116) {
            map_find_land(map, largest, next, &x, &y, FALSE);
        }
        warp->x = x-3;
        warp->y = y;
        for (i = -3; i <= 3; i++) {
            for (j = -3; j <= 3; j++) {
                if (i < 0 || j != 0)
                    map->tiles[x - 3 + i][y + j] = TILE_BLOCK;
            }
        }
        for (i = -2; i <= 2; i++) {
            for (j = -2; j <= 2; j++) {
                map->tiles[x - 3 + i][y + j] = TILE_WATER;
            }
        }
        for (i=-1; i <= 1; i++) {
            for (j=-1; j <= 1; j++) {
                map->tiles[x-3+i][y+j] = TILE_SWAMP;
            }
        }
        map->tiles[x-3][y] = TILE_CASTLE;
        map->rainbow_drop->x = warp->x + 3;
        map->rainbow_bridge->x = warp->x + 2;
        map->rainbow_bridge->y = map->rainbow_drop->y = warp->y;

    }

    if (OPEN_CHARLOCK(map)) {
        printf("Leaving Charlock open...\n");
        map->tiles[x-1][y] = TILE_BRIDGE;
    }

}

/**
 * Places tantegel castle on the map and updates the appropriate code to match
 * (return, wings, Gwaelin's Love, etc)
 * @param map
 * @param largest
 * @param next
 * @return
 */
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
    if (y < 119 && tile_is_walkable(map->tiles[x][y+1])) { 
        map->return_point->x = x;
        map->return_point->y = y+1;
    } else if (y > 0 && tile_is_walkable(map->tiles[x][y-1])) { 
        map->return_point->x = x;
        map->return_point->y = y-1;
    } else if (x > 0 && tile_is_walkable(map->tiles[x-1][y])) { 
        map->return_point->x = x-1;
        map->return_point->y = y;
    } else if (x < 119 && tile_is_walkable(map->tiles[x+1][y])) { 
        map->return_point->x = x+1;
        map->return_point->y = y;
    }
    /* do some stuff */
    map->meta[TANTEGEL].border = old_tile;
    return map->walkable[x][y];
}

/**
 * Randomly fills a point of the map with a particular tile.
 *
 * @param map The map struct.
 * @param points An array for points that have been filled.
 * @param x The x coordinate to place the tile
 * @param y The y coordinate to place the tile
 * @param tile The type of tile to place.
 * @return An integer indicating how many tiles to remove from remaining tiles
 *      to place.
 */
static int map_fill_point(dw_map *map, uint8_t *points,
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
    map->tiles[y][x] = (uint8_t)tile;
    if (tile == TILE_MOUNTAIN)
        return 2; /* not so many mountain tiles */
    return 1;
}

/**
 * Fills an area of the map with a particular tile
 *
 * @param map The map struct
 * @param tile The tile to fill the area with
 */
static void map_fill(dw_map *map, dw_tile tile)
{
    uint8_t x, y, *points, *p;
    uint64_t directions;
    int size;

    p = points = malloc(MAX_FILL_BLOB * 4 + 1);
    memset(p, 0xff, MAX_FILL_BLOB * 4 + 1);
    p[0] = mt_rand(0, 119);
    p[1] = mt_rand(0, 119);
    size = mt_rand(MAX_FILL_BLOB/4, MAX_FILL_BLOB);

    while (size > 0) {
        x = p[0]; y=p[1];
        directions = mt_rand(0, 15);
        if (directions & 8) {
            size -= map_fill_point(map, points, x-1, y, tile);
        }
        if (directions & 4) {
            size -= map_fill_point(map, points, x+1, y, tile);
        }
        if (directions & 2) {
            size -= map_fill_point(map, points, x, y-1, tile);
        }
        if (directions & 1) {
            size -= map_fill_point(map, points, x, y+1, tile);
        }
        if (p[2] < 120) {
            p += 2;
        } else break; 
    }
    free(points);

}

/**
 * Finds the largest 2 land masses on the map.
 *
 * @param map The map struct
 * @param lm_sizes An array containing the sizes of each land mass
 * @param lm_count The number of land masses
 * @param largest A pointer to fill with the largest land mass
 * @param next A pointer to fill with the next largest land mass
 */
static inline void find_largest_lm(int *lm_sizes, int lm_count,
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

    if (lm_sizes[(*next)-1] < MIN_LM_SIZE) {
        *next = *largest;
    }
//    printf("Largest: %d(%d), Next: %d(%d)\n", *largest, lm_sizes[(*largest)-1],
//           *next, lm_sizes[(*next)-1]);
}

/**
 * Finds all land masses on the map along with their sizes.
 *
 * @param map The map struct
 * @param lm_sizes An array to fill with the land mass sizes
 * @param largest The index of the largest land mass
 * @param next The index of the next larges land mass
 * @return The total number of land masses found
 */
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
    find_largest_lm(lm_sizes, land_mass, largest, next);
    return (int)land_mass;
}

/**
 * Places landmarks on the map (towns, castles, caves)
 *
 * @param map The map struct
 * @return A boolean indicating success or failure
 */
static BOOL place_landmarks(dw_map *map)
{
    int i, largest = 0, next = 0, lm_sizes[256],
            rimuldar_lm1, rimuldar_lm2, brecc_lm1, brecc_lm2;
    uint8_t tantegel_lm, charlock_lm;
    dw_map_index swamp_north, swamp_south;
    BOOL swamp_placed = FALSE;
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
    find_walkable_area(map, lm_sizes, &largest, &next);


    warp = &map->warps_from[WARP_CHARLOCK];
    place_charlock(map, largest, next);
    find_walkable_area(map, lm_sizes, &largest, &next);
    charlock_lm = map->walkable[warp->x+3][warp->y];

    if (RANDOM_MAP(map)) {
        if (charlock_lm != largest && charlock_lm != next) {
            printf("Charlock is obstructed, retrying...\n");
            return FALSE;
        }
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
        /* make sure the swamp cave entrances are on different land masses */
        if (caves[i] == WARP_SWAMP_NORTH || caves[i] == WARP_SWAMP_SOUTH) {
            if (!swamp_placed) {
                place(map, caves[i], TILE_CAVE, next, 0);
                swamp_placed = TRUE;
            } else {
                place(map, caves[i], TILE_CAVE, largest, 0);
            }
        } else {
            place(map, caves[i], TILE_CAVE, largest, next);
        }
    }
    swamp_north = map->warps_from[WARP_SWAMP_NORTH].map;
    swamp_south = map->warps_from[WARP_SWAMP_SOUTH].map;

    rimuldar_lm1 = brecc_lm1 = largest;
    rimuldar_lm2 = brecc_lm2 = next;

    /* check for swamp cave to be in tantegel and/or garinham */
    if (swamp_north == TANTEGEL || swamp_south == TANTEGEL) {
        if (swamp_placed) {
            tantegel_lm = place_tantegel(map, largest, 0);
        } else {
            tantegel_lm = place_tantegel(map, next, 0);
            swamp_placed = TRUE;
        }
        /* rimuldar needs to be on the same land mass as tantegel (for keys) */
        /* brecconary on the same land mass as tantegel (for uncursing) */
        rimuldar_lm1 = rimuldar_lm2 = brecc_lm1 = brecc_lm2 = (int)tantegel_lm;
    } else {
        tantegel_lm = place_tantegel(map, largest, next);
    }

    if (swamp_north == GARINHAM || swamp_south == GARINHAM) {
        if (swamp_placed) {
            map->meta[GARINHAM].border =
                    place(map, WARP_GARINHAM, TILE_TOWN, largest, 0);
        } else {
            map->meta[GARINHAM].border =
                    place(map, WARP_GARINHAM, TILE_TOWN, next, 0);
        }
        /* rimuldar needs to be on the same land mass as tantegel (for keys) */
        rimuldar_lm1 = rimuldar_lm2 = tantegel_lm;
    } else {
        map->meta[GARINHAM].border =
                place(map, WARP_GARINHAM, TILE_TOWN, largest, next);
    }
    map->meta[RIMULDAR].border =
            place(map, WARP_RIMULDAR, TILE_TOWN, rimuldar_lm1, rimuldar_lm2);
    map->meta[BRECCONARY].border =
            place(map, WARP_BRECCONARY, TILE_TOWN, brecc_lm1, brecc_lm2);

    map->meta[KOL].border =
            place(map, WARP_KOL, TILE_TOWN, largest, next);
    map->meta[HAUKSNESS].border =
            place(map, WARP_HAUKSNESS, TILE_TOWN, largest, next);
    map->meta[CANTLIN].border =
            place(map, WARP_CANTLIN, TILE_TOWN, largest, next);

    return TRUE;
}

static void map_add_river(dw_map *map) {
    int x, y, maindir, dir;

    do {
        x = mt_rand(0, 119);
        y = mt_rand(0, 119);
    } while (map->tiles[x][y] != TILE_WATER);

    maindir = mt_rand(0, 3);
    while (map->tiles[x][y] == TILE_WATER) {

        if (maindir == 0) /* north */
            --y;
        else if (maindir == 1) /* south */
            ++y;
        else if (maindir == 2) /* west */
            --x;
        else if (maindir == 3) /* east */
            ++x;
        if (map_ob(x) || map_ob(y)) {
            return;
        }
    }
    while (map->walkable[x][y] || map->tiles[x][y] != TILE_WATER) {
        map->tiles[x][y] = TILE_WATER;
            dir = mt_rand(0, 3);
         /* keep the general direction */
        if (dir == (maindir ^ 1)) {
            dir ^= 1;
        }
        if (dir == 0) /* north */
            --y;
        else if (dir == 1) /* south */
            ++y;
        else if (dir == 2) /* west */
            --x;
        else if (dir == 3) /* east */
            ++x;

        if (map_ob(x) || map_ob(y)) {
            break;
        }
    }
}

/**
 * Generates a new in-game map.
 *
 * @param rom The rom struct
 * @return A boolean indicating whether terrain generation was successful or not
 */
BOOL map_generate_terrain(dw_rom *rom)
{
    int i, j, x, y, lm_sizes[256];
    int largest, next, total_area;

    if (VANILLA(rom)) {
        return map_encode(&rom->map);
    }
    if (!RANDOM_MAP(rom)) {

        map_decode(&rom->map);
        for (i=0; i < VAN_SPOT_COUNT; ++i) {
            /* clear out the towns & caves so they can be placed again */
            x = vanilla_spots[i][0];
            y = vanilla_spots[i][1];
            rom->map.tiles[x][y] = rom->map.tiles[x-1][y];
            if (!tile_is_walkable(rom->map.tiles[x][y])) {
                rom->map.tiles[x][y] = rom->map.tiles[x+1][y];
            }
        }

    } else {

        dw_tile tiles[16] = {
                TILE_GRASS, TILE_GRASS, TILE_GRASS, TILE_SWAMP,
                TILE_DESERT, TILE_DESERT, TILE_HILL, TILE_MOUNTAIN,
                TILE_TREES, TILE_TREES, TILE_TREES, TILE_WATER,
                TILE_WATER, TILE_WATER, TILE_WATER, TILE_SWAMP
        };

        if (BIG_SWAMP(rom)) {
            tiles[0] = tiles[1] = tiles[4] = tiles[8] = tiles[9] = TILE_SWAMP;
        }

        /* first fill the map with water */
        memset(rom->map.tiles, TILE_WATER, 120 * 120);

        /* now fill it in with some other tiles */
        for (i = 0; i < FILL_ITERATIONS; i++) {
            mt_shuffle(tiles, sizeof(tiles) / sizeof(dw_tile), sizeof(dw_tile));
            for (j = 0; j < 16; j++) {
                map_fill(&rom->map, tiles[j]);
            }
        }

        map_smooth(&rom->map);

        find_walkable_area(&rom->map, lm_sizes, &largest, &next);

        for (i=0; i < RIVER_COUNT; i++) {
            map_add_river(&rom->map);
        }

        find_walkable_area(&rom->map, lm_sizes, &largest, &next);
        /* make sure the walkable area is large enough */
        total_area = lm_sizes[largest - 1];
        if (largest != next)
            total_area += lm_sizes[next - 1];
        if (total_area < MIN_WALKABLE) {
            printf("Total map area is too small, retrying...\n");
            return FALSE;
        }

        while (add_bridges(&rom->map)) {
            find_walkable_area(&rom->map, lm_sizes, &largest, &next);
        }
    }

    find_walkable_area(&rom->map, lm_sizes, &largest, &next);
    if (!place_landmarks(&rom->map)) {
        return FALSE;
    }

    /* place the token */
    find_walkable_area(&rom->map, lm_sizes, &largest, &next);
    map_find_land(&rom->map, largest, next, &rom->token->x, &rom->token->y,
            TRUE);

    return map_encode(&rom->map);
}

