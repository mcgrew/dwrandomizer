//
// Created by mcgrew on 7/29/17.
//

#include <inttypes.h>

#include "dwr.h"
#include "mt64.h"
#include "chaos.h"

/**
 * Squares a number and with some variance.
 *
 * @param x The number to square.
 * @param max The maximum return value.
 * @return The calculation result.
 */
static inline int approx_squared(int x, int max)
{
    x =  x * x - x + mt_rand(0, 2*x);
    return MIN(max, x);
}

/**
 * A function to be passed to qsort for sorting uint16_t arrays
 *
 * @param a The first item to compare
 * @param b The second item to compare
 * @return An integer indicating the relationship between the 2 numbers. 0
 *      indicates equality, a negative number indicates b > a, and a positive
 *      number indicates b < a
 */
static int compare_16(const void *a, const void *b)
{
    return *(uint16_t*)a - *(uint16_t*)b;
}

/**
 * Randomizes all enemy stats.
 *
 * @param rom The rom struct.
 */
static void chaos_enemies(dw_rom *rom)
{
    int i;
    dw_enemy *enemies;

    enemies = rom->enemies;

    for (i=SLIME; i <= DRAGONLORD_1; i++) {
        enemies[i].hp =   approx_squared(mt_rand(2, 12), 255);
        enemies[i].str =  approx_squared(mt_rand(2, 11), 255);
        enemies[i].agi =  approx_squared(mt_rand(2, 16), 255);
        enemies[i].xp =   approx_squared(mt_rand(2, 16), 255);
        enemies[i].gold = approx_squared(mt_rand(2, 16), 255);

        enemies[i].s_ss_resist = approx_squared(mt_rand(0, 4), 15) << 4 |
                                 approx_squared(mt_rand(0, 4), 15);
        enemies[i].hr_dodge    = approx_squared(mt_rand(0, 4), 15) << 4 |
                                 approx_squared(mt_rand(0, 4), 15);
    }

    enemies[DRAGONLORD_1].xp = 0;
    enemies[DRAGONLORD_1].gold = 0;
    enemies[DRAGONLORD_1].pattern = mt_rand(0, 255);

    /* Dragonlord with heal or sleep? That's just evil */
    enemies[DRAGONLORD_2].pattern &= 0xf;
    enemies[DRAGONLORD_2].pattern |= (mt_rand(0, 255) & 0xb0);
    /* Maybe STOPSPELL Will work on him... MWAHAHAHA */
    enemies[DRAGONLORD_2].s_ss_resist &= 0xf0;
    enemies[DRAGONLORD_2].s_ss_resist |= 15 - approx_squared(mt_rand(0, 4), 15);
    enemies[DRAGONLORD_2].hp = mt_rand(100, 230);
}

/**
 * Randomizes enemy zones in a better way for chaos mode.
 *
 * @param rom The rom struct
 */
static void chaos_zones(dw_rom *rom)
{
    int i, zone;

    /* randomize zone layout with no limits */
    for (i=0; i < CHEST_COUNT; i++) {
        rom->zone_layout[i] = 0;
        rom->zone_layout[i] |= mt_rand(0, 15) << 4;
        rom->zone_layout[i] |= mt_rand(0, 15);
    }

    /* randomize zones 0-2 again with no limits */
    for (zone=0; zone <= 2; zone++) {
        for (i=0; i < 5; i++) {
            rom->zones[zone * 5 + i] = mt_rand(SLIME, RED_DRAGON);
        }
    }
}

static void chaos_xp(dw_rom *rom)
{
    int i, x;

    if (FAST_XP(rom)) {
        for (i=1; i < 30; i++) {
            x = mt_rand(2, 36);
            rom->xp_reqs[i] = x * approx_squared(x, 1820);
        }
    } else if (VERY_FAST_XP(rom)) {
        for (i=1; i < 30; i++) {
            x = mt_rand(2, 32);
            rom->xp_reqs[i] = x * approx_squared(x, 2048);
        }
    } else {
        for (i=1; i < 30; i++) {
            x = mt_rand(2, 40);
            rom->xp_reqs[i] = x * approx_squared(x, 1638);
        }
    }
    qsort(rom->xp_reqs, 30, sizeof(uint16_t), &compare_16);
}

/**
 * Sets up chaos mode options if they are enabled.
 *
 * @param rom The rom struct
 */
void chaos_mode(dw_rom *rom)
{
    if (!CHAOS_MODE(rom))
        return;

    chaos_enemies(rom);
    chaos_zones(rom);
    chaos_xp(rom);
}



