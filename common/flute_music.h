
#ifndef _FLUTE_MUSIC_H_
#define _FLUTE_MUSIC_H_

#include "dwr_types.h"

#define FLUTE_MUSIC_ADDRESS 0x4c9a

void randomize_flute_song(dw_rom *rom);
void speed_up_flute_song(dw_rom *rom);

#endif
