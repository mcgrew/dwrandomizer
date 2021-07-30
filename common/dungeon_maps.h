

#ifndef _DUNGEON_MAPS_H_
#define _DUNGEON_MAPS_H_

#include "dwr_types.h"

void rotate_dungeons(dw_rom *rom);
void set_dungeon_tile(dw_rom *rom, dw_map_index town, uint8_t x,
        uint8_t y, uint8_t tile);

#endif
