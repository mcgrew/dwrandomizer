//
// Created by mcgrew on 7/29/17.
//

#define _GNU_SOURCE /* for qsort_r */
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "dwr.h"
#include "mt64.h"
#include "chaos.h"

static dw_enemy **global_enemy;

/**
 * Squares a number and with some variance.
 *
 * @param x The number to square.
 * @param max The maximum return value.
 * @return The calculation result.
 */
static inline int rand_power_curve(int min, int max, double power)
{
    return (int)floor(pow(mt_rand_double() * (max - min) + min, power));
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
 * Gives an index for enemy difficulty
 *
 * @param enemy The enemy struct
 * @return  A difficulty index
 */
static inline uint64_t enemy_pwr(dw_enemy *enemy)
{
    uint64_t pwr, multiplier = 0;

    pwr = 1;
    if (enemy->hp > 16)
        pwr *= enemy->hp;
    if (enemy->str > 16)
        pwr *= enemy->str;
    if (enemy->agi > 16)
        pwr *= enemy->agi;
    if (enemy->hp > 64)
        pwr *= enemy->hp;
    if (enemy->str > 64)
        pwr *= enemy->str;
    if (enemy->agi > 64)
        pwr *= enemy->agi;
    /* has healmore or sleep */
    if (enemy->pattern & 0x30) {
        if ((enemy->pattern & 0xc0) == 0) { /* sleep */
            multiplier += 32;
        } else if ((enemy->pattern & 0xc0) == 0xc0) { /* healmore */
            multiplier += 16;
        }
    }
    /* has DL breath or hurtmore */
    if (enemy->pattern & 0x3) {
        if ((enemy->pattern & 0xc) == 0xc) { /* DL breath */
            multiplier += 32;
        } else if ((enemy->pattern & 0xc) == 0x4) { /* hurtmore */
            multiplier += 24;
        }

    }
    return pwr * MAX(multiplier, 1);
}

/**
 * A compare function for enemies for use with qsort
 *
 * @param a The first enemy index
 * @param b The second enemy index
 * @param enemy A pointer to enemy data
 * @return An integer indicating the difficulty relationship between the 2
 *      enemies. 0 indicates equality, a negative number indicates b > a, and a
 *      positive number indicates b < a
 */
static int compare_enemies(const void *a, const void *b)
{
    dw_enemy *enemy_a, *enemy_b;

    enemy_a = &(*global_enemy)[*(dw_enemies*)a];
    enemy_b = &(*global_enemy)[*(dw_enemies*)b];

    if (enemy_pwr(enemy_a) > enemy_pwr(enemy_b))
        return 1;
    if (enemy_pwr(enemy_a) < enemy_pwr(enemy_b))
        return -1;
    return 0;
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
        enemies[i].hp =   rand_power_curve(2, 12, 2.0);
        enemies[i].str =  rand_power_curve(2, 11, 2.0);
        enemies[i].agi =  rand_power_curve(2, 16, 2.0);
        enemies[i].xp =   rand_power_curve(2, 16, 2.0);
        enemies[i].gold = rand_power_curve(2, 16, 2.0);

        enemies[i].s_ss_resist = rand_power_curve(0, 4, 2.0) << 4 |
                                 rand_power_curve(0, 4, 2.0);
        enemies[i].hr_dodge    = rand_power_curve(0, 4, 2.0) << 4 |
                                 rand_power_curve(0, 4, 2.0);
    }

    enemies[DRAGONLORD_1].str = rand_power_curve(9, 16, 2.0);
    enemies[DRAGONLORD_1].xp = 0;
    enemies[DRAGONLORD_1].gold = 0;
    enemies[DRAGONLORD_1].pattern = mt_rand(0, 255);
    enemies[DRAGONLORD_1].s_ss_resist &= 0xf0;
    enemies[DRAGONLORD_1].s_ss_resist |= 15 - rand_power_curve(0, 2, 2.0);

    /* Dragonlord with heal or sleep? That's just evil */
    enemies[DRAGONLORD_2].pattern |= (mt_rand(0, 255) & 0xb0);
    /* don't allow more than 50% to cast*/
    if ((enemies[DRAGONLORD_2].pattern & 0x30) == 0x30) {
        enemies[DRAGONLORD_2].pattern &= 0xcf;
    }
    /* Maybe STOPSPELL Will work on him... MWAHAHAHA */
    enemies[DRAGONLORD_2].s_ss_resist &= 0xf0;
    enemies[DRAGONLORD_2].s_ss_resist |= 15 - rand_power_curve(0, 4, 2.0);
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
    dw_enemies enemies[RED_DRAGON+1];

    /* set up an array to sort enemies by difficulty */
    for (i=SLIME; i <= RED_DRAGON; i++) {
        enemies[i] = i;
    }
    global_enemy = &rom->enemies;
    qsort(enemies, RED_DRAGON+1, sizeof(dw_enemies), &compare_enemies);

//    printf("  HP  STR  AGI\n");
//    for (i=SLIME; i <= RED_DRAGON; i++) {
//        printf("%4d %4d %4d %16lld\n",
//               rom->enemies[enemies[i]].hp,
//               rom->enemies[enemies[i]].str,
//               rom->enemies[enemies[i]].agi,
//               enemy_pwr(&rom->enemies[enemies[i]]));
//    }

    /* randomize zones 0-2 again with weaker monsters */
    for (zone=0; zone <= 2; zone++) {
        for (i=0; i < 5; i++) {
            rom->zones[zone * 5 + i] = enemies[mt_rand(0, 14)];
        }
    }
    /* randomize Charlock Zones */
    for (zone=16; zone <= 18; zone++) {
        for (i=0; i < 5; i++) {
            rom->zones[zone * 5 + i] = enemies[mt_rand(27, 36)];
        }
    }

    for (i=0; i < 3; i++) { /* randomize the forced encounters */
        *rom->encounter_types[i] = enemies[mt_rand(31, 36)];
    }
}

/**
 * Randomizes weapon and item prices
 *
 * @param rom The rom struct
 */
static void chaos_weapon_prices(dw_rom *rom)
{
    int i;

    for (i=0; i < 17; i++) {
        rom->weapon_price_display[i] = rom->weapon_prices[i] =
                rand_power_curve(1, 11, 4.0);
    }
}

/**
 * Randomizes XP requirements for each level
 *
 * @param rom The rom struct
 */
static void chaos_xp(dw_rom *rom)
{
    int i;

    if (FAST_XP(rom)) {
        for (i=1; i < 30; i++) {
            rom->xp_reqs[i] = rand_power_curve(2, 36, 3.0);
        }
    } else if (VERY_FAST_XP(rom)) {
        for (i=1; i < 30; i++) {
            rom->xp_reqs[i] = rand_power_curve(2, 32, 3.0);
        }
    } else {
        for (i=1; i < 30; i++) {
            rom->xp_reqs[i] = rand_power_curve(2, 40, 3.0);
        }
    }
    qsort(rom->xp_reqs, 30, sizeof(uint16_t), &compare_16);
}

/**
 * In random%, determines whether that function will be randomized or not.  This will be determined using a 0-100% model.
 *
 * @param Percentage chance of randomizing that function.
 */
static BOOL random_percent(dw_rom *rom, unsigned int chance) {
    return RANDOM_PCT(rom) && (mt_rand(0, 99) < chance);
}

/**
 * Sets up chaos mode options if they are enabled.
 *
 * @param rom The rom struct
 */
void chaos_mode(dw_rom *rom)
{
    if (!CHAOS_MODE(rom) || !random_percent(rom, 10))
        return;

    chaos_enemies(rom);
    chaos_zones(rom);
    chaos_xp(rom);
    chaos_weapon_prices(rom);
}



