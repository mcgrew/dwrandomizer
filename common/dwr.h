
#ifndef _DWR_H_
#define _DWR_H_

#include <stdint.h>
#include <stdarg.h>

#include "build.h"
#include "sprites.h"

#define DWR_VERSION "2.2"

#ifdef  DWR_RELEASE
#define VERSION DWR_VERSION
#elif DWR_ALPHA
#define VERSION DWR_VERSION " alpha " BUILD
#else
#define VERSION DWR_VERSION " beta " BUILD
#endif

#define DEFAULT_FLAGS "CDFGMPRWZ"
#define CHEST_COUNT 31

/** 64 bit versions of the randomization flags */
#define FLAG_0 UINT64_C(0x0000000000000001)
#define FLAG_1 UINT64_C(0x0000000000000002)
#define FLAG_2 UINT64_C(0x0000000000000004)
#define FLAG_3 UINT64_C(0x0000000000000008)
#define FLAG_4 UINT64_C(0x0000000000000010)
#define FLAG_5 UINT64_C(0x0000000000000020)
#define FLAG_6 UINT64_C(0x0000000000000040)
#define FLAG_7 UINT64_C(0x0000000000000080)
#define FLAG_8 UINT64_C(0x0000000000000100)
#define FLAG_9 UINT64_C(0x0000000000000200)
#define FLAG_A UINT64_C(0x0000000000000400)
#define FLAG_B UINT64_C(0x0000000000000800)
#define FLAG_C UINT64_C(0x0000000000001000)
#define FLAG_D UINT64_C(0x0000000000002000)
#define FLAG_E UINT64_C(0x0000000000004000)
#define FLAG_F UINT64_C(0x0000000000008000)
#define FLAG_G UINT64_C(0x0000000000010000)
#define FLAG_H UINT64_C(0x0000000000020000)
#define FLAG_I UINT64_C(0x0000000000040000)
#define FLAG_J UINT64_C(0x0000000000080000)
#define FLAG_K UINT64_C(0x0000000000100000)
#define FLAG_L UINT64_C(0x0000000000200000)
#define FLAG_M UINT64_C(0x0000000000400000)
#define FLAG_N UINT64_C(0x0000000000800000)
#define FLAG_O UINT64_C(0x0000000001000000)
#define FLAG_P UINT64_C(0x0000000002000000)
#define FLAG_Q UINT64_C(0x0000000004000000)
#define FLAG_R UINT64_C(0x0000000008000000)
#define FLAG_S UINT64_C(0x0000000010000000)
#define FLAG_T UINT64_C(0x0000000020000000)
#define FLAG_U UINT64_C(0x0000000040000000)
#define FLAG_V UINT64_C(0x0000000080000000)
#define FLAG_W UINT64_C(0x0000000100000000)
#define FLAG_X UINT64_C(0x0000000200000000)
#define FLAG_Y UINT64_C(0x0000000400000000)
#define FLAG_Z UINT64_C(0x0000000800000000)
#define FLAG_a UINT64_C(0x0000001000000000)
#define FLAG_b UINT64_C(0x0000002000000000)
#define FLAG_c UINT64_C(0x0000004000000000)
#define FLAG_d UINT64_C(0x0000008000000000)
#define FLAG_e UINT64_C(0x0000010000000000)
#define FLAG_f UINT64_C(0x0000020000000000)
#define FLAG_g UINT64_C(0x0000040000000000)
#define FLAG_h UINT64_C(0x0000080000000000)
#define FLAG_i UINT64_C(0x0000100000000000)
#define FLAG_j UINT64_C(0x0000200000000000)
#define FLAG_k UINT64_C(0x0000400000000000)
#define FLAG_l UINT64_C(0x0000800000000000)
#define FLAG_m UINT64_C(0x0001000000000000)
#define FLAG_n UINT64_C(0x0002000000000000)
#define FLAG_o UINT64_C(0x0004000000000000)
#define FLAG_p UINT64_C(0x0008000000000000)
#define FLAG_q UINT64_C(0x0010000000000000)
#define FLAG_r UINT64_C(0x0020000000000000)
#define FLAG_s UINT64_C(0x0040000000000000)
#define FLAG_t UINT64_C(0x0080000000000000)
#define FLAG_u UINT64_C(0x0100000000000000)
#define FLAG_v UINT64_C(0x0200000000000000)
#define FLAG_w UINT64_C(0x0400000000000000)
#define FLAG_x UINT64_C(0x0800000000000000)
#define FLAG_y UINT64_C(0x1000000000000000)
#define FLAG_z UINT64_C(0x2000000000000000)

#define BIG_SWAMP(x)          (x->flags & FLAG_b)
#define CURSED_PRINCESS(x)    (x->flags & FLAG_c)
#define RANDOM_ENEMY_DROPS(x) (x->flags & FLAG_d)
#define RANDOM_ENEMY_STATS(x) (x->flags & FLAG_e)
#define NO_KEYS(x)            (x->flags & FLAG_k)
#define SCARED_SLIMES(x)      (x->flags & FLAG_l)
#define MODERN_SPELLS(x)      (x->flags & FLAG_m)
#define OPEN_CHARLOCK(x)      (x->flags & FLAG_o)
#define SHORT_CHARLOCK(x)     (x->flags & FLAG_s)
#define FAST_TEXT(x)          (x->flags & FLAG_t)
#define VANILLA_MAP(x)        (x->flags & FLAG_v)
#define RANDOM_PRICES(x)      (x->flags & FLAG_w)
#define RANDOM_XP_REQS(x)     (x->flags & FLAG_x)
#define THREES_COMPANY(x)     (x->flags & FLAG_h)
#define SHUFFLE_CHESTS(x)     (x->flags & FLAG_C)
#define DEATH_NECKLACE(x)     (x->flags & FLAG_D)
#define FAST_XP(x)            (x->flags & FLAG_F)
#define RANDOMIZE_GROWTH(x)   (x->flags & FLAG_G)
#define SPEED_HACKS(x)        (x->flags & FLAG_H)
#define RANDOMIZE_MUSIC(x)    (x->flags & FLAG_K)
#define RANDOMIZE_SPELLS(x)   (x->flags & FLAG_M)
#define RANDOMIZE_PATTERNS(x) (x->flags & FLAG_P)
#define DISABLE_MUSIC(x)      (x->flags & FLAG_Q)
#define MENU_WRAP(x)          (x->flags & FLAG_R)
#define VERY_FAST_XP(x)       (x->flags & FLAG_V)
#define RANDOMIZE_SHOPS(x)    (x->flags & FLAG_W)
#define RANDOMIZE_ZONES(x)    (x->flags & FLAG_Z)

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initializes the dw_rom struct
 *
 * @param rom An uninitialized dw_rom
 * @param input_file The file to read the rom data from
 * @param flags The flags received from the user.
 * @return A boolean indicating whether initialization was sucessful
 */
BOOL dwr_init(dw_rom *rom, const char *input_file, char *flags);

/**
 * Randomizes a Dragon Warrior rom file
 *
 * @param input_file The name of the input file
 * @param seed The seed number to use for the random number generator
 * @param flags The flags to use for randomization options
 * @param output_dir The directory to write the new file to
 * @return A checksum for the new rom. This checksum is taken before certain
 *      options which don't affect gameplay.
 */
uint64_t dwr_randomize(const char* input_file, uint64_t seed, char *flags,
        const char *sprite_name, const char* output_dir);

#ifdef __cplusplus
}
#endif
#endif

