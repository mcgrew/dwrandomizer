
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

#include "dwr.h"
#include "map.h"
#include "patch.h"
#include "mt64.h"

#define CHEST_COUNT 31
#define ROM_SIZE 81936

typedef struct {
    uint8_t index;
    uint8_t width;
    uint8_t height;
    uint8_t tiles[30][30];
} dungeon_map;

static dungeon_map *maps;
static dw_warp *warps_from;
static dw_warp *warps_to;
static const dw_map_index indexes[] = { CHARLOCK, CHARLOCK_THRONE_ROOM,
    CHARLOCK_CAVE_1, CHARLOCK_CAVE_2, CHARLOCK_CAVE_3, CHARLOCK_CAVE_4,
    CHARLOCK_CAVE_5, CHARLOCK_CAVE_6, MOUNTAIN_CAVE, MOUNTAIN_CAVE_2,
    GARINS_GRAVE_1, GARINS_GRAVE_2, GARINS_GRAVE_3, GARINS_GRAVE_4,
    ERDRICKS_CAVE, ERDRICKS_CAVE_2, NO_MAP
};

/**
 * Patches the stair handling routine and the OUTSIDE spell routine for stair
 * shuffling. In the original game, up stairs can only lead to down stairs and
 * vice versa. This fixes that assumption. For OUTSIDE, this changes the code
 * to remember where you last entered a dungeon so it can return you there
 * instead of using a look up based on the current map.
 *
 * @param rom The rom struct
 */
static void code_patches(dw_rom *rom)
{
    vpatch(rom, 0x0d95e,    3,
        0x20, 0xb5, 0xda  /*             jsr save_entry_pos                  */
    );

    vpatch(rom, 0x0d9be, 9,
        0xf0, 0x07,       /*             beq b3_d9c7                         */
        0xc9, 0x03,       /*             cmp #$03                            */
        0xf0, 0x03,       /*             beq b3_d9c7                         */
        0x4c, 0xc6, 0xda  /*             jmp no_stairs                       */
    );

    vpatch(rom, 0x0da06,   11,
        0x4c, 0x3d, 0xd9, /*             jmp $d93d                           */
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
    );

    vpatch(rom, 0x0daa5, 62,
                          /* outside_spell:                                  */
        0xc9, 0x06,       /*             cmp #6                              */
        0xf0, 0x04,       /*             beq +                               */
        0xc9, 0x0f,       /*             cmp #15                             */
        0x90, 0xa8,       /*             bcc b3_da55                         */
        0xae, 0x2f, 0x66, /*         +   ldx $6680                           */
        0xa9,  0x02,      /*             lda #$02                            */
        0x4c, 0xe2, 0xd9, /*             jmp b3_d9e2                         */
                          /*                                                 */
                          /* save_entry_pos:                                 */
        0xa5, 0x45,       /*             lda MAP_INDEX                       */
        0xc9, 0x02,       /*             cmp #2                              */
        0xf0, 0x07,       /*             beq +                               */
        0xc9, 0x0f,       /*             cmp #15                             */
        0xb0, 0x03,       /*             bcs +                               */
        0x8e, 0x2f, 0x66, /*             stx $6680                           */
        0xbd, 0x61, 0xf4, /*         +   lda tab_warp_to,x                   */
        0x60,             /*             rts                                 */
                          /*                                                 */
                          /* no_stairs:                                      */
        0x20, 0xf0, 0xc6, /*             jsr show_window                     */
        0x02,             /*             hex 02                              */
        0x20, 0xcb, 0xc7, /*             jsr b3_c7cb                         */
        0x0d,             /*             hex 0d   ; no stairs here           */
        0x4c, 0xd9, 0xcf, /*             jmp b3_cfd9                         */
        0xff,  0xff,  0xff,  0xff,  0xff,  0xff,  0xff,  0xff,  0xff,
        0xff,  0xff,  0xff,  0xff,  0xff,  0xff,  0xff,  0xff,  0xff
    );
}

/**
 * Checks to see if the given x/y is outside the bounds of the map.
 *
 * @param map A pointer to the dungeon map struct
 * @param x The X coordinate
 * @param y The Y coordinate
 *
 * @return A boolean indicating if the coordinate is out of bounds
 */
static BOOL map_ob(dungeon_map *map, int x, int y)
{
    if (x < 0 || x >= map->width || y < 0 || y >= map->height)
        return TRUE;
    return FALSE;
}

/**
 * Gets a pointer to the requested dungeon_map struct
 *
 * @param index The index of the map
 *
 * @return A pointer to the requested map
 */
static dungeon_map *get_map(dw_map_index index)
{
    size_t i;

    for (i=0; maps[i].index; i++) {
        if (maps[i].index == index)
            return &maps[i];
    }
    return NULL;
}

/**
 * Indicates whether the given tile can be walked on.
 *
 * @param tile The dungeon tile type
 * @param have_keys Causes door tiles to be considered walkable
 *
 * @return A boolean indicating if the tile can be walked on
 */
static BOOL tile_is_walkable(dw_dungeon_tile tile, BOOL have_keys)
{
    switch(tile)
    {
        case DUNGEON_TILE_BLOCK:
        case DUNGEON_TILE_GWAELIN:
            return FALSE;
        case DUNGEON_TILE_DOOR:
            return have_keys;
        default:
            return TRUE;
    }
}

static void map_dungeon(dungeon_map *, uint8_t, uint8_t, BOOL);

/**
 * Follows a warp inside a dungeon and spiders the map at the other end
 *
 * @param map A pointer to the dungeon_map
 * @param x The x coordinate of the warp
 * @param y The y coordinate of the warp
 * @param have_keys Whether door tiles can be walked on or not
 */
static void follow_warp(dungeon_map *map, uint8_t x, uint8_t y, BOOL have_keys)
{
    size_t i;
    dw_warp *from, *to;
    dungeon_map *to_map = NULL;

    for (i=0; warps_to[i].map; i++) {
        from = &warps_from[i];
        to = &warps_to[i];
        if (from->map == map->index && from->x == x && from->y == y) {
            return map_dungeon(get_map(to->map), to->x, to->y, have_keys);
        }
        if (to->map == map->index && to->x == x && to->y == y) {
            return map_dungeon(get_map(from->map), from->x, from->y, have_keys);
        }
    }
}

/**
 * Maps the given dungeon starting at the given map, X, and Y
 *
 * @param map A pointer to the dungeon_map
 * @param x The starting x coordinate
 * @param y The starting y coordinate
 * @param have_keys Whether door tiles can be walked on or not
 */
static void map_dungeon(dungeon_map *map, uint8_t x, uint8_t y, BOOL have_keys)
{
    dw_warp warp;

    if (!map)
        return;

    if (map_ob(map, x, y))
        return;

    if (map->tiles[x][y] & 0x8)
        return;

    if (!tile_is_walkable(map->tiles[x][y], have_keys))
        return;

    map->tiles[x][y] |= 0x8;
    if (map->tiles[x][y] == (DUNGEON_TILE_STAIRS_UP | 0x8) ||
        map->tiles[x][y] == (DUNGEON_TILE_STAIRS_DOWN | 0x8)) {
        follow_warp(map, x, y, have_keys);
    }
    map_dungeon(map, x-1, y, have_keys);
    map_dungeon(map, x+1, y, have_keys);
    map_dungeon(map, x, y-1, have_keys);
    map_dungeon(map, x, y+1, have_keys);


}

// static void map_dungeon(dungeon_map *map, uint8_t x, uint8_t y)
// {
//     int left = 0;
//     int right;
// 
//     if (!map)
//         return;
// 
//     right = map->width;
// 
//     /* Move all the way to the left */
//     while(!map_ob(map, x, y) && tile_is_walkable(map->tiles[x-1][y] & 0x7, TRUE))
//         x--;
//     left = x;
// 
//     while(!map_ob(map, x, y) && tile_is_walkable(map->tiles[x][y] & 0x7, TRUE)) {
//         map->tiles[x][y] |= 0x8;
//         right = x++;
//     }
// 
//     /* go back to the left side and look up & down and follow stairs */
//     for (x = left; x <= right; x++) {
//         if (!map_ob(map, x, y-1) && tile_is_walkable(map->tiles[x][y-1] & 0x7)
//                 && !(map->tiles[x][y-1] & 0x8))
//             map_dungeon(map, x, y-1);
//         if (!map_ob(map, x, y+1) && tile_is_walkable(map->tiles[x][y+1] & 0x7)
//                 && !(map->tiles[x][y+1] & 0x8))
//             map_dungeon(map, x, y+1);
//         if ((map->tiles[x][y] & 0x7) == DUNGEON_TILE_STAIRS_UP ||
//             (map->tiles[x][y] & 0x7) == DUNGEON_TILE_STAIRS_DOWN) {
//             follow_warp(map, x, y);
//         }
//     }
// }

/**
 * Determines if the map uses a dungeon or town tile set
 */
// static BOOL is_dungeon(dw_map_index index)
// {
//     if (index >= CHARLOCK_CAVE_1) {
//         return TRUE;
//     }
//     return FALSE;
// }

/**
 * Unpacks a map into a 2 dimensional array
 *
 * @param rom The rom struct
 * @param map A pointer to the dungeon_map
 */
static void unpack_map(dw_rom *rom, dungeon_map *map)
{
    size_t x, y;
    size_t start_addr = ((*(uint16_t*)(&rom->map.meta[map->index].pointer))
            & 0x7fff);
    uint8_t *start = &rom->content[start_addr];

    for(y=0; y < map->height; y++) {
        for(x=0; x < map->width; x+=2) {
            map->tiles[x][y] = ((*start) >> 4) & 0x7;
            map->tiles[x+1][y] = (*start) & 0x7;
            start++;
        }
    }
}

/**
 * Translates town tiles to dungeon tiles for mapping purposes.
 *
 * @param tile The town tile type
 */
static dw_dungeon_tile translate_town_tile(dw_town_tile tile)
{
    switch(tile) {
        case TOWN_TILE_STAIRS_UP:
            return DUNGEON_TILE_STAIRS_UP;
        case TOWN_TILE_STAIRS_DOWN:
            return DUNGEON_TILE_STAIRS_DOWN;
        case TOWN_TILE_CHEST:
            return DUNGEON_TILE_CHEST;
        case TOWN_TILE_DOOR:
            return DUNGEON_TILE_DOOR;
        case TOWN_TILE_GRASS:
        case TOWN_TILE_DESERT:
        case TOWN_TILE_BRICK:
        case TOWN_TILE_TREES:
        case TOWN_TILE_SWAMP:
        case TOWN_TILE_BARRIER:
        case TOWN_TILE_BRIDGE:
            return DUNGEON_TILE_BRICK;
        case TOWN_TILE_WATER:
        case TOWN_TILE_BLOCK:
        case TOWN_TILE_WPN_SIGN:
        case TOWN_TILE_INN_SIGN:
        case TOWN_TILE_COUNTER:
            return DUNGEON_TILE_BLOCK;
    }

}

/**
 * Unpacks a town map
 *
 * @param rom The rom struct
 * @param map A pointer to the dungeon map
 */
static void unpack_town_map(dw_rom *rom, dungeon_map *map)
{
    size_t x, y;
    uint8_t tmp;
    size_t start_addr = ((*(uint16_t*)(&rom->map.meta[map->index].pointer))
            & 0x7fff);
    uint8_t *start = &rom->content[start_addr];

    for(y=0; y < map->height; y++) {
        for(x=0; x < map->width; x+=2) {
            map->tiles[x][y] = translate_town_tile(*start >> 4);
            map->tiles[x+1][y] = translate_town_tile(*start & 0xf);
            start++;
        }
    }
}

/**
 * Clears all walkability flags in a given dungeon
 *
 * @param map The dungeon_map
 */
static void clear_flags(dungeon_map *map)
{
    size_t x, y;

    for(y=0; y < map->height; y++) {
        for(x=0; x < map->width; x++) {
            map->tiles[x][y] &= 0x7;
        }
    }
}

/**
 * Clears all walkability flags.
 */
static void clear_all_flags()
{
    dungeon_map *map;
    for (map = maps; map->index; map++) {
        clear_flags(map);
    }
}

/**
 * Determines if the indexes array contains the given index
 *
 * @param indexes A pointer to the array of indexes
 * @param index The index to be searched for.
 *
 * @return a booean indicating if the index exists in the array
 */
static BOOL indexes_contains(const dw_map_index *indexes, dw_map_index index)
{
    size_t i;

    for (i=0;; i++) {
        if (indexes[i] == NO_MAP)
            return FALSE;
        if (indexes[i] == index)
            return TRUE;
    }
}

/**
 * Builds maps an initializes other data needed for stair shuffling
 */
static void stair_shuffle_init(dw_rom *rom)
{
    size_t i, j;
    dungeon_map *map;
    size_t map_count = sizeof(indexes) / sizeof(dw_map_index) - 1;

    maps = calloc(map_count+1, sizeof(dungeon_map));
    warps_from = calloc(80, sizeof(dw_warp));
    warps_to = warps_from + 15;
    j = 0;
    for (i=0; i < 102; i++) {
//         if (SHORT_CHARLOCK(rom) && (i % 51) == WARP_CHARLOCK_SURFACE_1)
//             continue;
        if (indexes_contains(indexes, rom->map.warps_from[i%51].map) &&
            indexes_contains(indexes, rom->map.warps_to[i%51].map)) {
            warps_from[j] = rom->map.warps_from[i];
            j++;
        }
        if (i == 51)
            warps_to = warps_from + j;
    }
    j = 0;
    for (i=0; indexes[i]; i++) {
        map = &maps[j++];
        map->index = (uint8_t)indexes[i];
        map->width = rom->map.meta[indexes[i]].width + 1;
        map->height = rom->map.meta[indexes[i]].height + 1;
        if (i < 2)
            unpack_town_map(rom, map);
        else
            unpack_map(rom, map);
    }
    memset(rom->chest_access, 0, 31);
    rom->map.key_access = 0;
}

/**
 * Determines if all chests can be accessed
 *
 * @return A boolean indicating if all chests can be accessed
 */
static BOOL all_chests_accessible()
{
    size_t i, x, y, width, height;
    dungeon_map *map;

    for (i=0; indexes[i]; i++) {
        map = &maps[i];
        for (x=0; x < map->width; x++) {
            for (y=0; y < map->width; y++) {
                if (map->tiles[x][y] == DUNGEON_TILE_CHEST)
                    return FALSE;
            }
        }
    }
    return TRUE;
}

// static void print_all_maps();

/**
 * Shuffles all dungeon stairwells
 *
 * @param rom The rom struct
 */
static void do_shuffle(dw_rom *rom)
{
    dungeon_map *charlock = get_map(CHARLOCK);
    dungeon_map *charlock_throne = get_map(CHARLOCK_THRONE_ROOM);

    while (TRUE) {
        clear_all_flags();
        mt_shuffle(warps_from, (warps_to - warps_from) * 2, sizeof(dw_warp));
        map_dungeon(get_map(GARINS_GRAVE_1), 6, 11, TRUE);
        map_dungeon(get_map(MOUNTAIN_CAVE), 6, 5, TRUE);
        map_dungeon(get_map(ERDRICKS_CAVE), 0, 0, TRUE);
        if (!SHORT_CHARLOCK(rom) &&
            ((charlock_throne->tiles[10][29] | charlock->tiles[10][19]) & 8)) {
            /* DL/Charlock accessible from outside. Try again */
            continue;
        }

        map_dungeon(get_map(CHARLOCK_THRONE_ROOM), 10, 29, TRUE);
        if (!all_chests_accessible()) {
            /* inaccessible chests, try again. */
            continue;
        }
        if (SHORT_CHARLOCK(rom)) {
            /* no need to check charlock throne accessibility */
            break;
        }
        clear_all_flags();
        map_dungeon(charlock, 10, 19, TRUE);
        if (charlock_throne->tiles[10][29] & 8) {
            /* DL is accessible from Charlock, we're good. */
            break;
        }
    }
}

/**
 * Marks all chests that are marked accessible with the given flag
 *
 * @param rom The rom struct
 * @param The bitwise flag to use
 */
static void mark_chests(dw_rom *rom, uint8_t flag)
{
    size_t i, x, y;
    dw_chest *chest;
    dungeon_map *map;

    for (i = 0; i < CHEST_COUNT; i++) {
        chest = &rom->chests[i];
        map = get_map(chest->map);
        if (!map)
            continue;
        x = chest->x;
        y = chest->y;
        if (map->tiles[x][y] == (DUNGEON_TILE_CHEST | 8)) {
            rom->chest_access[i] |= flag;
            if (flag & 0xf && chest->item == KEY)
                rom->map.key_access |= flag;
        }
    }
}

/**
 * Finds all chests that are accessible from each entrance.
 *
 * @param rom The rom struct
 */
static void chest_paths(dw_rom *rom)
{
    dungeon_map *map;

    printf("Mapping Dungeons...\n");

    map = get_map(ERDRICKS_CAVE);
    clear_all_flags();
    map_dungeon(get_map(ERDRICKS_CAVE), 0, 0, FALSE);
    mark_chests(rom, KEY_IN_TABLET);
    clear_all_flags();
    map_dungeon(get_map(ERDRICKS_CAVE), 0, 0, TRUE);
    mark_chests(rom, 0x10);

    clear_all_flags();
    map_dungeon(get_map(MOUNTAIN_CAVE), 6, 5, FALSE);
    mark_chests(rom, KEY_IN_MOUNTAIN);
    clear_all_flags();
    map_dungeon(get_map(MOUNTAIN_CAVE), 6, 5, TRUE);
    mark_chests(rom, 0x20);

    clear_all_flags();
    map_dungeon(get_map(GARINS_GRAVE_1), 6, 11, FALSE);
    mark_chests(rom, KEY_IN_GRAVE);
    clear_all_flags();
    map_dungeon(get_map(GARINS_GRAVE_1), 6, 11, TRUE);
    mark_chests(rom, 0x40);

    clear_all_flags();
    map_dungeon(get_map(CHARLOCK_THRONE_ROOM), 10, 19, FALSE);
    mark_chests(rom, 0x8);
    clear_all_flags();
    map_dungeon(get_map(CHARLOCK_THRONE_ROOM), 10, 19, TRUE);
    mark_chests(rom, 0x80);
}

/**
 * Writes back the shuffled warps to the in-game warp table
 *
 * @param rom The rom struct
 */
static void write_back_warps(dw_rom *rom)
{
    size_t i, j;
    j = 0;
    for (i=0; i < 51; i++) {
        if (indexes_contains(indexes, rom->map.warps_from[i].map) &&
            indexes_contains(indexes, rom->map.warps_to[i].map)) {
            rom->map.warps_from[i] = warps_from[j];
            rom->map.warps_to[i] = warps_to[j];
            j++;
        }
    }
}

/**
 * Shuffles all dungeon stairs and checks that all conditions are met.
 */
void stair_shuffle(dw_rom *rom)
{

    stair_shuffle_init(rom);
    if (STAIR_SHUFFLE(rom)) {
        printf("Shuffling stairs...\n");
        do_shuffle(rom);
        code_patches(rom);
        write_back_warps(rom);
    }
    chest_paths(rom);

}

#if 0
static void print_map(dungeon_map *map)
{
    size_t x, y;
    printf("%d, %d\n", map->width, map->height);

    for(y=0; y < map->height; y++) {
        for(x=0; x < map->width; x++) {
            printf("%X", map->tiles[x][y]);
        }
        printf("\n");
    }
}

static void print_all_maps()
{
    printf("Charlock\n");
    print_map(get_map(CHARLOCK));
    print_map(get_map(CHARLOCK_CAVE_1));
    print_map(get_map(CHARLOCK_CAVE_2));
    print_map(get_map(CHARLOCK_CAVE_3));
    print_map(get_map(CHARLOCK_CAVE_4));
    print_map(get_map(CHARLOCK_CAVE_5));
    print_map(get_map(CHARLOCK_CAVE_6));
    print_map(get_map(CHARLOCK_THRONE_ROOM));
    printf("\nGrave\n");
    print_map(get_map(GARINS_GRAVE_1));
    print_map(get_map(GARINS_GRAVE_1));
    print_map(get_map(GARINS_GRAVE_2));
    print_map(get_map(GARINS_GRAVE_3));
    print_map(get_map(GARINS_GRAVE_4));
    printf("\nMountain\n");
    print_map(get_map(MOUNTAIN_CAVE));
    print_map(get_map(MOUNTAIN_CAVE_2));
    printf("\nErdrick\n");
    print_map(get_map(ERDRICKS_CAVE));
    print_map(get_map(ERDRICKS_CAVE_2));
}

/* test code */

#include "string.h"

BOOL dwr_init(dw_rom *rom, const char *input_file)
{
    FILE *input;
    int read;

    rom->header = malloc(ROM_SIZE);
    input = fopen(input_file, "rb");
    if (!input) {
        fprintf(stderr, "Unable to open ROM file '%s'", input_file);
        return FALSE;
    }
    read = fread(rom->header, 1, ROM_SIZE, input);
    if (read < ROM_SIZE) {
        fprintf(stderr, "File '%s' is too small and may be corrupt, aborting.",
                input_file);
        fclose(input);
        return FALSE;
    }
    fclose(input);

    rom->content = &rom->header[0x10];
    rom->map.warps_from = (dw_warp*)&rom->content[0xf3c8];
    rom->map.warps_to   = (dw_warp*)&rom->content[0xf461];
    rom->chests = (dw_chest*)&rom->content[0x5dcd];

    rom->map.flags = rom->flags;
    memset(rom->chest_access, 0, 31);
    rom->map.key_access = 0;

    rom->map.meta = (dw_map_meta*)&rom->content[0x1a];
    return TRUE;
}


int main(int argc, char **argv)
{
    dw_rom rom;
    size_t i;

    dwr_init(&rom, argv[1]);
    mt_init(time(NULL));
    stair_shuffle(&rom);
    clear_all_flags();
    map_dungeon(get_map(ERDRICKS_CAVE), 0, 0, TRUE);
    map_dungeon(get_map(MOUNTAIN_CAVE), 6, 5, TRUE);
    map_dungeon(get_map(GARINS_GRAVE_1), 6, 11, TRUE);
//     map_dungeon(get_map(CHARLOCK), 10, 19, TRUE);
    print_all_maps();

    dw_warp *warp;
    for (i=0; i < 51; i++) {
        warp = &rom.map.warps_from[i];
        printf("%2d %2d %2d -> ", warp->map, warp->x, warp->y);
        warp = &rom.map.warps_to[i];
        printf("%2d %2d %2d\n", warp->map, warp->x, warp->y);
    }

    for (i=0; i < CHEST_COUNT; i++) {
        printf("%X", rom.chest_access[i]);
    }
    printf("\n");
    printf("%X\n", rom.map.key_access);

    free(maps);
    free(warps_from);
    return 0;
}
#endif
