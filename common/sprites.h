
#ifndef DWRANDOMIZER_SPRITES_H
#define DWRANDOMIZER_SPRITES_H

#include <stdio.h>

#include "dwr_types.h"

#ifdef __cplusplus
extern "C" {
#endif

extern const char * const dwr_sprite_names[];

void sprite(dw_rom *rom, const char *sprite_name);
size_t sprite_count();
void noir_mode(dw_rom *rom);

#ifdef __cplusplus
}
#endif

#endif //DWRANDOMIZER_SPRITES_H
