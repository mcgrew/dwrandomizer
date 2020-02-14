
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "chaos.h"
#include "mt64.h"
#include "dwr.h"
#include "patch.h"
#include "polyfit.h"


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

    if (enemy_a->rank > enemy_b->rank)
        return 1;
    if (enemy_a->rank < enemy_b->rank)
        return -1;
    return 0;
}

static double next_rank(uint64_t flags, double *rank, double min, double max)
{
    double newmin, newmax;

    if (flags /* & FLAG_g */) {
        newmin = MAX((*rank)-3, min);
        newmax = MIN((*rank)+3, max);
        *rank = mt_rand_double_ranged(newmin, newmax);
    } else {
        *rank = mt_rand_double_ranged(min, max);
    }
    return *rank;
}

/**
 * Randomizes all enemy stats.
 *
 * @param rom The rom struct.
 */
static void chaos_enemy_stats(dw_rom *rom)
{
    int i;
    double x, rank;
    uint64_t flags;
    dw_enemy *enemies;

    x = 0;
    flags = rom->flags;
    enemies = rom->enemies;

    for (i=SLIME; i <= RED_DRAGON; i++) {
        rank  = next_rank(0, &x, 1, 40);
        enemies[i].hp  = (uint8_t)polyfit(x, &mon_hp_fac);
        rank += next_rank(flags, &x, 1, 40);
        enemies[i].str = (uint8_t)polyfit(x, &mon_str_fac);
        rank += next_rank(flags, &x, 1, 40);
        enemies[i].agi = (uint8_t)polyfit(x, &mon_agi_fac);
        rank += next_rank(flags, &x, 1, 40) / 2;
        enemies[i].s_ss_resist = ((uint8_t)polyfit(x, &mon_sr_fac)) << 4;
        rank += next_rank(flags, &x, 1, 40) / 2;
        enemies[i].s_ss_resist |= (uint8_t)polyfit(x, &mon_ssr_fac);
        rank += next_rank(flags, &x, 1, 40) / 2;
        enemies[i].hr_dodge    = ((uint8_t)polyfit(x, &mon_hr_fac)) << 4;
        rank += next_rank(flags, &x, 1, 40) / 2;
        enemies[i].hr_dodge   |=  (uint8_t)polyfit(x, &mon_dodge_fac);
        if (!enemies[i].pattern) {
            enemies[i].s_ss_resist |= 15;
        }
        enemies[i].rank = rank / 5;
    }

    rank = next_rank(0, &x, 30, 40);
    enemies[DRAGONLORD_1].hp  = (uint8_t)polyfit(x, &mon_hp_fac);
    rank += next_rank(flags, &x, 30, 40);
    enemies[DRAGONLORD_1].str = (uint8_t)polyfit(x, &mon_str_fac);
    rank += next_rank(flags, &x, 30, 40);
    enemies[DRAGONLORD_1].agi = (uint8_t)polyfit(x, &mon_agi_fac);
    enemies[DRAGONLORD_1].pattern = mt_rand(0, 255);
    enemies[DRAGONLORD_1].s_ss_resist &= 0xf0;
    rank += next_rank(0, &x, 38, 40) / 2;
    enemies[DRAGONLORD_1].s_ss_resist |= (uint8_t)polyfit(x, &mon_sr_fac);
    rank += 40; /* for the default stats */
    enemies[DRAGONLORD_1].rank = rank / 5;

    /* Dragonlord with heal or sleep? That's just evil */
    enemies[DRAGONLORD_2].pattern |= (mt_rand(0, 255) & 0xb0);
    /* don't allow more than 50% to cast*/
    if ((enemies[DRAGONLORD_2].pattern & 0x30) == 0x30) {
        enemies[DRAGONLORD_2].pattern &= 0xcf;
    }
    /* Maybe STOPSPELL Will work on him... */
    enemies[DRAGONLORD_2].s_ss_resist &= 0xf0;
    rank = next_rank(0, &x, 38, 40);
    enemies[DRAGONLORD_2].s_ss_resist |= (uint8_t)polyfit(x, &mon_sr_fac);
    enemies[DRAGONLORD_2].hp = mt_rand(100, 230);
    rank += 104.45; /* for the default stats */
    enemies[DRAGONLORD_2].rank = rank / 5;

    /* update the repel table */
    for (i=SLIME; i <= RED_DRAGON; i++) {
        rom->repel_table[i] = enemies[i].str;
    }
}

static void chaos_enemy_drops(dw_rom *rom)
{
    int i;
    dw_enemy *enemies;
    double x;

    if (!RANDOM_ENEMY_DROPS(rom))
        return;

    enemies = rom->enemies;
    for (i=SLIME; i <= RED_DRAGON; i++) {
        if (RANDOM_ENEMY_STATS(rom))
            x = enemies[i].rank;
        else
            x = i;
        next_rank(rom->flags, &x, 1, 40);
        enemies[i].xp = (uint8_t)polyfit(x, &mon_xp_fac);
        next_rank(rom->flags, &x, 1, 40);
        enemies[i].gold = (uint8_t)polyfit(x, &mon_gold_fac);
    }
}


/**
 * Randomizes enemy zones in a better way for chaos mode.
 *
 * @param rom The rom struct
 */
static void chaos_zones(dw_rom *rom)
{
    int i, zone, index;
    dw_enemies enemies[RED_DRAGON+1];
    dw_enemy *low_str_enemy = NULL;

    /* set up an array to sort enemies by difficulty */
    for (i=SLIME; i <= RED_DRAGON; i++) {
        enemies[i] = i;
    }
    global_enemy = &rom->enemies;
    qsort(enemies, RED_DRAGON+1, sizeof(dw_enemies), &compare_enemies);

    /* randomize zones 0-2 again with weaker monsters */
    for (zone=0; zone <= 2; zone++) {
        for (i=0; i < 5; i++) {
            index = mt_rand(0, 14);
            rom->zones[zone * 5 + i] = enemies[index];
            /* Find the enemy in zones 0-2 with the lowest strength */
            if (!low_str_enemy ||
                    rom->enemies[enemies[index]].str < low_str_enemy->str) {
                low_str_enemy = &rom->enemies[enemies[index]];
            }
        }
    }
    /* ensure that at least one enemy is beatable */
    /* set the lowest strength enemy's HP to a max of 5. */
    low_str_enemy->hp = MIN(low_str_enemy->hp, 5);
    /* remove any abilities */
    low_str_enemy->pattern = 0;
    /* constrict the xp value so it can't be abused */
    low_str_enemy->xp =   rand_power_curve(4, 8, 2.0);

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

    if (!RANDOM_PRICES(rom))
        return;

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

    if (!RANDOM_XP_REQS(rom))
        return;

    for (i=1; i < 30; i++) {
        rom->xp_reqs[i] = 
            (uint16_t)polyfit(mt_rand_double_ranged(1,30), &hero_xp_fac);
    }
    if (FAST_XP(rom)) {
        for (i=1; i < 30; i++) {
            rom->xp_reqs[i] = (uint16_t)((double) rom->xp_reqs[i] * 3 / 4);
        }
    } else if (VERY_FAST_XP(rom)) {
        for (i=1; i < 30; i++) {
            rom->xp_reqs[i] /= 2;
        }
    }
    qsort(rom->xp_reqs, 30, sizeof(uint16_t), &compare_16);
}

/**
 * Changes to the code which determines the chances of running. It will now be
 * based on which map you are on in chaos mode as opposed to enemy index, since
 * enemy index is fairly meaningless in this mode.
 *
 * @param rom The rom struct
 */
void chaos_running(dw_rom *rom)
{
    vpatch(rom, 0xee94, 53,
      /*EE94*/ 0xa5, 0x45,       /* LDA $00E0 ; Load the current map.         */
      /*EE96*/ 0xc9, 0x14,       /* CMP #$01  ; If it's greater than 20.      */
      /*EE98*/ 0x90, 0x07,       /* BCC $EEA1 ;                               */
      /*EE9A*/ 0xa5, 0x95,       /* LDA $0095 ;                               */
      /*EE9C*/ 0x29, 0x7f,       /* AND #$7F  ; ...use a random number 0-127  */
      /*EE9E*/ 0x4c, 0xc7, 0xee, /* JMP $EEC7                                 */

      /*EEA1*/ 0xc9, 0x06,       /* CMP #$06  ; therwise if it's >= 6...      */
      /*EEA3*/ 0x90, 0x05,       /* BCC $EEAA                                 */
      /*EEA5*/ 0xa5, 0x95,       /* LDA $0095                                 */
      /*EEA7*/ 0x4c, 0xc7, 0xee, /* JMP $EEC7 ; use a random number 0-255.    */

      /*EEAA*/ 0xc9, 0x02,       /* CMP #$02  ; Otherwise if it's >= 2...     */
      /*EEAC*/ 0x90, 0x12,       /* BCC $EEB2                                 */
      /*EEAE*/ 0xa5, 0x95,       /* LDA $0095                                 */
      /*EEB0*/ 0x29, 0x3f,       /* AND #$3F                                  */
      /*EEB2*/ 0x85, 0x3e,       /* STA $003E ; put a number 0-63 at $3E.     */
      /*EEB4*/ 0x20, 0x5b, 0xc5, /* JSR $C55B                                 */
      /*EEB7*/ 0xa5, 0x95,       /* LDA $0095                                 */
      /*EEB9*/ 0x29, 0x1f,       /* AND #$1F                                  */
      /*EEBB*/ 0x65, 0x3e,       /* ADC $003E ; Add another number 0-31       */
      /*EEBD*/ 0x4c, 0xc7, 0xee, /* JMP $EEC7 ; This makes the number 0-94.   */

      /*EEC0*/ 0x20, 0x5b, 0xc5, /* JSR $C55B                                 */
      /*EEC3*/ 0xa5, 0x95,       /* LDA $0095 ; If the map is 1 (overworld)   */
      /*EEC5*/ 0x29, 0x3f,       /* AND #$3F  ; load a number 0-63.           */
      /*EEC7*/ 0x85, 0x3c        /* STA $003C ; Store the random number @ $3C */

    );

}

/**
 * Sets up chaos mode options if they are enabled.
 *
 * @param rom The rom struct
 */
void chaos_mode(dw_rom *rom)
{
    if (RANDOM_ENEMY_STATS(rom)) {
        chaos_enemy_stats(rom);
        chaos_zones(rom);
        chaos_running(rom);
    }

    chaos_enemy_drops(rom);
    chaos_xp(rom);
    chaos_weapon_prices(rom);
}



