
#include <stdlib.h>
#include <string.h>

#include "dwr_types.h"
#include "dwr.h"
#include "patch.h"
#include "mt64.h"

/**
 * Whether or not the given map contains roof data. Some of the dungeon maps
 * contain roof data in the MSB of the tile data.
 *
 * @param map The map number
 */
static BOOL contains_roof_data(dw_map_index map)
{
    switch(map) {
        case NO_MAP:
        case OVERWORLD:
        case CHARLOCK:
        case HAUKSNESS:
        case TANTEGEL:
        case TANTEGEL_THRONE_ROOM:
        case CHARLOCK_THRONE_ROOM:
        case KOL:
        case BRECCONARY:
        case GARINHAM:
        case CANTLIN:
        case RIMULDAR:
            return FALSE;
        case TANTEGEL_BASEMENT:
        case NORTHERN_SHRINE:
        case SOUTHERN_SHRINE:
        case CHARLOCK_CAVE_1:
        case CHARLOCK_CAVE_2:
        case CHARLOCK_CAVE_3:
        case CHARLOCK_CAVE_4:
        case CHARLOCK_CAVE_5:
        case CHARLOCK_CAVE_6:
        case SWAMP_CAVE:
        case MOUNTAIN_CAVE:
        case MOUNTAIN_CAVE_2:
        case GARINS_GRAVE_1:
        case GARINS_GRAVE_2:
        case GARINS_GRAVE_3:
        case GARINS_GRAVE_4:
        case ERDRICKS_CAVE:
        case ERDRICKS_CAVE_2:
        default:
                return TRUE;
    }
}

/**
 * Sets a single tile in a town or dungeon to the specified tile. You should
 * be sure to check is_dungeon_tileset() to ensure you are using the correct
 * tileset, as they differ between dungeons and towns.
 *
 * @param rom The rom struct.
 * @param town The town or dungeon index.
 * @param x The X coordinate of the tile to change.
 * @param y The Y coordinate of the tile to change.
 * @param tile The new tile.
 */
void set_dungeon_tile(dw_rom *rom, dw_map_index town, uint8_t x,
        uint8_t y, uint8_t tile)
{
    BOOL roof_data;
    uint8_t oldval;
    size_t offset = (size_t)y * (rom->map.meta[town].width+1) + x;
    size_t data_addr = ((*(uint16_t*)(&rom->map.meta[town].pointer)) & 0x7fff) +
        offset/2;

    if (x > rom->map.meta[town].width || y > rom->map.meta[town].height) {
        printf("Map index (%d, %d) out of bounds, skipping tile setting...\n",
                x, y);
        printf("Map: %d, Width: %d, Height: %d\n", town, 
                rom->map.meta[town].width, rom->map.meta[town].height);
        printf("This is a bug.\n");
        return;
    }

    /* Preserve the MSB for some maps, as they contain roof data for towns. */
    roof_data = contains_roof_data(town);
    if (roof_data && (tile & 8)) {
        printf("Invalid tile %d for map number %d\n", tile, town);
        printf("This is a bug.\n");
        tile &= 7;
    }
    oldval = rom->content[data_addr];
    if (offset % 2) {
        if (roof_data) {
            rom->content[data_addr] &= 0xf8;
        } else {
            rom->content[data_addr] &= 0xf0;
        }
        rom->content[data_addr] |= tile;
    } else {
        if (roof_data) {
            rom->content[data_addr] &= 0x8f;
        } else {
            rom->content[data_addr] &= 0x0f;
        }
        rom->content[data_addr] |= tile << 4;
    }
}

dw_dungeon_tile get_dungeon_tile(dw_rom *rom, dw_map_index town, uint8_t x,
        uint8_t y)
{
    size_t offset = (size_t)y * (rom->map.meta[town].width+1) + x;
    size_t data_addr = ((*(uint16_t*)(&rom->map.meta[town].pointer)) & 0x7fff) +
        offset/2;
    uint8_t tile;

    if (offset % 2) {
        tile = rom->content[data_addr] & 0x0f;
    } else {
        tile = rom->content[data_addr] >> 4;
    }

    if (contains_roof_data(town)) {
        tile &= 0x7;
    }

    return (dw_dungeon_tile)tile;
}

#define ROTATE_90(x)  ((x & 0x3) == 1)
#define ROTATE_180(x) ((x & 0x3) == 2)
#define ROTATE_270(x) ((x & 0x3) == 3)
#define MIRROR_V(x)   (x & 0x4)
#define MIRROR_H(x)   (x & 0x8)

static void rotate_warps(dw_rom *rom, dw_map_index map_index,
        uint8_t rotatemirror)
{
    size_t i;
    uint8_t x, y, tmp;
    dw_warp *warp;
    const dw_map_meta *meta = &rom->map.meta[map_index];
    const size_t warp_count = (rom->map.warps_to - rom->map.warps_from) * 2;

    for(i = 0; i < warp_count; i++) {
        warp = &rom->map.warps_from[i];
        if (warp->map == map_index) {
            x = warp->x;
            y = warp->y;
            if (MIRROR_V(rotatemirror))
                x = meta->width - x;
            if (MIRROR_H(rotatemirror))
                y = meta->height - y;
            if (ROTATE_90(rotatemirror)) {
                tmp = x;
                x = meta->height - y;
                y = tmp;
            }
            if (ROTATE_180(rotatemirror)) {
                x = meta->width - x;
                y = meta->height - y;
            }
            if (ROTATE_270(rotatemirror)) {
                tmp = x;
                x = y;
                y = meta->width - tmp;
            }
            warp->x = x;
            warp->y = y;
        }
    }
}

static void rotate_chest_positions(dw_rom *rom, dw_map_index map_index,
        uint8_t rotatemirror)
{
    size_t i;
    uint8_t x, y, tmp;
    dw_chest *chest;
    const dw_map_meta *meta = &rom->map.meta[map_index];

    for(i = 0; i < 31; i++) {
        chest = &rom->chests[i];
        if (chest->map == map_index) {
            x = chest->x;
            y = chest->y;
            if (MIRROR_V(rotatemirror))
                x = meta->width - x;
            if (MIRROR_H(rotatemirror))
                y = meta->height - y;
            if (ROTATE_90(rotatemirror)) {
                tmp = x;
                x = meta->height - y;
                y = tmp;
            }
            if (ROTATE_180(rotatemirror)) {
                x = meta->width - x;
                y = meta->height - y;
            }
            if (ROTATE_270(rotatemirror)) {
                tmp = x;
                x = y;
                y = meta->width - tmp;
            }
            chest->x = x;
            chest->y = y;
        }
    }
}

static void rotate_forced_encounter(dw_rom *rom, uint8_t rotatemirror)
{
    uint8_t x, y, tmp;
    const dw_map_meta *meta = &rom->map.meta[SWAMP_CAVE];

    x = rom->spike_table->x[1];
    y = rom->spike_table->y[1];
    if (MIRROR_V(rotatemirror)) {
        x = meta->width - x;
    }
    if (MIRROR_H(rotatemirror)) {
        y = meta->height - y;
    }
    if (ROTATE_90(rotatemirror)) {
        tmp = x;
        x = meta->height - y;
        y = tmp;
    }
    if (ROTATE_180(rotatemirror)) {
        x = meta->width - x;
        y = meta->height - y;
    }
    if (ROTATE_270(rotatemirror)) {
        tmp = x;
        x = y;
        y = meta->width - tmp;
    }
    rom->spike_table->x[1] = x;
    rom->spike_table->y[1] = y;
}

static void rotate_mirror_map(dw_rom *rom, dw_map_index map_index)
{
    dw_map_meta *meta = &rom->map.meta[map_index];
    const uint8_t width = meta->width+1, height = meta->height+1;
    const size_t bufsize = width * height;
    size_t i, x, y;
    uint8_t rotatemirror, *buf, *mirroredvbuf, *mirroredhbuf, *rotatedbuf;

    rotatemirror = mt_rand(0, 15);
    if (map_index == SWAMP_CAVE)

    if (!rotatemirror)
        return;

    buf = calloc(bufsize, 1);
    for (y=0; y < height; y++) {
        for (x=0; x < width; x++) {
            buf[y * width + x] = get_dungeon_tile(rom, map_index, x, y);
        }
    }
    // MIRRORING
    if (MIRROR_V(rotatemirror)) { // mirror on vertical axis
        mirroredvbuf = calloc(bufsize, 1);
        for (i = 0; i < bufsize; i++) {
            x = width - 1 - (i % width);
            y = i / width;
            mirroredvbuf[i] = buf[y * width + x];
        }
        free(buf);
    } else {
        mirroredvbuf = buf;
    }

    if (MIRROR_H(rotatemirror)) { // mirror on horizontal axis
        mirroredhbuf = calloc(bufsize, 1);
        for (i = 0; i < bufsize; i++) {
            x = i % width;
            y = height - 1 - (i / width);
            mirroredhbuf[i] = mirroredvbuf[y * width + x];
        }
        free(mirroredvbuf); 
    } else {
        mirroredhbuf = mirroredvbuf;
    }

    // ROTATION
    if (!(rotatemirror & 0x3)) {
        rotatedbuf = mirroredhbuf;
    } else {
        size_t newx, newy, newi;
        rotatedbuf = calloc(bufsize, 1);
        if (ROTATE_90(rotatemirror)) { // rotate 90 degrees clockwise
            for (i = 0; i < bufsize; i++) {
                x = i / height;
                y = height - 1 - (i % height);
                rotatedbuf[i] = mirroredhbuf[y * width + x];
            }
        } else if (ROTATE_180(rotatemirror)) { // rotate 180 degrees clockwise
            for (i = 0; i < bufsize; i++) {
                rotatedbuf[i] = mirroredhbuf[bufsize - 1 - i];
            }
        } else if (ROTATE_270(rotatemirror)) { // rotate 270 degrees clockwise
            for (i = 0; i < bufsize; i++) {
                x = width - 1 - (i / height);
                y = i % height;
                rotatedbuf[i] = mirroredhbuf[y * width + x];
            }
        }
        free(mirroredhbuf);
    }
    rotate_warps(rom, map_index, rotatemirror);
    rotate_chest_positions(rom, map_index, rotatemirror);
    if (map_index == SWAMP_CAVE) {
        rotate_forced_encounter(rom, rotatemirror);
    }


    if (rotatemirror & 1) { // rotated 90 or 270, swap w & h
        x = meta->height;
        meta->height = meta->width;
        meta->width = x;
    }

    for (y=0; y <= meta->height; y++) {
        for (x=0; x <= meta->width; x++) {
            set_dungeon_tile(rom, map_index, x, y,
                    rotatedbuf[y * (meta->width+1) + x]);
        }
    }

    free(rotatedbuf);
}

void rotate_dungeons(dw_rom *rom)
{
    size_t i;
    dw_map_index to_rotate[] = {
//         OVERWORLD,
//         CHARLOCK,
//         HAUKSNESS,
//         TANTEGEL,
//         TANTEGEL_THRONE_ROOM,
//         CHARLOCK_THRONE_ROOM,
//         KOL,
//         BRECCONARY,
//         GARINHAM,
//         CANTLIN,
//         RIMULDAR,
//         TANTEGEL_BASEMENT,
//         NORTHERN_SHRINE,
//         SOUTHERN_SHRINE,

        CHARLOCK_CAVE_1,
        CHARLOCK_CAVE_2,
        CHARLOCK_CAVE_3,
        CHARLOCK_CAVE_4,
        CHARLOCK_CAVE_5,
        CHARLOCK_CAVE_6,
        SWAMP_CAVE,
        MOUNTAIN_CAVE,
        MOUNTAIN_CAVE_2,
        GARINS_GRAVE_1,
        GARINS_GRAVE_2,
        GARINS_GRAVE_3,
        GARINS_GRAVE_4,
        ERDRICKS_CAVE,
        ERDRICKS_CAVE_2,
    };

    if (ROTATE_DUNGEONS(rom)) {
        printf("Rotating and mirroring dungeons...\n");

        for (i=0; i < sizeof(to_rotate) / sizeof(dw_map_index); i++) {
            rotate_mirror_map(rom, to_rotate[i]);
        }
    }
}

