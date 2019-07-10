
#ifndef DWRANDOMIZER_SPRITES_H
#define DWRANDOMIZER_SPRITES_H

#include "dwr_types.h"

#define SPRITE_COUNT 64

#ifdef __cplusplus
extern "C" {
#endif

extern const char * const dwr_sprite_names[SPRITE_COUNT];

void sprite(dw_rom *rom, const char *sprite_name);

#ifdef __cplusplus
}
#endif

#endif //DWRANDOMIZER_SPRITES_H
