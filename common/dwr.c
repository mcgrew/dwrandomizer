
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <assert.h>

#include "dwr.h"
#include "patch.h"
#include "chaos.h"
#include "map.h"
#include "dungeon_maps.h"
#include "chests.h"
#include "challenge.h"
#include "crc64.h"
#include "mt64.h"
#include "polyfit.h"
#include "base32.h"

const char *prg0sums[8] = {
      "6a50ce57097332393e0e8751924fd56456ef083c", /* Dragon Warrior (U) (PRG0) [!].nes      */
      "66330df6fe3e3c85adb8183721e5f88c149e52eb", /* Dragon Warrior (U) (PRG0) [b1].nes     */
      "49974889619f1d8c39b6c20fa208c62a0a73ecce", /* Dragon Warrior (U) (PRG0) [b1][o1].nes */
      "d98b8a3fc93bb2f1f5016326556b68998dd5f85d", /* Dragon Warrior (U) (PRG0) [b2].nes     */
      "e81a693efe322be9584c97b55c6d7ae38ae44a66", /* Dragon Warrior (U) (PRG0) [o1].nes     */
      "6e1a52b7b3a13494536bbab7248690861665001a", /* Dragon Warrior (U) (PRG0) [o2].nes     */
      "3077d5bd5c5c3744398b122d5ee1bba7055c8d45", /* Dragon Warrior (U) (PRG0) [o3].nes     */
      NULL
};
const char *prg1sums[2] = {
      "1ecc63aaac50a9612eaa8b69143858c3e48dd0ae", /* Dragon Warrior (U) (PRG1) [!].nes      */
      NULL
};


/* The [ and ] are actually opening/closing quotes. The / is .' */
const char alphabet[] = "0123456789abcdefghijklmnopqrstuvwxyz"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ[]'*>_:__.,-_?!;)(``/'___________  ";

const char title_alphabet[] = "0123456789__________________________"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ__________________________________!.c-     ";

const char flag_order[] =
        "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

/**
 * Returns the version number. Mosltly for use with emscripten.
 *
 * @return The version number
 */
const char *version()
{
    return VERSION;
}

/**
 * A function to be passed to qsort for sorting uint8_t arrays
 *
 * @param a The first item to compare
 * @param b The second item to compare
 * @return An integer indicating the relationship between the 2 numbers. 0
 *      indicates equality, a negative number indicates b > a, and a positive
 *      number indicates b < a
 */
static int compare(const void *a, const void *b)
{
    return *(uint8_t*)a - *(uint8_t*)b;
}

/**
 * Parses the flags string into a 64-bit integer, one bit indicating the status
 * of each flag.
 *
 * @param rom A dw_rom struct containing the rom data.
 * @param flags The flags from the user
 * @return A 64-bit integer containing the flags
 */
// static uint64_t parse_flags(dw_rom *rom, char *flags)
// {
//     int i, len;
//     char *order;
// 
//     len = strlen(flags);
//     qsort(flags, len, sizeof(char), &compare);
//     rom->flags = 0;
//     for (i=0; i < len; i++) {
//         order = strchr(flag_order, flags[i]);
//         if (order) {
//             rom->flags |= UINT64_C(1) << (order - flag_order);
//         }
//     }
//     return rom->flags;
// }

/**
 * Updates the "maybe" flags to be either on or off
 *
 */
static void update_flags(dw_rom *rom)
{
    size_t i;
    uint8_t tmp;

    /* Only the first 12 bytes can contain maybe flags. */
    for (i=0; i < 12; i++) {
        rom->flags[i] |= (rom->flags[i] >> 1) & 0x55 & mt_rand(0, 255);
        rom->flags[i] &= 0x55;
    }
    if (RANDOM_MAP_SIZE(rom) == 0xc) {
        rom->flags[14] &= 0xf3;
        rom->flags[14] |= mt_rand(0, 2) << 2;
    }
}

/**
 * Initializes the dw_rom struct
 *
 * @param rom An uninitialized dw_rom
 * @param input_file The file to read the rom data from
 * @param flags The flags received from the user.
 * @return A boolean indicating whether initialization was sucessful
 */
static BOOL dwr_init(dw_rom *rom, const char *input_file, char *flags)
{
    FILE *input;
    int read;

    rom->header = malloc(ROM_SIZE);
    memset(rom->header, 0, ROM_SIZE);
    input = fopen(input_file, "rb");
    if (!input) {
        fprintf(stderr, "Unable to open ROM file '%s'", input_file);
        return FALSE;
    }
    read = fread(rom->header, 1, ROM_SIZE, input);
    if (read < ROM_SIZE) {
        fprintf(stderr, "File '%s' is too small and may be corrupt, aborting.",
                input_file);
        fclose(input);
        return FALSE;
    }
    fclose(input);

    rom->content = &rom->header[0x10];
    strncpy((char *)rom->flags_encoded, flags, 24);
    rom->flags_encoded[24] = '\0'; /* Make sure it's null terminated */

    rom->map.flags = rom->flags;
    memset(rom->flags, 0, 15);
    base32_decode(rom->flags_encoded, rom->flags);
    update_flags(rom);
    /* subtract 0x9d5d from these pointers */
    rom->map.pointers = (uint16_t*)&rom->content[0x2653];
    rom->map.encoded = &rom->content[0x1d5d];
    rom->map.meta = (dw_map_meta*)&rom->content[0x1a];
    rom->map.warps_from = (dw_warp*)&rom->content[0xf3c8];
    rom->map.warps_to   = (dw_warp*)&rom->content[0xf461];
    rom->map.love_calc = (dw_love_calc*)&rom->content[0xdf3b];
    rom->map.return_point = (dw_return_point*)&rom->content[0xdb01];
    rom->map.rainbow_drop = (dw_rainbow_drop*)&rom->content[0xde8b];
    rom->map.rainbow_bridge = (dw_rainbow_drop*)&rom->content[0x2c3b];
    rom->stats = (dw_stats*)&rom->content[0x60cd];
    rom->new_spells = (dw_new_spell*)&rom->content[0xeae8];
    rom->mp_reqs = &rom->content[0x1d53];
    rom->xp_reqs = (uint16_t*)&rom->content[0xf35b];
    rom->enemies = (dw_enemy*)&rom->content[0x5e4b];
    rom->zones = &rom->content[0xf54f];
    rom->zone_layout = &rom->content[0xf522];
    rom->chests = (dw_chest*)&rom->content[0x5dcd];
    rom->spike_entries = (dwr_spike_table_entry*)&rom->content[0xcd82];
    // FIXME: these should be removed or modified for the new treasure code.
    // check to see if this has been done.
    rom->token = (dw_searchable*)&rom->content[0xe10b];
    rom->flute = (dw_searchable*)&rom->content[0xe12a];
    rom->armor = (dw_searchable*)&rom->content[0xe140];
    rom->repel_table = &rom->content[0xf4fa];
    rom->weapon_shops = &rom->content[0x1991];
    rom->weapon_prices = (uint16_t*)&rom->content[0x1947];
    rom->weapon_price_display = (uint16_t*)&rom->content[0x7e10];
    rom->music = &rom->content[0x31af];
    rom->title_text = &rom->content[0x3f26];

    map_decode(&rom->map);
    return TRUE;
}

/**
 * Converts an ASCII string into one suitable for use in-game
 *
 * @param string The string to be converted.
 * @return The number of characters converted.
 */
size_t ascii2dw(uint8_t *string)
{
    uint8_t i, j;
    int len, alphalen;

    len = strlen((char*)string);
    alphalen = strlen(alphabet);
    for (i=0; i < len; i++) {
        for (j=0; j < alphalen; j++) {
            if (string[i] == alphabet[j]) {
                string[i] = j;
                break;
            }
        }
    }
    return i;
}

/**
 * Converts an ASCII string to uppercase
 *
 * @param string The string to convert
 */
void touppercase(uint8_t *string) {
    char *c = (char*)string;
    while (*c) {
        *c = toupper(*c);
        c++;
    }
}

/**
 * Converts an ASCII string into one suitable for use on the title screen or
 * ending screen
 *
 * @param string The string to be converted.
 * @return The number of characters converted.
 */
size_t ascii2dw_title(uint8_t *string)
{
    uint8_t i, j;
    int len, alphalen;

    touppercase(string);

    len = strlen((char*)string);
    alphalen = strlen(title_alphabet);
    for (i=0; i < len; i++) {
        for (j=0; j < alphalen; j++) {
            if (string[i] == title_alphabet[j]) {
                string[i] = j;
                break;
            } else if (j == alphalen-1) {
                string[i] = 0x5f;
            }
        }
    }
    return i;
}

/**
 * Converts an in-game string to ASCII
 *
 * @param string The string to convert
 * @param bufsize The size of the buffer to be converted
 * @return The number of characters converted (normally the same as bufsize)
 */
size_t dw2ascii(uint8_t *string, size_t bufsize)
{
    size_t i, alphalen;

    alphalen = strlen(alphabet);
    for (i=0; i < bufsize; i++) {
        if (string[i] < alphalen) {
            string[i] = alphabet[string[i]];
        } else {
            string[i] = ' ';
        }
    }
    return bufsize;
}

size_t set_text(dw_rom *rom, const size_t address, char *text)
{
    size_t size = strlen(text);
    uint8_t *translated = malloc(size+1);

    strcpy((char *)translated, text);
    ascii2dw(translated);
    return patch(rom, address, strlen(text), translated);
}


/**
 * Searches for a string inside the rom and replaces it with the new string
 *
 * @param rom The rom struct
 * @param text An ASCII version of the string to be replaced
 * @param replacement An ASCII version of the replacement string
 * @return returns a pointer to the beginning of the string that was replaced,
 *      or NULL if the string is not found.
 */
static uint8_t *dwr_str_replace(dw_rom *rom, const char *text,
                                const char *replacement)
{
    int len;
    uint8_t *start, *end;
    char dw_text[256], dw_repl[256];

    len = strlen(text);
    if (!len)
        return NULL;
    end = &rom->content[ROM_SIZE - len - 0x10];

    strncpy(dw_text, text, 256);
    strncpy(dw_repl, replacement, 256);
    ascii2dw((unsigned char *)dw_text);
    ascii2dw((unsigned char *)dw_repl);

    for (start = rom->content; start < end; start++) {
        if (!memcmp(start, dw_text, len)) {
            memcpy(start, dw_repl, len);
            return start;
        }
    }
    return NULL;
}

/**
 * Patches the game to allow the use of torches and fairy water in battle
 *
 * @param rom The rom struct
 */
static void torch_in_battle(dw_rom *rom) {
    /* change the jump address in the item use code */

    if (!TORCH_IN_BATTLE(rom))
        return;

    printf("Making torches and fairy water more deadly...\n");
    /* patch the jump address */
    vpatch(rom, 0x0e87d,   2, 0x75, 0xc4);

    vpatch(rom, 0x0c475, 71,
        0xc9, 0x04,
        0xd0, 0x2a,
        0xa9, 0x01,
        0x20, 0x4b, 0xe0,
        0x20, 0xc5, 0xc7,
        0x29,
        0xa5, 0xe0,
        0xc9, 0x10,
        0xd0, 0x0f,
        0x20, 0x5b, 0xc5,
        0xa5,
        0x95, 0x29, 0x01,
        0xd0, 0x03,
        0x4c, 0x58, 0xe6,
        0x4c, 0x94, 0xe6,
        0x20, 0x5b, 0xc5,
        0xa5, 0x95,
        0x29, 0x03,
        0x69, 0x06,
        0x4c, 0x94, 0xe6,
        0xc9, 0x05,
        0xd0, 0x12,
        0xa9, 0x02,
        0x20, 0x4b, 0xe0,
        0x20, 0xc5, 0xc7,
        0x2a,
        0xa5, 0xe0,
        0xc9, 0x10,
        0xf0, 0xd2,
        0x4c, 0x44, 0xe7,
        0x4c, 0xfd, 0xe6
    );

    vpatch(rom, 0x0bc6c, 68,
     /* NAME           h     u     r     l     e     d           a        */
        0xf8, 0x5f, 0x11, 0x1e, 0x1b, 0x15, 0x0e, 0x0d, 0x5f, 0x0a, 0x5f,
    /*     t     o     r     c     h           a     t           t     h  */
        0x1d, 0x18, 0x1b, 0x0c, 0x11, 0x5f, 0x0a, 0x1d, 0x5f, 0x1d, 0x11,
    /*     e        ENMY     !   END                                      */
        0x0e, 0x5f, 0xf4, 0x4c, 0xfc,
    /*  NAME           s     p     r     i     n     k     l     e     d  */
        0xf8, 0x5f, 0x1c, 0x19, 0x1b, 0x12, 0x17, 0x14, 0x15, 0x0e, 0x0d,
    /*           f     a     i     r     y           w     a     t     e  */
        0x5f, 0x0f, 0x0a, 0x12, 0x1b, 0x22, 0x5f, 0x20, 0x0a, 0x1d, 0x0e,
    /*     r           o     n           t     h     e        ENMY     .  */
        0x1b, 0x5f, 0x18, 0x17, 0x5f, 0x1d, 0x11, 0x0e, 0x5f, 0xf4, 0x47,
    /*  END                                                               */
        0xfc,

        /* pad the end */
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff);

}

/**
 * Generates a new monster attack pattern with constraints:
 *
 * No 75% DL2 fire
 * No 75% Sleep
 */
static uint8_t attack_pattern() {
    uint8_t pattern;

    pattern = (uint8_t)mt_rand(0, 255); /* random pattern */
    if ((pattern & 0x0f) == (MOVE2_FIRE2 | MOVE2_75_PERCENT)) {
        /* 75% DL2 fire, try again... */
        pattern &= 0xfc; /* clear move 2 chance */
        pattern |= (uint8_t)mt_rand(0,2); /* new move 2 chance */
    }
    if ((pattern & 0xf0) == (MOVE1_SLEEP | MOVE1_75_PERCENT)) {
        /* 75% sleep, try again... */
        pattern &= 0xcf; /* clear move 1 chance */
        pattern |= (uint8_t)(mt_rand(0,2) << 4); /* new move 1 chance */
    }
    return pattern;
}

/**
 * Randomizes enemy attack patterns (spells)
 *
 * @param rom The rom struct
 */
static void randomize_attack_patterns(dw_rom *rom)
{
    int i;
    dw_enemy *enemy;

    if (!RANDOMIZE_PATTERNS(rom))
        return;

    printf("Randomizing enemy attack patterns...\n");

    for (i=SLIME; i <= RED_DRAGON; i++) {
        enemy = rom->enemies + i;
        enemy->pattern = 0;
        if (mt_rand_bool()) {
            enemy->pattern = attack_pattern();
        }
        if (enemy->pattern & 0x33) {
            /* base max resistance on enemy index */
            enemy->s_ss_resist &= 0xf0;
            enemy->s_ss_resist |= mt_rand(0, i/4);
        } else {
            /* no spells, max out resistance */
            enemy->s_ss_resist |= 0xf;
        }
    }
}

/**
 * Randomizes the music of the game
 *
 * @param rom The rom struct
 */
static void randomize_music(dw_rom *rom)
{
    int i;

    if (!RANDOMIZE_MUSIC(rom))
        return;

    printf("Randomizing game music...\n");

    uint8_t choices[] = {0x1, 0x1, 0x1, 0x2, 0x2, 0x2, 0x3, 0x3, 0x3, 0x4, 0x4,
                         0x4, 0x5, 0x5, 0x5, 0xf, 0xf, 0xf, 0x10, 0x10, 0x10,
                         0xe, 0xe, 0xe, 0x6, 0x7, 0x8, 0x9, 0xa, 0xb, 0xc, 0xd};


    for (i=0; i < 29; i++) {
        rom->music[i] = choices[mt_rand(0, sizeof(choices)-1)];
    }
    /* Randomize town music, Dragonlord battle music, normal battle music, and cave music */
    vpatch(rom, 0xd8f7, 1, choices[mt_rand(0, sizeof(choices)-1)]);
    vpatch(rom, 0xe4e8, 1, choices[mt_rand(0, sizeof(choices)-1)]);
    vpatch(rom, 0xe4ec, 1, choices[mt_rand(0, sizeof(choices)-1)]);
    vpatch(rom, 0xd0a0, 1, choices[mt_rand(0, sizeof(choices)-1)]);
    vpatch(rom, 0xd49d, 1, choices[mt_rand(0, sizeof(choices)-1)]);
    vpatch(rom, 0xdf01, 1, choices[mt_rand(0, sizeof(choices)-1)]);
}

/**
 * Disables most of the game music
 *
 * @param rom The rom struct
 */
static void disable_music(dw_rom *rom)
{
    if (!DISABLE_MUSIC(rom))
        return;

    printf("Disabling game music...\n");

    memset(rom->music, 0, 29);
    vpatch(rom, 0xd8f7, 1, 0); // Disable town music after staying at inn.
    vpatch(rom, 0xe4e8, 1, 0); // Disable Dragonlord battle music
    vpatch(rom, 0xe4ec, 1, 0); // Disable normal battle music
    vpatch(rom, 0xd0a0, 1, 0); // Disable cave music after rescuing princess gwaelin.
    vpatch(rom, 0xd49d, 1, 0); // Disable castle music after saying you love the princess.
    vpatch(rom, 0xdf01, 1, 0); // Disable overworld music after using rainbow drop.
}

/**
 * Sets an overworld zone to the specified zone
 *
 * @param rom The rom struct
 * @param x The x index of the zone to set (max 7)
 * @param y The y index of the zones to set (max 7)
 * @param value The zone number to set the specified zone to
 */
static inline void set_ow_zone(dw_rom *rom, uint8_t x, uint8_t y, uint8_t value)
{
    int zone_index;

    if (x > 7 || y > 7)  /* ignore anything out of bounds. */
        return;

    zone_index = x + 8 * y;
    if (zone_index & 1) {
        rom->zone_layout[zone_index / 2] &= 0xf0;  /* second nybble */
        rom->zone_layout[zone_index / 2] |= (0xf & value);
    } else {
        rom->zone_layout[zone_index / 2] &= 0xf; /* first nybble */
        rom->zone_layout[zone_index / 2] |= (0xf0 & (value << 4));
    }
}

/**
 * Randomizes the zone layout of the overworld.
 *
 * @param rom The rom struct
 */
static void randomize_zone_layout(dw_rom *rom)
{
    int i;
    dw_warp *tantegel = &rom->map.warps_from[WARP_TANTEGEL];

    printf("Randomizing enemy zone layout...\n");

    for (i=0; i < 32; i++) {
        rom->zone_layout[i] = 0;
        rom->zone_layout[i] |= mt_rand(3, 15) << 4;
        rom->zone_layout[i] |= mt_rand(3, 15);
    }

    /* set up zones around tantegel */
    set_ow_zone(rom, tantegel->x / 15, tantegel->y / 15, 0);
    set_ow_zone(rom, (tantegel->x) / 15 - 1, tantegel->y / 15, 1);
    set_ow_zone(rom, (tantegel->x) / 15 + 1, tantegel->y / 15, 1);
    set_ow_zone(rom, tantegel->x / 15, (tantegel->y) / 15 - 1, 2);
    set_ow_zone(rom, tantegel->x / 15, (tantegel->y) / 15 + 1, 2);
}

/**
 * Randomizes the enemies present in each zone.
 *
 * @param rom The rom struct
 */
static void randomize_zones(dw_rom *rom)
{
    int i, zone;
    const dw_enemies charlock_enemies[10] = { WEREWOLF, GREEN_DRAGON, STARWYVERN,
            WIZARD, AXE_KNIGHT, BLUE_DRAGON, STONEMAN, ARMORED_KNIGHT,
            RED_DRAGON, GOLEM };

    if (!RANDOMIZE_ZONES(rom))
        return;

    printf("Randomizing monsters in enemy zones...\n");

    zone = 0;  /* tantegel zone */
    for (i=0; i < 5; i++) {
        rom->zones[zone * 5 + i] = mt_rand(SLIME, SCORPION);
    }

    for (zone=1; zone <= 2; zone++) { /* tantegel adjacent zones */
        for (i=0; i < 5; i++) {
            rom->zones[zone * 5 + i] = mt_rand(SLIME, WOLF);
        }
    }

    for (zone=3; zone <= 15; zone++) { /* overworld/hybrid zones */
        for (i=0; i < 5; i++) {
            rom->zones[zone * 5 + i] = mt_rand(SLIME, RED_DRAGON);
        }
    }

    for (zone=16; zone <= 18; zone++) { /* charlock */
        for (i=0; i < 5; i++) {
            rom->zones[zone * 5 + i] = charlock_enemies[mt_rand(0, 9)];
        }
    }
    zone = 19;  /* swamp cave */
    for (i=0; i < 5; i++) {
        rom->zones[zone * 5 + i] = mt_rand(SLIME, RED_DRAGON);
    }
}

/**
 * Determines whether a shop contains a certain item.
 *
 * @param shop_start A pointer to the beginning of the shop items.
 * @param shop_end A pointer to the end of the shop items.
 * @param item The item to search for.
 * @return A boolean indicating whether the shop has the item or not.
 */
static BOOL shop_contains(uint8_t *shop_start, uint8_t *shop_end,
    dw_shop_item item)
{
    while (shop_start < shop_end) {
        if (*shop_start == item) {
            return TRUE;
        }
        shop_start++;
    }
    return FALSE;

}

/**
 * Randomizes the items in the weapon shops.
 *
 * @param rom The rom struct
 */
static void randomize_shops(dw_rom *rom)
{
    uint8_t *shop_start, *shop_item;
    int i, j, six_item_shop;
    dw_shop_item items[] = {
            BAMBOO_POLE, CLUB, COPPER_SWORD, HAND_AXE, BROAD_SWORD, FLAME_SWORD,
            CLOTHES, LEATHER_ARMOR, CHAIN_MAIL, HALF_PLATE, FULL_PLATE,
            MAGIC_ARMOR, SMALL_SHIELD, LARGE_SHIELD, SILVER_SHIELD,
    };
    size_t item_counts[] = { 5, 5, 5, 5, 5, 5, 5 };

    if (!RANDOMIZE_SHOPS(rom))
        return;

    printf("Randomizing weapon shop inventory...\n");

    /* make one weapon shop have 6 items */
    item_counts[mt_rand(0, 6)] = 6;
    shop_item = rom->weapon_shops;

    for (i=0; i < 7; i++) {
        shop_start = shop_item;
        for (j=0; j < item_counts[i]; j++) {
            while(shop_contains(shop_start, shop_item,
                    *shop_item = items[mt_rand(0, 14)])) {}
            shop_item++;
        }
        qsort(shop_start, item_counts[i], sizeof(uint8_t), &compare);
        *(shop_item++) = SHOP_END;
    }
}

/**
 * Randomizes the player's stat growth
 *
 * @param rom The rom struct
 */
static void randomize_growth(dw_rom *rom)
{
    int i;
    dw_stats *stats;
    uint8_t str[30];
    uint8_t agi[30];
    uint8_t  mp[30];
    uint8_t  hp[30];

    if (RANDOMIZE_GROWTH(rom)) {

        printf("Randomizing stat growth...\n");

        for (i=0; i < 30; i++) {
            hp[i] =  (uint8_t)polyfit(mt_rand_double_ranged(0,30), &hero_hp_fac);
            mp[i] =  (uint8_t)polyfit(mt_rand_double_ranged(0,30), &hero_mp_fac);
            str[i] = (uint8_t)polyfit(mt_rand_double_ranged(0,30), &hero_str_fac);
            agi[i] = (uint8_t)polyfit(mt_rand_double_ranged(0,30), &hero_agi_fac);
        }

        qsort(hp,  30, sizeof(uint8_t), &compare);
        qsort(mp,  30, sizeof(uint8_t), &compare);
        qsort(str, 30, sizeof(uint8_t), &compare);
        qsort(agi, 30, sizeof(uint8_t), &compare);

        /* Give a little hp boost for swamp mode */
        for (i=0; i < 30; i++) {
            stats = &rom->stats[i];
            stats->str = str[i];
            stats->agi = agi[i];
            stats->hp =  hp[i];
            stats->mp =  mp[i];
        }
    }

    if (BIG_SWAMP(rom)) {
        printf("Boosting early HP...\n");
        rom->stats[0].hp += 10;
        for (i=1; i < 10; i++) {
            stats = &rom->stats[i];
            stats->hp = MAX(rom->stats[i-1].hp, stats->hp + 10 - i);
        }
        for (i=10; i < 30; i++) {
            stats = &rom->stats[i];
            stats->hp = MAX(rom->stats[i-1].hp, stats->hp);
        }
    }
}

/**
 * Randomizes the order in which spells are learned.
 * * @param rom The rom struct
 */
static void randomize_spells(dw_rom *rom)
{
    int i, j, tmp;
    dw_stats *stats;

    if (!RANDOMIZE_SPELLS(rom))
        return;

    printf("Randomizing spell learning...\n");

    /* choose levels for each spell */
    for (i=HEAL; i <= HURTMORE; i++) {
        rom->new_spells[i].level = mt_rand(1, 16);
    }

    if (HEAL_HURT_B4_MORE(rom)) {
        if (rom->new_spells[HEAL].level > rom->new_spells[HEALMORE].level) {
            tmp = rom->new_spells[HEAL].level;
            rom->new_spells[HEAL].level = rom->new_spells[HEALMORE].level;
            rom->new_spells[HEALMORE].level = tmp;
        }
        if (rom->new_spells[HURT].level > rom->new_spells[HURTMORE].level) {
            tmp = rom->new_spells[HURT].level;
            rom->new_spells[HURT].level = rom->new_spells[HURTMORE].level;
            rom->new_spells[HURTMORE].level = tmp;
        }
    }

    for (i=0; i < 30; i++) {
        stats = &rom->stats[i];
        stats->spells = 0;
        for (j=HEAL; j <= HURTMORE; j++) {
            if ((j == REPEL && PERMANENT_REPEL(rom)) ||
                (j == HURTMORE && NO_HURTMORE(rom))) {
                rom->new_spells[j].level = 31; // this spell is never learned.
                continue;
            }
            /* spell masks are in big endian format */
            if (rom->new_spells[j].level <= i+1) {
                stats->spells |= 1 << (j + 8) % 16;
            }
        }
        if (stats->spells) {
            stats->mp = MAX(stats->mp, 6);
        }
    }
}

/**
 * Go directly to the Dragonlord. Do not pass Go, do not collect 200 Gold.
 *
 * @param rom The rom struct.
 */
static void short_charlock(dw_rom *rom)
{
    if (!SHORT_CHARLOCK(rom))
        return;

    printf("Shortening Charlock Castle...\n");

    rom->map.warps_to[WARP_CHARLOCK].map = CHARLOCK_THRONE_ROOM;
    rom->map.warps_to[WARP_CHARLOCK].x = 10;
    rom->map.warps_to[WARP_CHARLOCK].y = 29;
    rom->map.warps_from[WARP_CHARLOCK_CHEST].map = CHARLOCK_THRONE_ROOM;
    rom->map.warps_from[WARP_CHARLOCK_CHEST].x = 18;
    rom->map.warps_from[WARP_CHARLOCK_CHEST].y = 9;
    vpatch(rom, 0x4c4, 1, 0x76); /* Add some downward stairs near the chests */
}

/**
 * Implements changes needed for when Charlock doesn't require the Rainbow Drop
 * to enter.
 *
 * @param rom The rom struct.
 */
static void open_charlock(dw_rom *rom)
{
    int i;

    if (!OPEN_CHARLOCK(rom))
        return;

    printf("Opening Charlock and removing quest items...\n");
    /* remove the quest items since we won't need them */
    for (i=0; i <= 31; ++i) {
        if (is_quest_item(rom->chests[i].item)) {
            rom->chests[i].item = GOLD;
        }
    }
    if (is_quest_item(rom->token->item))
            rom->token->item = 0;
    if (is_quest_item(rom->armor->item))
            rom->armor->item = 0;
    if (is_quest_item(rom->flute->item))
            rom->flute->item = 0;
}

/**
 * Changes the amount of experience and gold received from each enemy. Most
 * values are taken from the SFC & GBC remakes.
 *
 * @param rom The rom struct
 */
static void update_drops(dw_rom *rom)
{
    int i;

    printf("Modifying enemy drops...\n");
    const uint16_t remake_xp[] = {
              1,   2,   3,   4,   8,  12,  16,  14,  15,  18,  20,  25,  28,
             31,  40,  42, 255,  47,  52,  58,  58,  64,  70,  72, 350,   6,
             78,  83,  90,  95, 135, 105, 120, 130, 180, 155, 172, 350, 0, 0
    };
    const uint16_t remake_gold[] = {
              2,   4,   6,   8,  16,  20,  25,  21,  19,  30,  25,  42,  50,
             48,  60,  62,   6,  75,  80,  95, 110, 105, 110, 120,  10, 650,
            150, 135, 148, 155, 160, 169, 185, 165, 150, 148, 152, 143, 0, 0
    };

    for (i=SLIME; i <= DRAGONLORD_2; i++) {
        rom->enemies[i].xp =   remake_xp[i];
        rom->enemies[i].gold = remake_gold[i];
    }
}

/**
 * Changes the MP required for each spell to match the SFC & GBC remakes.
 * @param rom The rom struct
 */
static void update_mp_reqs(dw_rom *rom)
{
    int i;
    const uint8_t mp_reqs[10] = {3, 2, 2, 2, 2, 6, 8, 2, 8, 5};

    printf("Changing MP requirements for spells...\n");
    for (i=0; i < 10; i++) {
        rom->mp_reqs[i] = mp_reqs[i];
    }
}

/**
 * Lowers the amount of XP required for each level, based on user specified
 * flags
 *
 * @param rom The rom struct
 */
static void lower_xp_reqs(dw_rom *rom)
{
    int i;

    if (FAST_XP(rom)) {
        printf("Changing required experience to 75%% of normal...\n");
        for (i=0; i < 30; i++) {
            rom->xp_reqs[i] = rom->xp_reqs[i] * 3 / 4;
        }
    } else if (VERY_FAST_XP(rom)) {
        printf("Changing required experience to 50%% of normal...\n");
        for (i=0; i < 30; i++) {
            rom->xp_reqs[i] = rom->xp_reqs[i] / 2;
        }
    }
}

/**
 * Changes the max hit points of each enemy. Most of these values are taken from
 * the SFC & GBC remakes.
 *
 * @param rom The rom struct
 */
static void update_enemy_hp(dw_rom *rom)
{
    int i;
    const uint8_t remake_hp[] = {
              2,   3,   5,   7,  12,  13,  13,  22,  23,  20,  16,  24,  28,
             18,  33,  39,   3,  33,  37,  35,  44,  37,  40,  40, 153,  50,
             47,  48,  38,  70,  72,  74,  65,  67,  98, 135,  99, 106, 100, 165
    };
    for (i=SLIME; i <= DRAGONLORD_2; i++) {
        rom->enemies[i].hp =   remake_hp[i];
    }
    rom->enemies[DRAGONLORD_2].hp -= mt_rand(0, 15);
}

/**
 * Centers new text on the title screen
 *
 * @param pos A pointer to the rom data where the patched in text should go.
 *      This should be the beginning of a line.
 * @param text The text to place on the screen. Keep in mind the title screen
 *      character set does not contain lower case characters.
 * @return A pointer to the end of the patch.
 */
static uint8_t *center_title_text(uint8_t *pos, const char *text)
{
    uint8_t len, space, dw_text[34];

    strncpy((char*)dw_text, text, 33);

    ascii2dw_title(dw_text);
    len = strlen(text);
    space = 32 - len;
    if (space)
        pos = pvpatch(pos, 3, 0xf7, space/2, 0x5f);
    pos = ppatch(pos, len, dw_text);
    if (space)
        pos = pvpatch(pos, 4, 0xf7, (space+1)/2, 0x5f, 0xfc);

    return pos;
}

/**
 * Adds extra padding to the title screen if needed to make sure we fill the
 * space.
 *
 * @param pos The current position of the title screen pointer.
 * @param end The end of the title screen space.
 * @param reserved Space to reserve for other text.
 * @return
 */
static uint8_t *pad_title_screen(uint8_t *pos, uint8_t *end, int reserved)
{
    char text[32];
    int needed;

    needed = MIN(end - pos - reserved - 1, 32);
    if (needed < 0) {
        printf("An unexpected error occurred while updating the title "
                       "screen!\n");
    }
    memset(text, 0x5f, 32);
    if (needed < 32) {
        needed -= 3;
        pos = pvpatch(pos, 3, 0xf7, 32 - needed, 0x5f);
    }
    pos = ppatch(pos, needed, (uint8_t*)text);
    pos = pvpatch(pos, 1, 0xfc);
    return pos;
}

/**
 * Updates the title screen with the randomizer version, flags, and seed number.
 *
 * @param rom The rom struct
 */
static void update_title_screen(dw_rom *rom)
{
    unsigned char text[33];
    uint8_t *pos, *end;

//    pos = rom->title_text;
//    end = &rom->title_text + 143;
    pos = &rom->content[0x3f26];
    end = &rom->content[0x3fb5];
    text[32] = '\0';

    printf("Updating title screen...\n");
    pos = pvpatch(pos, 4, 0xf7, 32, 0x5f, 0xfc); /* blank line */
    pos = center_title_text(pos, "RANDOMIZER");  /* RANDOMIZER text */
    pos = pvpatch(pos, 4, 0xf7, 32, 0x5f, 0xfc); /* blank line */
    pos = center_title_text(pos, VERSION);       /* version number */

    pos = pvpatch(pos, 4, 0xf7, 32, 0x5f, 0xfc); /* blank line */
    pos = pvpatch(pos, 4, 0xf7, 32, 0x5f, 0xfc); /* blank line */
    pos = pvpatch(pos, 4, 0xf7, 32, 0x5f, 0xfc); /* blank line */
    pos = pvpatch(pos, 4, 0xf7, 32, 0x5f, 0xfc); /* blank line */

    pos = center_title_text(pos, (const char *)rom->flags_encoded); /* flags */
    snprintf((char *)text, 33, "%"PRIu64, rom->seed);

     /* blank line */
    pos = pad_title_screen(pos, end, 15 + strlen((const char *)text));
    pos = center_title_text(pos, (const char*)text);  /* seed number */
    pos = pad_title_screen(pos, end, 4); /* blank line */
    pos = pad_title_screen(pos, end, 0); /* blank line */

    // 0x3fc5
}


/**
 * Patches in functionality for the fighter's ring (+2 ATK)
 *
 * @param rom The rom struct
 */
static void dwr_fighters_ring(dw_rom *rom)
{

    printf("Fixing the fighter's ring...\n");
    /* fighter's ring fix */
    vpatch(rom, 0xf0fc, 4, 0x20, 0x7d, 0xff, 0xea);
    vpatch(rom, 0xff7d, 17,
        /* ff7d: */
        0x85, 0xcd,      /* STA $00CD  ; replaces code removed from $F00C     */
        0xa5, 0xcf,      /* LDA $00CF  ; load status bits                     */
        0x29, 0x20,      /* AND #$20   ; check bit 6 (fighter's ring)         */
        0xf0, 0x06,      /* BEQ $FF8B                                         */
        0xa5, 0xcc,      /* LDA $00CC  ; load attack power                    */
        0x69, 2,         /* ADC #$02   ; add fighter's ring power.            */
        0x85, 0xcc,      /* STA $00CC  ; store new attack power               */
        /* ff8b: */
        0x4c, 0x54, 0xff /* JMP $FF54  ; jump to next section                 */
    );
    vpatch(rom, 0xff54, 3,
        /* ff54: */
        0xa5, 0xcf,      /* LDA $00CF   ; replaces code removed from $F00E    */
        0x60             /* RTS                                               */
    );
}

/**
 * Patches in functionality for the death necklace (+10ATK, -25%HP)
 *
 * @param rom The rom struct
 */
static void dwr_death_necklace(dw_rom *rom)
{

    if (!DEATH_NECKLACE(rom))
        return;

    printf("Adding functionality to the death necklace...\n");

    vpatch(rom, 0xff54, 31,
            /* ff54: */
            0x24, 0xcf,  /* AND #$80  ; check bit 8 (death necklace)       */
            0x10, 0x18,  /* BPL $FF71                                      */
            0xa5, 0xca,  /* LDA $00CA ; load max HP                        */
            0x46, 0xca,  /* LSR $00CA ; divide max HP by 4                 */
            0x46, 0xca,  /* LSR $00CA                                      */
            0x38,        /* SEC       ; Set the carry bit (no borrow)      */
            0xe5, 0xca,  /* SBC $00CA ; subtract the result                */
            0x85, 0xca,  /* STA $00CA ; rewrite max HP                     */
            0xc5, 0xc5,  /* CMP $00C5 ; compare to current HP              */
            0xb0, 0x02,  /* BCS $FF69                                      */
            0x85, 0xc5,  /* STA $00C5 ; set current HP to max HP           */
            /* ff69: */
            0x18,        /* CLC       ; Clear the carry bit                */
            0xa5, 0xcc,  /* LDA $00CC ; load attack power                  */
            0x69, 0x0a,  /* ADD #$0A  ; add 10                             */
            0x85, 0xcc,  /* STA $00CC ; rewrite attack power               */
            /* ff70: */
            0xa5, 0xcf,  /* LDA $00CF ; replaces code removed from $F00E   */
            0x60         /* RTS                                            */
        );
}

/**
 * Inserts code to handle 2 byte xp and gold values
 *
 * @param rom The rom struct
 */
static void support_2_byte_xp_gold(dw_rom *rom)
{
    int ss_lvl = 1; /* level to not scale metal slime xp */

    if (SCALED_SLIMES(rom)) {
        printf("Reducing early metal rewards...\n");
        ss_lvl = 8;
    }

    /* Note that this code won't work properly if scaled slimes is on and
     * metal slimes are set to give more than 255XP. If that is changed then
     * this code will need to be reworked.
     */
//     vpatch(rom, 0x0ea0a, 84,
//             0xad, 0x06, 0x01,  /*   lda $0106   ; Load low byte of XP        */
//             0x85, 0x00,        /*   sta $00     ; Store at $00               */
//             0xad, 0x07, 0x01,  /*   lda $0107   ; Load High byte of XP       */
//             0x85, 0x01,        /*   sta $01     ; Store at $01               */
//             0xa5, 0xe0,        /*   lda $e0     ; Load Monster ID            */
//             0xc9, 0x10,        /*   cmp #16     ; Is it a metal slime?       */
//             0xd0, 0x0d,        /*   bne +       ; No, skip to loading XP     */
//             0xa5, 0xc7,        /*   lda $c7     ; Load player level          */
//             0xc9, ss_lvl,      /*   cmp #ss_lvl ; Is it 8 or higher?         */
//             0xb0, 0x07,        /*   bcs +       ; Yes, skip to print XP      */
//             0x0a,              /*   asl         ;                            */
//             0x0a,              /*   asl         ; Multiply level by 32 and   */
//             0x0a,              /*   asl         ; use this value for XP      */
//             0x0a,              /*   asl         ; gained                     */
//             0x0a,              /*   asl         ;                            */
//             0x85, 0x00,        /*   sta $00     ; Store XP at $00            */
//             0x20, 0xcb, 0xc7,  /* + jsr $c7cb   ; Print XP gained            */
//             0xef,              /*   hex ef                                   */
//             0xa5, 0x00,        /*   lda $00     ; Load Low byte of XP        */
//             0x18,              /*   clc                                      */
//             0x65, 0xba,        /*   adc $ba     ; Add it to low byte of      */
//             0x85, 0xba,        /*   sta $ba     ; player XP                  */
//             0xa5, 0x01,        /*   lda $01     ; Load high byte of XP       */
//             0x65, 0xbb,        /*   adc $bb     ; Add it to low byte of      */
//             0x85, 0xbb,        /*   sta $bb     ; player XP                  */
//             0x90, 0x06,        /*   bcc +       ; No overflow, go to gold    */
//             0xa9, 0xff,        /*   lda #$ff    ; Overflow, set player XP    */
//             0x85, 0xba,        /*   sta $ba     ; to 65535                   */
//             0x85, 0xbb,        /*   sta $bb                                  */
//             0xad, 0x08, 0x01,  /* + lda $0108   ; Load low byte of gold      */
//             0x85, 0x00,        /*   sta $00     ; Store at $00               */
//             0xad, 0x09, 0x01,  /*   lda $0109   ; Load high byte of gold     */
//             0x85, 0x01,        /*   sta $01     ; Store at $01               */
//             0x20, 0xcb, 0xc7,  /*   jsr $c7cb   ; Print GOLD gained          */
//             0xf0,              /*   hex f0                                   */
//             0xa5, 0x00,        /*   lda $00     ; Load low byte of gold      */
//             0x18,              /*   clc                                      */
//             0x65, 0xbc,        /*   adc $bc     ; Add to low byte of player  */
//             0x85, 0xbc,        /*   sta $bc     ; gold                       */
//             0xa5, 0x01,        /*   lda $01     ; Load high byte of gold     */
//             0x65, 0xbd,        /*   adc $bd     ; Add to high byte of player */
//             0x85, 0xbd,        /*   sta $bd     ; gold                       */
//             0x90, 0x63, 0xea   /*   bcc $ea63   ; no overflow, continue      */
//     );

    vpatch(rom, 0x0ea0f,   77,
            0xad,  0x07,  0x01,
            0x85,  0x01,
            0xa5,  0xe0,
            0xc9,  0x10,
            0xd0,  0x0d,
            0xa5,  0xc7,
            0xc9,  ss_lvl,
            0xb0,  0x07,
            0x0a,
            0x0a,
            0x0a,
            0x0a,
            0x0a,
            0x85,  0x00,
            0x20,  0xcb,  0xc7,
            0xef,
            0xa5,  0x00,
            0x18,
            0x65,  0xba,
            0x85,  0xba,
            0xa5,  0x01,
            0x65,  0xbb,
            0x85,  0xbb, 
            0x90,  0x06, 
            0xa9,  0xff, 
            0x85,  0xba,
            0x85,  0xbb,
            0xad,  0x08,  0x01,
            0x85,  0x00, 
            0xad,  0x09,  0x01,
            0x85,  0x01,
            0x20,  0xcb,  0xc7, 
            0xf0,
            0xa5,  0x00,
            0x18,
            0x65,  0xbc,
            0x85,  0xbc,
            0xa5,  0x01, 
            0x65,  0xbd,
            0x85,  0xbd,
            0x90
        );
}

/**
 * Makes metal slimes always have a chance to run from the hero
 *
 * @param rom The rom struct
 */
static void scared_metal_slimes(dw_rom *rom)
{

    if (!SCARED_SLIMES(rom)) {
        return;
    }

    printf("Terrorizing the Metal Slimes...\n");
    /* having the carry set upon returning gives the enemy a chance to run */
    vpatch(rom, 0x0c4e8,   13,
            0xcd,  0x00,  0x01, /* CMP $0100 ; Replace the code from $EFBA */
            0xb0,  0x07,        /* BCS c4fa  ; Return, everything is fine. */
            0xa5,  0xe0,        /* LDA $E0   ; Load the enemy type         */
            0xc9,  0x10,        /* CMP #$10  ; Is it a metal slime?        */
            0xf0,  0x01,        /* BEQ c4fa                                */
            0x18,               /* CLC       ; It's not, enemy doesn't run */
            /* c4fa: */
            0x60                /* RTS                                     */
    );
    vpatch(rom, 0x0efba,    3,
            0x20,  0xe8,  0xc4 /* JSR $C4E8  ; Jump to the new code        */
    );
}

/**
 * Adds a new goal to give the princess a cursed belt to finish the game
 *
 * @param rom The rom struct
 */
static void cursed_princess(dw_rom *rom)
{

    if (!CURSED_PRINCESS(rom)) 
        return;

    printf("The princess shall be cursed...\n");

    /* new dialogue */
    vpatch(rom, 0x09f40,   39,
            /* O      h             m      y      ,             w      h      a */
            0x32,  0x11,  0x5f,  0x16,  0x22,  0x48,  0x5f,  0x20,  0x11,  0x0a,
            /* t             a             l      o      v      e      l      y */
            0x1d,  0x5f,  0x0a,  0x5f,  0x15,  0x18,  0x1f,  0x0e,  0x15,  0x22,
            /*        c      o      r      s      e      t      .             */
            0x5f,  0x0c,  0x18,  0x1b,  0x1c,  0x0e,  0x1d,  0x45,  0x52,  0x5f,
            0x5f,  0x5f,  0x5f,  0x5f,  0x5f,  0x5f,  0x5f,  0x5f,  0x5f);

    vpatch(rom, 0x0c4ce,  26,
            0x85,  0x0e,        /* STA $OE    Store the item Gwaelin took     */
            0x4c,  0x4b,  0xe0, /* JMP $E04B  Continue on                     */
            /* C4D0: */
            0x48,               /* PHA        ; Store register                */
            0xa5,  0x0e,        /* LDA $OE    ; Load the item Gwaelin took    */
            0xc9,  0x09,        /* CMP $09    ; It is a cursed belt?          */
            0xd0,  0x0a,        /* BNE C4E1   ; No, keep playing              */
            0x20,  0xcb,  0xc7, /* JSR $C7CB  ; Yes, Display dialogue         */
            0xa0,               /* .DB $A0    ; Oh my...                      */
            0x20,  0xfa,  0xdf, /* JSR $DFFA  ; Play the curse music          */
            0x4c,  0xb8,  0xcc, /* JMP $CCB8  ; Jump to the ending            */
            /* C4E1: */
            0x68,               /* PLA        ; Restore register and continue */
            0x4c,  0x33,  0xd4  /* JMP $D433                                  */
    );
    /* Change a few JSR addresses */
    vpatch(rom, 0x0d3dd,    2,  0xce,  0xc4);
    vpatch(rom, 0x0d40e,    2,  0xd3,  0xc4);
}

/**
 * Adds a new goal: bring the princess to the Dragonlord and join him to win
 *
 * @param rom The rom struct
 */
static void threes_company(dw_rom *rom)
{
    if (!THREES_COMPANY(rom))
        return;

    printf("Shall we join the Dragonlord?\n");

    /* new dialogue */
    vpatch(rom, 0x09f69,  141,
            0x50,  0x2c,  0x5f,  0x11,  0x0a,  0x1f,  0x0e,  0x5f,  0x1c,  0x18,
            0x16,  0x0e,  0x5f,  0x10,  0x1b,  0x0e,  0x0a,  0x1d,  0x5f,  0x12,
            0x0d,  0x0e,  0x0a,  0x1c,  0x5f,  0x1d,  0x18,  0x5f,  0x1c,  0x19,
            0x1b,  0x1e,  0x0c,  0x0e,  0x5f,  0x1e,  0x19,  0x5f,  0x1d,  0x11,
            0x12,  0x1c,  0x5f,  0x19,  0x15,  0x0a,  0x0c,  0x0e,  0x47,  0x5f,
            0x2c,  0x5f,  0x20,  0x0a,  0x1c,  0x5f,  0x1d,  0x11,  0x12,  0x17,
            0x14,  0x12,  0x17,  0x10,  0x5f,  0x18,  0x0f,  0x5f,  0x19,  0x1e,
            0x1d,  0x1d,  0x12,  0x17,  0x10,  0x5f,  0x12,  0x17,  0x5f,  0x1c,
            0x18,  0x16,  0x0e,  0x5f,  0x17,  0x0e,  0x20,  0x5f,  0x0c,  0x1e,
            0x1b,  0x1d,  0x0a,  0x12,  0x17,  0x1c,  0x48,  0x5f,  0x16,  0x0a,
            0x22,  0x0b,  0x0e,  0x5f,  0x0a,  0x5f,  0x11,  0x18,  0x1d,  0x5f,
            0x1d,  0x1e,  0x0b,  0x48,  0x5f,  0x0a,  0x5f,  0x0d,  0x12,  0x1c,
            0x0c,  0x18,  0x5f,  0x0b,  0x0a,  0x15,  0x15,  0x5f,  0x18,  0x1f,
            0x0e,  0x1b,  0x5f,  0x11,  0x0e,  0x1b,  0x0e,  0x45,  0x52,  0x5f,
            0x5f);
    vpatch(rom, 0x0d509,    8,
            0xea,               /* NOP       ; Don't reset the quest byte     */
            0xea,               /* NOP       ; So we know if Gwaelin is here  */
            0x85,  0xe4,        /* STA $E4   ; Original Code                  */
            0x20,  0xbc,  0xc4, /* JSR $C4C0 ; Jump to three's company code   */
            0xea                /* NOP                                        */
    );
    vpatch(rom, 0x0c4bc,  18,
            0xa5,  0xdf,        /* LDA $DF   ; Load the status bits           */
            0x29,  0x01,        /* AND #$01  ; Are we carrying Gwaelin?       */
            0xf0,  0x07,        /* BEQ $C4CD ; No, back to original code      */
            0x20,  0xcb,  0xc7, /* JSR $CBC7 ; Show dialogue                  */
            0xa1,               /* .DB $A1   ; I have some great ideas....    */
            0x4c,  0xb8,  0xcc, /* JMP $CCB8 ; Jump to the ending credits     */
            /* c4cd: */
            0x20,  0xcb,  0xc7, /* JSR $CBC7 ; Show dialoge                   */
            0xc3,               /* .DB $A1   ; Now take a long rest...        */
            0x60                /* RTS                                        */
    );
}

/**
 * Makes all caves visible without using radiant or a torch.
 *
 * @param rom The rom struct
 */
static void permanent_torch(dw_rom *rom)
{
    if (!PERMANENT_TORCH(rom))
        return;

    printf("Illuminating all the caves...\n");

    vpatch(rom, 0x2f18,    2,
            0xa9,  0x03         /* LDA #$03  ; Set light radius to 3 instead */
                                /*           ; of 1 when loading map          */
    );
    vpatch(rom, 0xca60,    2,
            0xc9, 0x03          /* CMP #$03  ; Don't decrease the light */
                                /*           ; radius if it's 3. */
    );
}

/**
 * Other various patches for gameplay, such as silver harp enemies, town and
 * dungeon map changes and moving some NPCs.
 *
 * @param rom The rom struct
 */
static void other_patches(dw_rom *rom)
{
    printf("Applying various other patches...\n");

    /* convert PRG1 to PRG0 */
    vpatch(rom, 0x03f9e, 2,  0x37,  0x32);
    vpatch(rom, 0x0af6c, 1,  0xef);

    /* open the fairy water shop in breconnary */
    vpatch(rom, 0x0074e, 1,  0x6f);
    vpatch(rom, 0x0076c, 1,  0x6f);
    vpatch(rom, 0x0077a, 1,  0x46);

    /* make search actually open a chest */
    vpatch(rom, 0xe1dc, 2, 0xfd, 0xe1);

    /* Have the jerk take the token along with staff & stones */
    vpatch(rom, 0x0d383,    2,  0xca,  0xbf); /* jump to new code below */
    vpatch(rom, 0x03fca,    8,
            0x20,  0x4b,  0xe0, /* JSR $E04B ; replace the rewritten code */
            0xa9,  0x07,        /* LDA #$07  ; remove token from inventory */
            0x4c,  0x4b,  0xe0  /* JMP $E04B ; jump to remove code and return */
    );

    /* make sure all forced encounters are never marked defeated */
    vpatch(rom, 0xe98d, 2, 0xea, 0xea);
    vpatch(rom, 0xe97b, 2, 0xea, 0xea);

    vpatch(rom, 0xde23, 10,  /* Changes the enemies summoned by the harp. */
        /* de23: */
        0x29, 0x7,  /* AND #$07    ; limit the random number to 0-7 */
        0xaa,  /* TAX         ; move the value to the X register */
        0xbd, 0x54, 0xf5,  /* LDA #F554,X ; load a monster from zone 1-2 */
        0xea,  /* NOP         ; fill in the rest with NOP */
        0xea,  /* NOP         ; */
        0xea,  /* NOP         ; */
        0xea   /* NOP         ; */
    );
    vpatch(rom, 0x42a, 1, 0x47);  /* add new stairs to the throne room */
    vpatch(rom, 0x2a9, 1, 0x45);  /* add new stairs to the 1st floor */
    vpatch(rom, 0x2c7, 1, 0x66);  /* add a new exit to the first floor */
    /* replace the charlock loop stairs with some for tantegel */
    vpatch(rom, 0xf437, 3, 5, 1, 8);
    vpatch(rom, 0xf4d0, 3, 4, 1, 7);
    set_dungeon_tile(rom, CHARLOCK_CAVE_6, 9, 0, DUNGEON_TILE_BRICK);
    /* Sets the encounter rate of Zone 0 to be the same as other zones. */
    vpatch(rom, 0xcebf, 3, 0x4c, 0x04, 0xcf);  /* skip over the zone 0 code */
    vpatch(rom, 0xe260, 1, 0);  /* set death necklace chance to 100% */
    vpatch(rom, 0xe74d, 1, 9);  /* buff the hurt spell */
    vpatch(rom, 0xdbc1, 1, 18);  /* buff the heal spell */
    /* fixing some annoying roaming npcs */
    vpatch(rom, 0x18ee, 1, 0xa7); /* move the stupid old man from the item shop */
    vpatch(rom, 0x90f,  1, 0x6f); /* quit ignoring the customers */
    vpatch(rom, 0x93c,  1, 0x6f); /* quit ignoring the customers */
    vpatch(rom, 0x17a2, 3, 0, 0, 0); /* delete roaming throne room guard */
    vpatch(rom, 0xf131, 2, 0x69, 0x03); /* Lock the stat build modifier at 3 */

    /* I always hated this wording */
    dwr_str_replace(rom, "The spell will not work", "The spell had no effect");
}

/**
 * Patches the credits to add contributors to the randomizer
 *
 * @param rom The rom struct
 */
static void credits(dw_rom *rom)
{
    printf("Patching credits...\n");

    vpatch(rom, 0x057e9,    1,  0x8b);
    vpatch(rom, 0x057f1,  272,  0x32,
            /* A few edits to save space */
            /* PRODUCE BY -> PRODUCER */
            0x35,

            0xfc, 0xe8, 0x21, /* Write PPU Address 2043 */
            /* K     O     I     C     H     I       */
            0x2e, 0x32, 0x2c, 0x26, 0x2b, 0x2c, 0x5f,
            /* N     A     K     A     M     U     R     A */
            0x31, 0x24, 0x2e, 0x24, 0x30, 0x38, 0x35, 0x24,

            0xfc, 0xc0, 0x23,  /* Write PPU Address 23C0 (start of attrs) */
            /* PPU Attributes */
            0xf7, 0x20, 0x0a,

            /* Next page */
            0xfd, 0x8b, 0x21, /* Write PPU Address 2043 */
            /* D     I     R     E     C     T     O     R */
            0x33, 0x35, 0x32, 0x27, 0x38, 0x26, 0x28, 0x35,

            0xfc, 0xe8, 0x21, /* Write PPU Address 2043 */
            /* Y     U     K     I     N     O     B     U       */
            0x3c, 0x38, 0x2e, 0x2c, 0x31, 0x32, 0x25, 0x38, 0x5f,
            /* C     H     I     D     A */
            0x26, 0x2b, 0x2c, 0x27, 0x24,

            0xfc, 0xc0, 0x23, /* Write PPU Address 2043 */
            /* PPU Attributes */
            0xf7, 0x20, 0x0f,

            /* Next page */
            0xfd, 0x43, 0x20, /* Write PPU Address 2043 */
            /* D     R     A     G     O     N        */
            0x27, 0x35, 0x24, 0x2a, 0x32, 0x31, 0x5f,
            /* W     A     R     R     I     O     R        */
            0x3a, 0x24, 0x35, 0x35, 0x2c, 0x32, 0x35, 0x5f,
            /* R     A     N     D     O     M     I     Z     E     R */
            0x35, 0x24, 0x31, 0x27, 0x32, 0x30, 0x2c, 0x3d, 0x28, 0x35,

            0xfc, 0xa3, 0x20,  /* Write PPU Address 20A3 */
            /* D     E     V     E     L     O     P     E     R */
            0x27, 0x28, 0x39, 0x28, 0x2f, 0x32, 0x33, 0x28, 0x35,

            0xfc, 0xb7, 0x20,  /* Write PPU Address 20b7 */
            /* M     C     G     R     E     W */
            0x30, 0x26, 0x2a, 0x35, 0x28, 0x3a,

            0xfc, 0x03, 0x21,  /* Write PPU Address 2103 */
            /* C     O     N     T     R     I     B     U     T     O     R */
            0x26, 0x32, 0x31, 0x37, 0x35, 0x2c, 0x25, 0x38, 0x37, 0x32, 0x35,
            /* S */
            0x36,

            0xfc, 0x14, 0x21,  /* Write PPU Address 2114 */
            /* G     A     M     E     B     O     Y     F     9 */
            0x2a, 0x24, 0x30, 0x28, 0x25, 0x32, 0x3c, 0x29, 0x09,

            0xfc, 0x54, 0x21,  /* Write PPU Address 2154 */
            /* C     A     I     T     S     I     T     H     2 */
            0x26, 0x24, 0x2c, 0x37, 0x36, 0x2c, 0x37, 0x2b, 0x02,

            0xfc, 0x9a, 0x21,  /* Write PPU Address 219A */
            /* D     V     J */
            0x27, 0x39, 0x2d,

            0xfc, 0xd3, 0x21,  /* Write PPU Address 21D3 */
            /* B     U     N     D     E     R     2     0     1     5 */
            0x25, 0x38, 0x31, 0x27, 0x28, 0x35, 0x02, 0x00, 0x01, 0x05,

            0xfc, 0x17, 0x22,  /* Write PPU Address 2217 */
            /* D     W     E     D     I     T */
            0x27, 0x3a, 0x28, 0x27, 0x2c, 0x37,

            0xfc, 0x63, 0x22,  /* Write PPU Address 2263 */
            /* S     P     R     I     T     E     S */
            0x36, 0x33, 0x35, 0x2c, 0x37, 0x28, 0x36,

            0xfc, 0x75, 0x22,  /* Write PPU Address 2275 */
            /* X     A     R     N     A     X     4     2 */
            0x3b, 0x24, 0x35, 0x31, 0x24, 0x3b, 0x04, 0x02,

            0xfc, 0xb3, 0x22,  /* Write PPU Address 22B3 */
            /* R     Y     U     S     E     I     S     H     I     N */
            0x35, 0x3c, 0x38, 0x36, 0x28, 0x2c, 0x36, 0x2b, 0x2c, 0x31,

            0xfc, 0xf7, 0x22,  /* Write PPU Address 22F7 */
            /* M     C     G     R     E     W */
            0x30, 0x26, 0x2a, 0x35, 0x28, 0x3a,

            0xfc, 0x32, 0x23,  /* Write PPU Address 2332 */
            /* M     I     S     T     E     R     H     O     M     E     S */
            0x30, 0x2c, 0x36, 0x37, 0x28, 0x35, 0x2b, 0x32, 0x30, 0x28, 0x36,

            0xfc, 0x72, 0x23,  /* Write PPU Address 2372 */
            /* I     N     V     E     N     E     R     A     B     L     E */
            0x2c, 0x31, 0x39, 0x28, 0x31, 0x28, 0x35, 0x24, 0x25, 0x2f, 0x28,
            0x5f, 0x5f,

            0xfc, 0xc0, 0x23,  /* Write PPU Address 23C0 (start of attrs) */
            /* PPU Attributes */
            /* 0xf7 means the next number is a count followed by a value */
            0xf7, 0x08, 0xff, 0xf7, 0x04, 0x55, 0xf7, 0x04, 0xaa, 0xf7,
            0x04, 0x55, 0xf7, 0x0c, 0xaa, 0xf7, 0x04, 0x55, 0xf7, 0x1c);
}

/**
 * Enables top to bottom wrapping of the menu cursor.
 *
 * @param rom The rom struct.
 */
static void dwr_menu_wrap(dw_rom *rom)
{
    if (!MENU_WRAP(rom))
        return;

    printf("Enabling menu cursor wrap-around...\n");

    /* implement up/down wraparound for most menus (from @gameboy9) */
    vpatch(rom, 0x69e0, 4,
        /* 69e0 */
        0x4c, 0xa0, 0xbe,  /* JMP $BEA0 */
        0xea               /* NOP */
    );
    vpatch(rom, 0x6a23, 4,
        /* 6a23: */
        0x4c, 0xcd, 0xbe,  /* JMP #BECD */
        0xea               /* NOP */
    );
    vpatch(rom, 0x7ea0, 45,
        /* 7e9e: */
        0xa5, 0x45,        /* LDA $0045 ; Load the map number */
        /* 7ea0: */
        0xf0, 0x57,        /* BEQ $7EFB ; if it's 0 (the title screen), jump to $7EFB */
        0xad, 0xe5, 0x64,  /* LDA $64E5 ; load ??? */
        0xc9, 0x04,        /* CMP #$04  ; compare to ??? */
        0xf0, 0x1e,        /* BEQ $7ED7 ; if equal branch to $7ED7 (return) */
        0x20, 0x30, 0xab,  /* JMP $AB30 ; jump to $AB30 */
        0xa5, 0xd9,        /* LDA $00D9 ; load cursor y position */
        0xd0, 0x14,        /* BNE $7ED4 ; if it's not 0, jump to $7ED4 */
        /* 7eb0: */
        0xad, 0xe5, 0x64,  /* LDA $64E5 ; load ? */
        0xe9, 0x03,        /* SBC #$03  ; subtract 3 */
        0x4a,              /* LSR       ; divide by 2 */
        0xe9, 0x00,        /* SBC #$00  ; subtract 0 (?) */
        0x85, 0xd9,        /* STA $00D9 ; rewrite map number (?) */
        0x0a,              /* ASL       ; multiply by 2 (?) */
        0x6d, 0xf3, 0x64,  /* ADC $64F3 ; add ??? */
        0x8d, 0xf3, 0x64,  /* STA $64F3 ; write to ??? */
        /* 7ec2: */
        0x4c, 0x27, 0xaa,  /* JMP $AA27 ; jump to $AA27 */
        /* 7ed4: */
        0x4c, 0xe4, 0xa9,  /* JMP $A9E4 ; jump to $A9E4 */
        /* 7ed7: */
        0x60,              /* RTS       ; return */
        0xa5, 0xd9,        /* LDA $00D9 ; load cursor y position */
        0x60               /* RTS       ; return */
    );
    vpatch(rom, 0x7ecd, 46,
        /* 7ecb: */
        0x48,              /* PHA       ; push A to stack */
        0xa5, 0x45,        /* LDA $0045 ; Load map number */
        0xf0, 0x30,        /* BEQ $7F00 ; if it's 0, branch to $7F00 */
        /* 7ed0: */
        0xad, 0xe5, 0x64,  /* LDA $64E5 ; load ??? */
        0xc9, 0x04,        /* CMP #$04  ; compare to 4 */
        0xf0, 0x20,        /* BEQ $7EFB ; if it's 4, branch to $7EFB */
        0x20, 0x30, 0xab,  /* JSR $AB30 ; jump to $AB30 */
        0x68,              /* PLA       ; pull A from stack */
        0xc5, 0xd9,        /* CMP $00D9 ; compare to map number */
        0xd0, 0x15,        /* BNE $7EF4 ; if they are equal, branch to $7EF4 */
        0xa9, 0x01,        /* LDA #$01  ; set A to 1 */
        /* 7ee1: */
        0x85, 0xd9,        /* STA $00D9 ; set map number to 1 */
        0xad, 0xf3, 0x64,  /* LDA $64F3 ; load ??? */
        0x29, 0x01,        /* AND #$01  ; limit to only the lowest bit */
        0xd0, 0x02,        /* BNE $7EEC ; if it's odd, branch to $7EEC */
        0x69, 0x02,        /* ADC #$02  ; add 2 */
        /* 7eec: */
        0x69, 0x01,        /* ADC #$01  ; add 1 */
        /* 7eee: */
        0x8d, 0xf3, 0x64,  /* STA $64F3 ; store at $64F3 */
        0x4c, 0xe4, 0xa9,  /* JMP $A9E4 ; jump to $A9E4 */
        /* 7ef4: */
        0x4c, 0x27, 0xaa,  /* JMP $AA27 ; jump to $AA27 */
        /* 7ef7: */
        0x68,              /* PLA       ; pull A from stack */
        /* 7ef8: */
        0x60               /* RTS       ; return */
    );
    vpatch(rom, 0x7efb, 7,
        /* 7efb: */
        0xa5, 0xd9,        /* LDA $00D9 ; load map number */
        0xf0, 0xfb,        /* BEQ $7EF8 ; if it's 0, branch to $7EF8 (return) */
        0x4c, 0xe4, 0xa9   /* JMP $A9E4 ; jump to $A9E4 */
    );
    vpatch(rom, 0x7f02, 8,
        /* 7f02: */
        0x68,              /* PLA       ; pull A from stack */
        0xc5, 0xd9,        /* CMP $00D9 ; compare to map number */
        0xf0, 0xf3,        /* BEQ $7EF8 ; if equal, jump to $7EF8 (return) */
        0x4c, 0x27, 0xaa   /* JMP $A9E4 ; jump to $A9E4 */
    );
}

static void spike_rewrite(dw_rom *rom)
{

    size_t i;
    const dw_enemies spike_enemies[] = {
        AXE_KNIGHT, BLUE_DRAGON, STONEMAN, ARMORED_KNIGHT, RED_DRAGON, GOLEM };
    dw_enemies spikes[] = {
        AXE_KNIGHT, GREEN_DRAGON, GOLEM, SLIME, SLIME, SLIME, SLIME, SLIME };

    if (!RANDOMIZE_ZONES(rom)) {
        for (i=0; i < 8; i++) {
            spikes[i] = spike_enemies[mt_rand(0, sizeof(spike_enemies)-1)];
        }
    }

    /* patch some jump addresses */
    vpatch(rom, 0x03263,    2,  0xca,  0xbf);
    vpatch(rom, 0x0335d,    2,  0xca,  0xbf);
    vpatch(rom, 0x033e9,    2,  0xca,  0xbf);
    vpatch(rom, 0x03515,    2,  0xca,  0xbf);
    
    /* save the previous tile you were on */
    vpatch(rom, 0x03fca,   11,
            0xa5,  0x3a,       /*   lda X_POS                                */
            0x85,  0xdc,       /*   sta _UNUSED_DC                           */
            0xa5,  0x3b,       /*   lda Y_POS                                */
            0x85,  0xdd,       /*   sta _UNUSED_DD                           */
            0x4c,  0xcc,  0xb1 /*   jmp $b1cc                                */
    );

    /* spike encounter table */
    vpatch(rom, 0x0cd82, 32,
          /* MAP    X     Y    MON */
            HAUKSNESS           , 25, 12, spikes[0],
            SWAMP_CAVE          ,  4, 15, spikes[1],
            CHARLOCK_THRONE_ROOM, 25, 22, spikes[2],
            NO_MAP,                0,  0, spikes[3],
            NO_MAP,                0,  0, spikes[4],
            NO_MAP,                0,  0, spikes[5],
            NO_MAP,                0,  0, spikes[6],
            NO_MAP,                0,  0, spikes[7] 
    );
    vpatch(rom, 0x0cd51, 39,
            0xa2, 0x00,       /*    ldx #$00                                 */
            0xa5, 0x45,       /* -  lda MAP_INDEX                            */
            0xdd, 0x82, 0xcd, /*    cmp spike_tab,x                          */
            0xd0, 0x14,       /*    bne +                                    */
            0xa5, 0x3a,       /*    lda X_POS                                */
            0xdd, 0x83, 0xcd, /*    cmp spike_tab+1,x                        */
            0xd0, 0x0d,       /*    bne +                                    */
            0xa5, 0x3b,       /*    lda Y_POS                                */
            0xdd, 0x84, 0xcd, /*    cmp spike_tab+2,x                        */
            0xd0, 0x06,       /*    bne +                                    */
            0xbd, 0x85, 0xcd, /*    lda spike_tab+3,x                        */
            0x4c, 0xdf, 0xe4, /*    jmp start_combat                         */
            0xe8,             /* +  inx                                      */
            0xe8,             /*    inx                                      */
            0xe8,             /*    inx                                      */
            0xe8,             /*    inx                                      */
            0xe0, 0x10,       /*    cpx #16                                  */
            0xd0, 0xdd,       /*    bne -                                    */
            0xf0, 0x2a        /*    beq +done                                */
    );
    vpatch(rom, 0x0e8d4, 51,
            0xa2, 0x00,       /*    ldx #$00                                 */
            0xa5, 0x45,       /* -  lda MAP_INDEX                            */
            0xdd, 0x82, 0xcd, /*    cmp spike_tab,x                          */
            0xd0, 0x20,       /*    bne +                                    */
            0xa5, 0x3a,       /*    lda X_POS                                */
            0xdd, 0x83, 0xcd, /*    cmp spike_tab+1,x                        */
            0xd0, 0x19,       /*    bne +                                    */
            0xa5, 0x3b,       /*    lda Y_POS                                */
            0xdd, 0x84, 0xcd, /*    cmp spike_tab+2,x                        */
            0xd0, 0x12,       /*    bne +                                    */
            0x20, 0xbb, 0xc6, /*    jsr b3_c6bb                              */
            0xa5, 0xdc,       /*    lda _UNUSED_DC                           */
            0x85, 0x3a,       /*    sta X_POS                                */
            0x85, 0x8e,       /*    sta X_POS_2                              */
            0xa5, 0xdd,       /*    lda _UNUSED_DD                           */
            0x85, 0x3b,       /*    sta Y_POS                                */
            0x85, 0x8f,       /*    sta Y_POS_2                              */
            0x4c, 0x97, 0xb0, /*    jmp $b097                                */
            0xe8,             /* +  inx                                      */
            0xe8,             /*    inx                                      */
            0xe8,             /*    inx                                      */
            0xe8,             /*    inx                                      */
            0xe0, 0x10,       /*    cpx #16                                  */
            0xd0, 0xd1,       /*    bne -                                    */
            0xf0, 0x39        /*    beq +done                                */
        );
}

static void sorted_inventory(dw_rom *rom)
{
    /* patch some jump addresses */
    vpatch(rom, 0x0d388,    2,  0x88,  0xc2);
    vpatch(rom, 0x0d3bf,    2,  0x88,  0xc2);
    vpatch(rom, 0x0d3e9,    2,  0x88,  0xc2);
    vpatch(rom, 0x0d72c,    2,  0x88,  0xc2);
    vpatch(rom, 0x0d875,    2,  0x88,  0xc2);
    vpatch(rom, 0x0e0f5,    2,  0x88,  0xc2);
//     vpatch(rom, 0x0e13c,    2,  0x88,  0xc2);
    vpatch(rom, 0x0e27a,    2,  0x88,  0xc2);

    vpatch(rom, 0x0c288, 61,
            0x20, 0x1b, 0xe0,  /*      jsr b3_e01b ; add the new item        */
            0xe0, 0x04,        /*      cpx #$04                              */
            0xf0, 0x35,        /*      beq ++                                */
            0xa2, 0x03,        /*      ldx #$03                              */
            0xb5, 0xc1,        /*  -   lda ITEMS,x                           */
            0x29, 0x0f,        /*      and #$0f                              */
            0x95, 0xa8,        /*      sta $a8,x                             */
            0xb5, 0xc1,        /*      lda ITEMS,x                           */
            0x4a,              /*      lsr a                                 */
            0x4a,              /*      lsr a                                 */
            0x4a,              /*      lsr a                                 */
            0x4a,              /*      lsr a                                 */
            0x95, 0xac,        /*      sta $ac,x                             */
            0xa9, 0x00,        /*      lda #$00                              */
            0x95, 0xc1,        /*      sta ITEMS,x                           */
            0xca, 0x10,        /*      dex                                   */
            0xeb,              /*      bpl -                                 */
            0xa9, 0x00,        /*      lda #$00                              */
            0xa2, 0x07,        /*  --  ldx #$07                              */
            0xd5, 0xa8,        /*  -   cmp $a8,x                             */
            0xb0, 0x04,        /*      bcs +                                 */
            0xb5, 0xa8,        /*      lda $a8,x                             */
            0x86, 0xa7,        /*      stx $a7                               */
            0xca,              /*  +   dex                                   */
            0x10, 0xf5,        /*      bpl -                                 */
            0xc9, 0x00,        /*      cmp #$00                              */
            0xf0, 0x0b,        /*      beq ++                                */
            0x20, 0x1b, 0xe0,  /*      jsr b3_e01b ; add the next item       */
            0xa6, 0xa7,        /*      ldx $a7                               */
            0xa9, 0x00,        /*      lda #$00                              */
            0x95, 0xa8,        /*      sta $a8,x                             */
            0xf0, 0xe4,        /*      beq --                                */
            0x60               /*  ++  rts                                   */
    );
}

/**
 * Enables various hacks to speed up gameplay, such as text and music changes.
 *
 * @param rom The rom struct.
 */
static void dwr_speed_hacks(dw_rom *rom)
{
    if (FAST_TEXT(rom)) {
        printf("Enabling fast text...\n");

        /* speed up the text */
        vpatch(rom, 0x7a33, 3, 0xea, 0xea, 0xea);
    }

    if (!SPEED_HACKS(rom))
        return;

    printf("Enabling speed hacks...\n");

    /* Following are some speed hacks from @gameboy9 */
    /* speed up encounter intros */
    vpatch(rom, 0xe40a, 3, 0xea, 0xea, 0xea);
    vpatch(rom, 0xe43d, 3, 0xea, 0xea, 0xea);
    vpatch(rom, 0xc52f, 3, 0xea, 0xea, 0xea);
    vpatch(rom, 0xef39, 1, 2);  /* speed up the player attack animation */
    vpatch(rom, 0xed35, 1, 3);  /* speed up the enemy attack animation */
    /* speed up the death music */
    vpatch(rom, 0x4d28, 1, 0x1);
    vpatch(rom, 0x4d2c, 1, 0x6);
    vpatch(rom, 0x4d3b, 1, 0x7);
    vpatch(rom, 0x4d3d, 1, 0x8);
    vpatch(rom, 0x4d3f, 1, 0x8);
    vpatch(rom, 0x4d41, 1, 0x8);
    vpatch(rom, 0x4d43, 1, 0x2);
    vpatch(rom, 0x4d45, 1, 0x2);
    vpatch(rom, 0x4d47, 1, 0x10);
    /* speed up the level up music */
    vpatch(rom, 0x462b, 1, 0xff),
    /* speed up the battle win music */
    vpatch(rom, 0x4714, 1, 1);
    vpatch(rom, 0x471a, 1, 1);
    vpatch(rom, 0x471c, 1, 1);
    vpatch(rom, 0x471e, 1, 1);
    /* speed up the fairy flute */
    vpatch(rom, 0x4ca1, 1, 1);
    vpatch(rom, 0x4ca3, 1, 1);
    vpatch(rom, 0x4cb5, 1, 1);
    vpatch(rom, 0x4cb7, 1, 1);
    vpatch(rom, 0x4cb9, 1, 1);
    vpatch(rom, 0x4cbd, 1, 1);
    vpatch(rom, 0x4cd2, 1, 1);
    vpatch(rom, 0x4cd4, 1, 1);
    vpatch(rom, 0x4cd6, 1, 1);
    vpatch(rom, 0x4cd8, 1, 1);
    vpatch(rom, 0x4cda, 1, 1);
    vpatch(rom, 0x4cdc, 1, 1);
    /* speed up the inn music */
    vpatch(rom, 0x46d4, 1, 1);
    vpatch(rom, 0x46d6, 1, 1);
    vpatch(rom, 0x46d8, 1, 1);
    vpatch(rom, 0x46da, 1, 1);
    vpatch(rom, 0x46dc, 1, 1);
    vpatch(rom, 0x46de, 1, 1);
    vpatch(rom, 0x46e0, 1, 1);
    vpatch(rom, 0x46e2, 1, 1);
    vpatch(rom, 0x46e4, 1, 1);
    vpatch(rom, 0x46e8, 1, 1);
    vpatch(rom, 0x46f1, 1, 1);
    vpatch(rom, 0x46f3, 1, 1);
    vpatch(rom, 0x46f5, 1, 1);
    vpatch(rom, 0x46f7, 1, 1);
    vpatch(rom, 0x46f9, 1, 1);
    vpatch(rom, 0x46fb, 1, 1);
    vpatch(rom, 0x46fd, 1, 1);
    vpatch(rom, 0x46ff, 1, 1);
    vpatch(rom, 0x4701, 1, 1);
    vpatch(rom, 0x4705, 1, 1);
    /* speed up spell casting */
    vpatch(rom, 0x435b, 1, 0x9e);
    vpatch(rom, 0xdb39, 6, 0xea, 0xea, 0xea, 0xea, 0xea, 0xea);
    vpatch(rom, 0xdb44, 9, 0xea, 0xea, 0xea, 0xea, 0xea, 0xea, 0xea, 0xea, 0xea);
}

/**
 * Removes the need for keys to open doors
 *
 * @param rom The rom struct
 */
static void no_keys(dw_rom *rom)
{
    int i;
    dw_chest *chest;

    if (!NO_KEYS(rom))
        return;

    printf("Removing the need for keys...\n");
    /* Don't require keys to open the door */
    vpatch(rom, 0xdc99, 2, 0xa9, 0x01);
    vpatch(rom, 0xdca8, 2, 0xea, 0xea);

    chest = rom->chests;
    for (i=0; i < CHEST_COUNT; i++) {
        if (chest->item == KEY)
            chest->item = GOLD;
        chest++;
    }

    /* remove the key shopkeepers */
    vpatch(rom, 0x1783, 3, 0, 0, 0);
    vpatch(rom, 0x185c, 3, 0, 0, 0);
    vpatch(rom, 0x181b, 3, 0, 0, 0);
}

/**
 * Modifications to repel:
 *   Allow repel to work in dungeons
 *   Allow permanent repel
 *
 * @param rom The rom struct
 */
static void repel_mods(dw_rom *rom)
{
    if (REPEL_IN_DUNGEONS(rom))
        printf("Making repel work in dungeons...\n");
        vpatch(rom, 0xcf20, 2, 0xa9, 0x01);

    if (PERMANENT_REPEL(rom)) {
        printf("Making repel permanent...\n");
        vpatch(rom, 0xcf26, 2, 0xa9, 0xff);
    }
}

/**
 * Updates spell names to names used in the later games and ports.
 *
 * @param rom The rom struct
 */
static void modern_spell_names(dw_rom *rom)
{
    if (!MODERN_SPELLS(rom))
        return;

    vpatch(rom, 0x07e5b,   67,
            /* S     I     Z     Z  */
            0x36, 0x2c, 0x3d, 0x3d, 0xff,
            /* S     N     O     O     Z     E  */
            0x36, 0x31, 0x32, 0x32, 0x3d, 0x28, 0xff,
            /* G     L     O     W  */
            0x2a, 0x2f, 0x32, 0x3a, 0xff,
            /* F     I     Z     Z     L     E   */
            0x29, 0x2c, 0x3d, 0x3d, 0x2f, 0x28, 0xff,
            /* E     V     A     C  */
            0x28, 0x39, 0x24, 0x26, 0xff,
            /* Z     O     O     M   */
            0x3d, 0x32, 0x32, 0x30, 0xff,
            /* P     R     O     T     E     C     T    */
            0x33, 0x35, 0x32, 0x37, 0x28, 0x26, 0x37, 0xff,
            /* M     I     D     H     E     A     L   */
            0x30, 0x2C, 0x27, 0x2b, 0x28, 0x24, 0x2f, 0xff,
            /* K     A     S     I     Z     Z     L     E   */
            0x2e, 0x24, 0x36, 0x2c, 0x3d, 0x3d, 0x2f, 0x28, 0xff,

            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
    );

    /* fix the repel end text */
    vpatch(rom, 0x08774,   26,
            /* P      R      O      T      E      C      T     */
            0x33,  0x35,  0x32,  0x37,  0x28,  0x26,  0x37,  0x5f,
            /* l      o      s      t             i      t      s        */
            0x15,  0x18,  0x1c,  0x1d,  0x5f,  0x12,  0x1d,  0x1c,  0x5f,
            /* e      f      f      e      c      t      .               */
            0x0e,  0x0f,  0x0f,  0x0e,  0x0c,  0x1d,  0x47,  0x5f,  0x5f
    );
}

/**
 * Updates the Cantlin NPC dialogue to reveal the new overworld item location.
 * If there is no overworld item to search for, the NPC will just give you
 * encouragement.
 *
 * @param rom The rom struct.
 */
static void dwr_token_dialogue(dw_rom *rom)
{
    uint8_t text1[24], text2[75];
    int dx, dy;

    if (!rom->token->item) {
        strcpy((char*)text1, "Thou must go and fight!");
        strcpy((char*)text2, "Go forth, descendant of Erdrick, "
                "I have complete faith in thy victory! ");
        ascii2dw(text1);
        patch(rom, 0xa228, 23, text1);
        vpatch(rom, 0xa288, 1, 0x53); /* replace .' with ' */
    } else {
        dx = rom->token->x - rom->map.warps_from[WARP_TANTEGEL].x;
        dy = rom->token->y - rom->map.warps_from[WARP_TANTEGEL].y;
//        strcpy((char*)text1, "Thou may go and search.");
        if (ABS(dx) > 100 || ABS(dy) > 100) {
            snprintf((char*)text2, 75, "From Tantegel Castle travel %3d "
                    "leagues to the %s and %3d %s/    ",
                    ABS(dy), (dy < 0) ? "north" : "south",
                    ABS(dx), (dx < 0) ? "west" : "east");
        } else {
            snprintf((char*)text2, 75, "From Tantegel Castle travel %2d "
                    "leagues to the %s and %2d to the %s/",
                    ABS(dy), (dy < 0) ? "north" : "south",
                    ABS(dx), (dx < 0) ? "west" : "east");
        }
    }
    ascii2dw(text2);
    patch(rom, 0xa242, 70, text2);
}

/**
 * What? No color?
 *
 * @param rom The rom struct.
 */
static void noir_mode(dw_rom *rom)
{
    if (!NOIR_MODE(rom))
        return;

    printf("Enabling noir mode...\n");
    /* Change the PPUMASK writes to disable color */
    vpatch(rom, 0x030b9,    1,  0x19);
    vpatch(rom, 0x03b5e,    1,  0x19);
    vpatch(rom, 0x0535d,    1,  0x19);
    vpatch(rom, 0x0c9e9,    1,  0x19);
    vpatch(rom, 0x0d39b,    1,  0x19);
    vpatch(rom, 0x0db4e,    1,  0x19);
    vpatch(rom, 0x0f84c,    1,  0x19);
    if (NO_SCREEN_FLASH(rom)) {
        vpatch(rom, 0x0d38e,    1,  0x19);
        vpatch(rom, 0x0db40,    1,  0x19);
    } else {
        vpatch(rom, 0x0d38e,    1,  0x00);
        vpatch(rom, 0x0db40,    1,  0x00);
    }
}

/**
 * Disables the screen flash when spells are cast
 *
 * @param rom The rom struct.
 */
static void no_screen_flash(dw_rom *rom)
{
    if (!NO_SCREEN_FLASH(rom))
        return;

    printf("Disabling screen flash for spells...\n");
    /* Change the PPUMASK writes to disable flashing during spells */
    vpatch(rom, 0x0d38e,    1,  0x18);
    vpatch(rom, 0x0db40,    1,  0x18);
}



/**
 * Writes the new rom out to disk.
 *
 * @param rom The rom struct
 * @param output_file The name of the file to write to.
 * @return A boolean indicating success or failure of rom creation.
 */
static BOOL dwr_write(dw_rom *rom, const char *output_file)
{
    FILE *output;

    output = fopen(output_file, "wb");
    if (!output) {
        fprintf(stderr, "Unable to open file '%s' for writing", output_file);
        return FALSE;
    }
    fwrite(rom->header, 1, ROM_SIZE, output);
    fclose(output);
    return TRUE;
}

static void check_structs()
{
    assert(sizeof(dw_enemy) == 16);
}

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
        const char *sprite_name, const char* output_dir)
{
    uint64_t crc;
    char output_file[1024];

    check_structs();

    snprintf(output_file, 1024, "%s/DWRando.%"PRIu64".%s.nes", output_dir, seed,
            flags);
    printf("Using seed# %"PRIu64"\n", seed);
    printf("Using flags %s\n", flags);

    mt_init(seed);
    dw_rom rom;
    if (!dwr_init(&rom, input_file, flags)) {
        return 0;
    }
    rom.seed = seed;

    /* Clear the unused code so we can make sure it's unused */
    memset(&rom.content[0xc288], 0xff, 0xc4f5 - 0xc288);

    do_chest_flags(&rom);
    map_generate_terrain(&rom);
    spike_rewrite(&rom);
    randomize_attack_patterns(&rom);
    randomize_zone_layout(&rom);
    randomize_zones(&rom);
    randomize_shops(&rom);
    randomize_growth(&rom);
    randomize_spells(&rom);
    update_drops(&rom);
    update_mp_reqs(&rom);
    lower_xp_reqs(&rom);
    update_enemy_hp(&rom);
    dwr_fighters_ring(&rom);
    dwr_death_necklace(&rom);
    dwr_menu_wrap(&rom);
    dwr_speed_hacks(&rom);
    open_charlock(&rom);
    dwr_token_dialogue(&rom);
    chaos_mode(&rom);
    short_charlock(&rom);
    no_keys(&rom);
    cursed_princess(&rom);
    threes_company(&rom);
    scared_metal_slimes(&rom);
    support_2_byte_xp_gold(&rom);
    torch_in_battle(&rom);
    repel_mods(&rom);
    permanent_torch(&rom);
    rotate_dungeons(&rom);
    sorted_inventory(&rom);
    other_patches(&rom);
    credits(&rom);

    modern_spell_names(&rom);
    randomize_music(&rom);
    disable_music(&rom);

    no_numbers(&rom);
    invisible_hero(&rom);
    invisible_npcs(&rom);

    crc = crc64(0, rom.content, 0x10000);

    update_title_screen(&rom);
    sprite(&rom, sprite_name);
    no_screen_flash(&rom);
    noir_mode(&rom);

    printf("Checksum: %016"PRIx64"\n", crc);
    if (!dwr_write(&rom, output_file)) {
        return 0;
    }
    free(rom.header);
    return crc;
}
