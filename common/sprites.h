//
// Created by mcgrew on 8/30/17.
//

#ifndef DWRANDOMIZER_SPRITES_H
#define DWRANDOMIZER_SPRITES_H

#include "dwr_types.h"
#define SPRITE_COUNT 49

#ifdef __cplusplus
extern "C" {
#endif

extern const char *dwr_sprite_names[SPRITE_COUNT];

void sprite(dw_rom *rom, const char *sprite_name);

#ifdef __cplusplus
}
#endif

#endif //DWRANDOMIZER_SPRITES_H
