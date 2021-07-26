
#ifndef _DWR_H_
#define _DWR_H_

#include <stdint.h>
#include <stdarg.h>

#include "build.h"
#include "sprites.h"

#define DWR_VERSION "3.0"
#define DWR_ALPHA 1

#ifdef  DWR_RELEASE
#define VERSION DWR_VERSION
#elif DWR_ALPHA
#define VERSION DWR_VERSION " alpha " BUILD
#else
#define VERSION DWR_VERSION " beta " BUILD
#endif

#define DEFAULT_FLAGS "CDFGMPRSTWZ"
#define CHEST_COUNT 31

#if 0
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

#define SCALED_SLIMES(x)      (x->flags & FLAG_a)
#define BIG_SWAMP(x)          (x->flags & FLAG_b)
#define CURSED_PRINCESS(x)    (x->flags & FLAG_c)
#define RANDOM_ENEMY_DROPS(x) (x->flags & FLAG_d)
#define RANDOM_ENEMY_STATS(x) (x->flags & FLAG_e)
#define NO_SCREEN_FLASH(x)    (x->flags & FLAG_f)
#define CONSISTENT_STATS(x)   (x->flags & FLAG_g)
#define THREES_COMPANY(x)     (x->flags & FLAG_h)
#define INVISIBLE_HERO(x)     (x->flags & FLAG_i)
#define NOIR_MODE(x)          (x->flags & FLAG_j)
#define NO_KEYS(x)            (x->flags & FLAG_k)
#define SCARED_SLIMES(x)      (x->flags & FLAG_l)
#define MODERN_SPELLS(x)      (x->flags & FLAG_m)
#define NO_NUMBERS(x)         (x->flags & FLAG_n)
#define OPEN_CHARLOCK(x)      (x->flags & FLAG_o)
#define PERMANENT_REPEL(x)    (x->flags & FLAG_p)
#define INVISIBLE_NPCS(x)     (x->flags & FLAG_q)
#define REPEL_IN_DUNGEONS(x)  (x->flags & FLAG_r)
#define SHORT_CHARLOCK(x)     (x->flags & FLAG_s)
#define FAST_TEXT(x)          (x->flags & FLAG_t)
#define NO_HURTMORE(x)        (x->flags & FLAG_u)
#define RANDOM_PRICES(x)      (x->flags & FLAG_w)
#define RANDOM_XP_REQS(x)     (x->flags & FLAG_x)
#define PERMANENT_TORCH(x)    (x->flags & FLAG_y)
#define RANDOM_CHEST_LOCATIONS(x) (x->flags & FLAG_A)
#define SHUFFLE_CHESTS(x)     (x->flags & FLAG_C)
#define DEATH_NECKLACE(x)     (x->flags & FLAG_D)
#define FAST_XP(x)            (x->flags & FLAG_F)
#define RANDOMIZE_GROWTH(x)   (x->flags & FLAG_G)
#define SPEED_HACKS(x)        (x->flags & FLAG_H)
#define RANDOMIZE_MUSIC(x)    (x->flags & FLAG_K)
#define RANDOM_MAP(x)         (x->flags & FLAG_M)
#define RANDOMIZE_PATTERNS(x) (x->flags & FLAG_P)
#define DISABLE_MUSIC(x)      (x->flags & FLAG_Q)
#define MENU_WRAP(x)          (x->flags & FLAG_R)
#define RANDOMIZE_SPELLS(x)   (x->flags & FLAG_S)
#define TORCH_IN_BATTLE(x)    (x->flags & FLAG_T)
#define VERY_FAST_XP(x)       (x->flags & FLAG_V)
#define RANDOMIZE_SHOPS(x)    (x->flags & FLAG_W)
#define HEAL_HURT_B4_MORE(x)  (x->flags & FLAG_Y)
#define RANDOMIZE_ZONES(x)    (x->flags & FLAG_Z)
#endif

#define SHUFFLE_CHESTS(x)         ((x->flags[ 0] >> 6) & 3)
#define RANDOM_CHEST_LOCATIONS(x) ((x->flags[ 0] >> 4) & 3)
#define RANDOMIZE_GROWTH(x)       ((x->flags[ 0] >> 2) & 3)
#define RANDOM_MAP(x)             ((x->flags[ 0] >> 0) & 3)
#define RANDOMIZE_SPELLS(x)       ((x->flags[ 1] >> 6) & 3)
#define RANDOMIZE_SHOPS(x)        ((x->flags[ 1] >> 4) & 3)
#define RANDOM_PRICES(x)          ((x->flags[ 1] >> 2) & 3)
#define RANDOM_XP_REQS(x)         ((x->flags[ 1] >> 0) & 3)
#define HEAL_HURT_B4_MORE(x)      ((x->flags[ 2] >> 6) & 3)

#define MENU_WRAP(x)              ((x->flags[ 3] >> 6) & 3)
#define DEATH_NECKLACE(x)         ((x->flags[ 3] >> 4) & 3)
#define TORCH_IN_BATTLE(x)        ((x->flags[ 3] >> 2) & 3)
#define BIG_SWAMP(x)              ((x->flags[ 3] >> 0) & 3)
#define REPEL_IN_DUNGEONS(x)      ((x->flags[ 4] >> 6) & 3)
#define PERMANENT_REPEL(x)        ((x->flags[ 4] >> 4) & 3)
#define PERMANENT_TORCH(x)        ((x->flags[ 4] >> 2) & 3)

#define RANDOMIZE_PATTERNS(x)     ((x->flags[ 5] >> 6) & 3)
#define RANDOMIZE_ZONES(x)        ((x->flags[ 5] >> 4) & 3)
#define RANDOM_ENEMY_STATS(x)     ((x->flags[ 5] >> 2) & 3)
#define RANDOM_ENEMY_DROPS(x)     ((x->flags[ 5] >> 0) & 3)
#define CONSISTENT_STATS(x)       ((x->flags[ 6] >> 6) & 3)
#define SCARED_SLIMES(x)          ((x->flags[ 6] >> 4) & 3)
#define SCALED_SLIMES(x)          ((x->flags[ 6] >> 2) & 3)

#define FAST_TEXT(x)              ((x->flags[ 7] >> 6) & 3)
#define SPEED_HACKS(x)            ((x->flags[ 7] >> 4) & 3)
#define OPEN_CHARLOCK(x)          ((x->flags[ 7] >> 2) & 3)
#define SHORT_CHARLOCK(x)         ((x->flags[ 7] >> 0) & 3)
#define NO_KEYS(x)                ((x->flags[ 8] >> 6) & 3)
#define FAST_XP(x)                ((x->flags[14] >> 0) & 1)
#define VERY_FAST_XP(x)           ((x->flags[14] >> 1) & 1)

#define NO_HURTMORE(x)            ((x->flags[ 9] >> 6) & 3)
#define NO_NUMBERS(x)             ((x->flags[ 9] >> 4) & 3)
#define INVISIBLE_HERO(x)         ((x->flags[ 9] >> 2) & 3)
#define INVISIBLE_NPCS(x)         ((x->flags[ 9] >> 0) & 3)

#define CURSED_PRINCESS(x)        ((x->flags[ 8] >> 2) & 3)
#define THREES_COMPANY(x)         ((x->flags[ 8] >> 0) & 3)

#define MODERN_SPELLS(x)          ((x->flags[10] >> 6) & 3)
#define NOIR_MODE(x)              ((x->flags[10] >> 4) & 3)
#define RANDOMIZE_MUSIC(x)        ((x->flags[14] >> 7) & 3)
#define DISABLE_MUSIC(x)          ((x->flags[14] >> 6) & 3)
#define NO_SCREEN_FLASH(x)        ((x->flags[14] >> 5) & 3)

#ifdef __cplusplus
extern "C" {
#endif

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

