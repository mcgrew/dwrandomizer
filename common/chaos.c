
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "chaos.h"
#include "mt64.h"
#include "dwr.h"
#include "patch.h"


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

    enemies[DRAGONLORD_1].hp  = rand_power_curve(9, 14, 2.0);
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

    /* update the repel table */
    for (i=SLIME; i <= RED_DRAGON; i++) {
        rom->repel_table[i] = enemies[i].str;
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
 * Changes to the code which determines the chances of running. It will now be
 * based on which map you are on in chaos mode as opposed to enemy index, since
 * enemy index is fairly meaningless in this mode.
 *
 * @param rom The rom struct
 */
void chaos_running(dw_rom *rom)
{
    vpatch(rom, 0xeea4, 53,
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
    if (!CHAOS_MODE(rom))
        return;

    chaos_enemies(rom);
    chaos_zones(rom);
    chaos_xp(rom);
    chaos_weapon_prices(rom);
    chaos_running(rom);
}



