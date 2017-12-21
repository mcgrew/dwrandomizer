
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <math.h>
#include <ctype.h>

#include "dwr.h"
#include "chaos.h"
#include "map.h"
#include "crc64.h"
#include "mt64.h"

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


const char alphabet[] = "0123456789abcdefghijklmnopqrstuvwxyz"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ\"\"'*>_:__.,-_?!;)(``_'___________  ";

const char title_alphabet[] = "0123456789__________________________"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ__________________________________!.c-     ";

const char flag_order[] =
        "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

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
static uint64_t parse_flags(dw_rom *rom, char *flags)
{
    int i, len;
    char *order;

    len = strlen(flags);
    qsort(flags, len, sizeof(char), &compare);
    rom->flags = 0;
    for (i=0; i < len; i++) {
        order = strchr(flag_order, flags[i]);
        if (order) {
            rom->flags |= UINT64_C(1) << (order - flag_order);
        }
    }
    return rom->flags;
}

/**
 * Initializes the dw_rom struct
 *
 * @param rom An uninitialized dw_rom
 * @param input_file The file to read the rom data from
 * @param flags The flags received from the user.
 * @return A boolean indicating whether initialization was sucessful
 */
BOOL dwr_init(dw_rom *rom, const char *input_file, char *flags)
{
    FILE *input;
    int read;

    rom->raw = malloc(ROM_SIZE);
    memset(rom->raw, 0, ROM_SIZE);
    input = fopen(input_file, "rb");
    if (!input) {
        fprintf(stderr, "Unable to open ROM file '%s'", input_file);
        return FALSE;
    }
    read = fread(rom->raw, 1, OLD_ROM_SIZE, input);
    if (read < OLD_ROM_SIZE) {
        fprintf(stderr, "File '%s' is too small and may be corrupt, aborting.",
                input_file);
        return FALSE;
    }
    fclose(input);

    // Move everything from 0xc010 to 0x14010 -> 0x1c010 to 0x24010
    int i = 0xc010;
    for (i = 0xc010; i < 0x14010; i++) {
        rom->raw[i+0x10000] = rom->raw[i];
        rom->raw[i] = 0xff;
    }
    rom->raw[0x4] = 8;

    rom->map.flags = parse_flags(rom, flags);
    /* subtract 0x9d5d from these pointers */
    rom->map.pointers = (uint16_t*)&rom->raw[0x2663];
    rom->map.encoded = &rom->raw[0x1d6d];
    rom->map.meta = (dw_map_meta*)&rom->raw[0x2a];
    rom->map.warps_from = (dw_warp*)&rom->raw[0x1f3d8];
    rom->map.warps_to   = (dw_warp*)&rom->raw[0x1f471];
    rom->map.love_calc = (dw_love_calc*)&rom->raw[0x1df4b];
    rom->map.return_point = (dw_return_point*)&rom->raw[0x1db11];
    rom->map.rainbow_drop = (dw_rainbow_drop*)&rom->raw[0x1de9b];
    rom->map.rainbow_bridge = (dw_rainbow_drop*)&rom->raw[0x2c4b];
    rom->stats = (dw_stats*)&rom->raw[0x60dd];
    rom->new_spells = (dw_new_spell*)&rom->raw[0x1eaf8];
    rom->mp_reqs = &rom->raw[0x1d63];
    rom->xp_reqs = (uint16_t*)&rom->raw[0x1f36b];
    rom->enemies = (dw_enemy*)&rom->raw[0x5e5b];
    rom->zones = &rom->raw[0x1f55f];
    rom->zone_layout = &rom->raw[0x1f532];
    rom->chests = (dw_chest*)&rom->raw[0x5ddd];
    rom->axe_knight = (dw_forced_encounter*)&rom->raw[0x1cd61];
    rom->green_dragon = (dw_forced_encounter*)&rom->raw[0x1cd78];
    rom->golem = (dw_forced_encounter*)&rom->raw[0x1cd95];
    rom->axe_knight_run = (dw_forced_encounter*)&rom->raw[0x1e8e4];
    rom->green_dragon_run = (dw_forced_encounter*)&rom->raw[0x1e90b];
    rom->golem_run = (dw_forced_encounter*)&rom->raw[0x1e938];
    rom->encounter_types[0] = &rom->raw[0x1cd74];
    rom->encounter_types[1] = &rom->raw[0x1cd91];
    rom->encounter_types[2] = &rom->raw[0x1cdae];
    rom->token = (dw_searchable*)&rom->raw[0x1e11b];
    rom->flute = (dw_searchable*)&rom->raw[0x1e15a];
    rom->armor = (dw_searchable*)&rom->raw[0x1e170];
    rom->weapon_shops = &rom->raw[0x19a1];
    rom->weapon_prices = (uint16_t*)&rom->raw[0x1957];
    rom->weapon_price_display = (uint16_t*)&rom->raw[0x7e20];
    rom->music = &rom->raw[0x31bf];
    rom->title_text = &rom->raw[0x3f36];

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
            } else if (j == alphalen-1) {
                string[i] = 0x5f;
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

/**
 * Patches the rom at the specified address with the specified bytes.
 *
 * @param rom The rom struct to be patched
 * @param address The address inside the rom to apply the patch.
 * @param size The size of the patch
 * @param ... A series of uint8_t bytes, the patch data
 * @return The address of the end of the patch
 */
static uint16_t vpatch(dw_rom *rom, uint32_t address, uint32_t size, ...)
{
    uint32_t i;
    va_list arg;
    uint8_t *p;

    p = &rom->raw[address];
    va_start(arg, size);

    for (i=0; i < size; i++) {
        *(p++) = va_arg(arg, int);
    }
    va_end(arg);
    return p - rom->raw;

}

/**
 * Patches the rom at the specified address with the specified bytes.
 *
 * @param rom The rom struct to be patched
 * @param address The address inside the rom to apply the patch
 * @param size The size of the patch
 * @param data An array of bytes to use for the patch.
 * @return The address of the end of the patch
 */
static uint16_t patch(dw_rom *rom, uint32_t address, uint32_t size,
                      uint8_t *data)
{
    uint32_t i;
    uint8_t *p;

    p = &rom->raw[address];

    for (i=0; i < size; i++) {
        *(p++) = data[i];
    }
    return p - rom->raw;
}

/**
 * Patches the data at the specified pointer
 *
 * @param p A pointer to the rom data to be patched
 * @param size The size of the patch
 * @param data An array of bytes to use for the patch.
 * @return A pointer to the end of the patch
 */
static uint8_t *ppatch(uint8_t *p, uint32_t size, uint8_t *data)
{
    uint32_t i;

    for (i=0; i < size; i++) {
        *(p++) = data[i];
    }
    return p;
}

/**
 * Patches the data at the specified pointer
 *
 * @param p A pointer to the rom data to be patched
 * @param size The size of the patch.
 * @param ... A series of uint8_t bytes to be used for the patchj
 * @return A pointer to the end of the patch
 */
static uint8_t *pvpatch(uint8_t *p, uint32_t size, ...)
{
    uint32_t i;
    va_list arg;

    va_start(arg, size);

    for (i=0; i < size; i++) {
        *(p++) = va_arg(arg, int);
    }
    va_end(arg);
    return p;

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
    end = &rom->raw[ROM_SIZE - len];

    strncpy(dw_text, text, 256);
    strncpy(dw_repl, replacement, 256);
    ascii2dw((unsigned char *)dw_text);
    ascii2dw((unsigned char *)dw_repl);

    for (start = rom->raw; start < end; start++) {
        if (!memcmp(start, dw_text, len)) {
            memcpy(start, dw_repl, len);
            return start;
        }
    }
    return NULL;
}

/**
 * Returns a random index of a chest that is not in charlock. For simplicity,
 * this function will also not return the starting throne room key chest.
 */
static inline int non_charlock_chest()
{
    int chest;

    chest = (int)mt_rand(0, 22);
    /* avoid 11-16 and chest 24 (they are in charlock) */
    if (chest >= 6) chest += 1; /* chest 6 is the throne room key */
    if (chest >= 11) chest += 6;
    if (chest >= 24) chest += 1;
    return chest;
}

/**
 * Determines whether or not the chest item is a quest item.
 *
 * @param item The item member of a chest
 * @return A boolean indicated whether or not the item is crucial for the quest
 *      to obtain the rainbow drop.
 */
static inline BOOL is_quest_item(uint8_t item)
{
    switch(item) {
        case TOKEN:
        case STONES:
        case HARP:
        case STAFF:
            return TRUE;
        default:
            return FALSE;
    }
}

/**
 * Looks for quest items which may be in Charlock due to shuffling and moves
 * them to a chest outside Charlock.
 *
 * @param rom The rom struct.
 */
static inline void check_quest_items(dw_rom *rom)
{
    int i, tmp_index;
    uint8_t tmp_item;

    for (i=1; i <= 30; i++) {
        if (rom->chests[i].map == CHARLOCK_THRONE_ROOM ||
                rom->chests[i].map == CHARLOCK_CAVE_2) {

            if (is_quest_item(rom->chests[i].item)) {
                do {
                    tmp_index = non_charlock_chest();
                } while (is_quest_item(rom->chests[tmp_index].item));
                tmp_item = rom->chests[tmp_index].item;
                rom->chests[tmp_index].item = rom->chests[i].item;
                rom->chests[i].item = tmp_item;
            }
        }
    }
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
 * Shuffles the contents of all chests in the game with the exception of the
 * starting throne room key and the staff of rain.
 *
 * @param rom The rom struct
 */
static void shuffle_chests(dw_rom *rom) {
    int i;
    dw_chest *chest;
    uint8_t *cont, contents[CHEST_COUNT-2] = {
            CURSED_BELT, CURSED_BELT,
            GOLD_500, GOLD_500, GOLD_500, GOLD_500, GOLD_500, GOLD_500, GOLD_500,
            WINGS, WINGS,
            KEY, KEY, KEY,
            HERB, HERB, HERB, HERB,
            TORCH, TORCH,
            FAIRY_WATER, FAIRY_WATER,
            STONES, DRAGON_SCALE, HARP, SWORD, NECKLACE, TORCH, RING
    };

    if (!SHUFFLE_CHESTS(rom) && !random_percent(rom, 67))
        return;

    printf("Shuffling chest contents...\n");
    cont = contents;
    chest = rom->chests;

    if (!mt_rand(0, 2)) { /* 33% chance */
        rom->token->map = NO_MAP; /* remove token */
        contents[0] = TOKEN; /* replace cursed belt in a chest */
    }
    if (!mt_rand(0, 2)) { /* 33% chance */
        rom->flute->map = NO_MAP; /* remove flute */
        contents[1] = FLUTE; /* replace cursed belt in a chest */
    }
    
    /* shuffle the contents and place them in chests */
    mt_shuffle(contents, CHEST_COUNT-2, sizeof(uint8_t));
    chest = rom->chests;
    cont = contents;
    for (i=0; i < CHEST_COUNT; i++) {
        /* don't move the staff or starting key*/
        if ((chest->map == TANTEGEL_THRONE_ROOM && chest->item == KEY) ||
                    chest->item == STAFF) { 
            chest++;
            continue;
        }
        (chest++)->item = *(cont++);
    }

    check_quest_items(rom);

    /* make sure the player can't get more than one token or flute */
    vpatch(rom, 0x1e2fa, 95,
           0xc9, 0x08,        /* CMP #$08   ; If it's not the flute           */
           0xd0, 0x0b,        /* BNE $E2F7  ; Jump to the next check          */
           0xa9, 0x05,        /* LDA #$05   ; Load the flute inventory value  */
           0x20, 0x55, 0xe0,  /* JSR $E055  ; Jump to inventory check         */
           0xc9, 0xff,        /* CMP #$FF   ; If return value is $FF (found)  */
           0xd0, 0xac,        /* BNE $E2A3  ; The chest is empty              */
           0xa9, 0x08,        /* LDA #$08   ; Reset the flute chest value     */
           0xc9, 0x0a,        /* CMP #$0A   ; If it's not the token           */
           0xd0, 0x33,        /* BNE $E330  ; Jump to the next check          */
           0xa9, 0x07,        /* LDA #$07   ; Load the token inventory value  */
           0x20, 0x55, 0xe0,  /* JSR $E055  ; Jump to inventory check         */
           0xc9, 0xff,        /* CMP #$FF   ; If return value is $FF (found)  */
           0xd0, 0x9d,        /* BNE $E2A3  ; The chest is empty              */
           0xa9, 0x0a,        /* LDA #$0A   ; Reset the token chest value     */
           0xea, 0xea, 0xea, 0xea, 0xea, 0xea, 0xea, 0xea, /* NOP x 40 */
           0xea, 0xea, 0xea, 0xea, 0xea, 0xea, 0xea, 0xea,
           0xea, 0xea, 0xea, 0xea, 0xea, 0xea, 0xea, 0xea,
           0xea, 0xea, 0xea, 0xea, 0xea, 0xea, 0xea, 0xea,
           0xea, 0xea, 0xea, 0xea, 0xea, 0xea, 0xea, 0xea,
           0xc9, 0x11,         /* CMP #$11  ; If value is >= $11              */
           0xb0, 0x03,         /* BCS $E337 ; Jump to the next check          */
           0x4c, 0x69, 0xe2,   /* JSR $E269 ; Add the item to inventory       */
           0xc9, 0x17,         /* CMP #$17  ; If the value is greater >= $17  */
           0xb0, 0x0e,         /* BCS $E349 ; It's the tablet                 */
           0xa9, 0xff,         /* LDA #$FF                                    */
           0x85, 0x3e,         /* STA $003E                                   */
           0xa9, 0xf4,         /* LDA #$F4                                    */
           0x85, 0x00,         /* STA $0000                                   */
           0xa9, 0x01,         /* LDA #$01                                    */
           0x85, 0x01,         /* STA $0001                                   */
           0xd0, 0x1c          /* BNE $E365 ; Add gold to the player          */
    );

}

/**
 * Randomizes enemy attack patterns (spells)
 *
 * @param rom The rom struct
 */
static void randomize_attack_patterns(dw_rom *rom)
{
    int i;
    dw_enemy *enemies;

    if (!RANDOMIZE_PATTERNS(rom) && !random_percent(rom, 67))
        return;

    printf("Randomizing enemy attack patterns...\n");
    enemies = rom->enemies;

    for (i=SLIME; i <= RED_DRAGON; i++) {
        if (mt_rand_bool()) {
            enemies[i].pattern = mt_rand(0, 255);
            enemies[i].s_ss_resist &= 0xf0;
            enemies[i].s_ss_resist |= mt_rand(0, i/3);
        } else {
            enemies[i].pattern = 0;
            /* no spells, max out resistance */
            enemies[i].s_ss_resist |= 0xf;
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
                         0x6, 0x7, 0x8, 0x9, 0xa, 0xb, 0xc, 0xd};


    for (i=0; i < 29; i++) {
        rom->music[i] = choices[mt_rand(0, sizeof(choices)-1)];
    }
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

    printf("Disabling game music\n");

    memset(rom->music, 0, 29);
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
    BOOL zones_randomized = FALSE;

    if (rom->randomized_map || RANDOMIZE_ZONES(rom) || random_percent(rom, 67)) {
        zones_randomized = TRUE;
        int i;

        printf("Randomizing enemy zone layout...\n");

        for (i=0; i < 32; i++) {
            rom->zone_layout[i] = 0;
            rom->zone_layout[i] |= mt_rand(3, 15) << 4;
            rom->zone_layout[i] |= mt_rand(3, 15);
        }
    }

    // Ensure that something easy is hanging out near Tantegel.
    if (rom->randomized_map || zones_randomized) {
        dw_warp *tantegel = &rom->map.warps_from[WARP_TANTEGEL];

        /* set up zones around tantegel */
        set_ow_zone(rom, tantegel->x / 15, tantegel->y / 15, 0);
        if (!STAT_OLDSCHOOL(rom) && !random_percent(rom, 10)) {
            set_ow_zone(rom, (tantegel->x) / 15 - 1, tantegel->y / 15, 1);
            set_ow_zone(rom, (tantegel->x) / 15 + 1, tantegel->y / 15, 1);
            set_ow_zone(rom, tantegel->x / 15, (tantegel->y) / 15 - 1, 2);
            set_ow_zone(rom, tantegel->x / 15, (tantegel->y) / 15 + 1, 2);
        }
    }
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

    if (!RANDOMIZE_ZONES(rom) && !random_percent(rom, 67))
        return;

    printf("Randomizing monsters in enemy zones...\n");

    zone = 0;  /* tantegel zone */
    for (i=0; i < 5; i++) { 
        rom->zones[zone * 5 + i] = mt_rand(SLIME, SCORPION);
    }

    if (!STAT_OLDSCHOOL(rom) && !random_percent(rom, 90)) {
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
    } else {
        for (zone=1; zone <= 15; zone++) { /* overworld/hybrid zones */
            for (i=0; i < 5; i++) {
                rom->zones[zone * 5 + i] = mt_rand(SLIME, RED_DRAGON);
            }
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

    for (i=0; i < 3; i++) { /* randomize the forced encounters */
        *rom->encounter_types[i] = charlock_enemies[mt_rand(4, 9)];
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
    dw_shop_item items[15] = {
            BAMBOO_POLE, CLUB, COPPER_SWORD, HAND_AXE, BROAD_SWORD, FLAME_SWORD,
            CLOTHES, LEATHER_ARMOR, CHAIN_MAIL, HALF_PLATE, FULL_PLATE,
            MAGIC_ARMOR, SMALL_SHIELD, LARGE_SHIELD, SILVER_SHIELD
    };

    if (!RANDOMIZE_SHOPS(rom) && !random_percent(rom, 67))
        return;

    printf("Randomizing weapon shop inventory...\n");
    
    six_item_shop = mt_rand(0, 6);
    shop_item = rom->weapon_shops;
    
    for (i=0; i < 7; i++) {
        shop_start = shop_item;
        for (j=0; j < 5; j++) {
            while(shop_contains(shop_start, shop_item, 
                    *shop_item = items[mt_rand(0, 14)])) {}
            shop_item++;
        }
        if (i == six_item_shop) {
            while(shop_contains(shop_start, shop_item, 
                    *shop_item = items[mt_rand(0, 14)])) {}
            shop_item++;
            qsort(shop_start, 6, sizeof(uint8_t), &compare);
        } else {
            qsort(shop_start, 5, sizeof(uint8_t), &compare);
        }
        *(shop_item++) = SHOP_END;
    }
}

/**
 * Shuffles the 3 items which can be searched for.
 *
 * @param rom The rom struct
 */
static void shuffle_searchables(dw_rom *rom)
{
    dw_searchable searchables[3];

    if (!SHUFFLE_CHESTS(rom) && !random_percent(rom, 67))
        return;

    printf("Shuffling searchable items...\n");

    searchables[0] = *(rom->token);
    searchables[1] = *(rom->flute);
    searchables[2] = *(rom->armor);

    mt_shuffle(searchables, 3, sizeof(dw_searchable));

    rom->token->map = searchables[0].map;
    rom->token->x   = searchables[0].x;
    rom->token->y   = searchables[0].y;
    rom->flute->map = searchables[1].map;
    rom->flute->x   = searchables[1].x;
    rom->flute->y   = searchables[1].y;
    rom->armor->map = searchables[2].map;
    rom->armor->x   = searchables[2].x;
    rom->armor->y   = searchables[2].y;
}

/**
 * Chooses random numbers on an inverted power curve
 * @param min The minimum value
 * @param max The maximum value
 * @param power An exponent to use for the curve.
 * @return The chosen random number.
 */
static uint8_t inverted_power_curve(uint8_t min, uint8_t max, double power)
{
    double p_range;
    
    p_range= pow((double)(max - min), 1 / power);
    return min + round((max - min) - pow((mt_rand_double() * p_range), power));
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

    if (!RANDOMIZE_GROWTH(rom) && !random_percent(rom, 67))
        return;

    printf("Randomizing stat growth...\n");
    unsigned int random_stat = 0;
    if (RANDOM_PCT(rom)) {
        // If "no equipment" was randomly selected,
        // Force SuperWarrior or God stat growth
        // so the game can end at a reasonable hour
        if (&rom->raw[0x18a4] == 0x00)
            if (random_percent(rom, 75))
                random_stat = 4;
            else
                random_stat = 5;
        else
            if (random_percent(rom, 50))
                random_stat = mt_rand(0, 5);
    }

    for (i=0; i < 30; i++) {
        if (STAT_STRONG(rom) || random_stat == 1) {
            str[i] = inverted_power_curve(8, 165, 1.15);
            agi[i] = inverted_power_curve(4, 155, 1.50);
            hp[i] =  inverted_power_curve(10, 230, 0.8);
            mp[i] =  inverted_power_curve(0, 220, 0.7);
        } else if (STAT_WEAK(rom) || random_stat == 2) {
            str[i] = inverted_power_curve(4, 155, 0.90);
            agi[i] = inverted_power_curve(4, 145, 1.32);
            hp[i] =  inverted_power_curve(10, 250, 0.98);
            mp[i] =  inverted_power_curve(0, 240, 0.98);
        } else if (STAT_OLDSCHOOL(rom) || random_stat == 3) {
            str[i] = inverted_power_curve(4, 155, 1.00);
            agi[i] = inverted_power_curve(4, 145, 1.00);
            hp[i] =  inverted_power_curve(10, 230, 1.00);
            mp[i] =  inverted_power_curve(0, 220, 1.00);
        } else if (STAT_SUPERWAR(rom) || random_stat == 4) {
            str[i] = inverted_power_curve(4, 175, 1.8);
            agi[i] = inverted_power_curve(4, 165, 1.32);
            hp[i] =  inverted_power_curve(10, 250, 1.18);
            mp[i] =  inverted_power_curve(0, 250, 1.08);
        } else if (STAT_GOD(rom) || random_stat == 5) {
            str[i] = inverted_power_curve(4, 255, 2);
            agi[i] = inverted_power_curve(0, 255, 1.92);
            hp[i] =  inverted_power_curve(2, 255, 1.84);
            mp[i] =  inverted_power_curve(0, 255, 1.76);
        } else { // normal
            str[i] = inverted_power_curve(4, 155, 1.18);
            agi[i] = inverted_power_curve(4, 145, 1.32);
            hp[i] =  inverted_power_curve(10, 230, 0.98);
            mp[i] =  inverted_power_curve(0, 220, 0.95);
        }
    }
    qsort(str, 30, sizeof(uint8_t), &compare);
    qsort(agi, 30, sizeof(uint8_t), &compare);
    qsort(hp,  30, sizeof(uint8_t), &compare);
    qsort(mp,  30, sizeof(uint8_t), &compare);

    for (i=0; i < 30; i++) {
        stats = &rom->stats[i];
        stats->str = str[i];
        stats->agi = agi[i];
        stats->hp =  hp[i];
        stats->mp =  mp[i];
    }
}

/**
 * Randomizes the order in which spells are learned.
 *
 * @param rom The rom struct
 */
static void randomize_spells(dw_rom *rom)
{
    int i, j;
    dw_stats *stats;

    if (!RANDOMIZE_SPELLS(rom) && !random_percent(rom, 67))
        return;

    printf("Randomizing spell learning...\n");

    // If you use random percent, spell levels can be
    // 20(10%), or the normal 16(80%).
    // If you use Old School stats, the max level is 20.
    int maxSpellLevel = (STAT_OLDSCHOOL(rom) ||
                         random_percent(rom, 10) ? 20 : 16);
    /* choose levels for each spell */
    for (i=0; i < 10; i++) {
        rom->new_spells[i].level = mt_rand(1, maxSpellLevel);
    }

    for (i=0; i < 30; i++) {
        stats = &rom->stats[i];
        stats->spells = 0;
        for (j=0; j < 10; j++) {
            /* spell masks are in big endian format for some reason */
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
    if (!SHORT_CHARLOCK(rom) && !random_percent(rom, 67))
        return;

    printf("Shortening Charlock Castle...\n");

    rom->map.warps_to[WARP_CHARLOCK].map = CHARLOCK_THRONE_ROOM;
    rom->map.warps_to[WARP_CHARLOCK].x = 10;
    rom->map.warps_to[WARP_CHARLOCK].y = 29;
    rom->map.warps_from[WARP_CHARLOCK_CHEST].map = CHARLOCK_THRONE_ROOM;
    rom->map.warps_from[WARP_CHARLOCK_CHEST].x = 18;
    rom->map.warps_from[WARP_CHARLOCK_CHEST].y = 9;
    vpatch(rom, 0x4d4, 1, 0x76); /* Add some downward stairs near the chests */
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

    if (rom->randomized_map && !rom->open_charlock)
        return;

    if (!OPEN_CHARLOCK(rom) && !random_percent(rom, 33))
        return;

    rom->token->map = NO_MAP; /* remove token */

    printf("Opening Charlock and removing quest items...\n");
    /* remove the quest items since we won't need them */
    for (i=0; i <= 31; ++i) {
        if (is_quest_item(rom->chests[i].item)) {
            rom->chests[i].item = HERB;
        }
    }

    if (!RANDOMIZE_MAP(rom)) {
        rom->raw[0x20fe] = 0xb0;
    }
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
    const uint8_t remake_xp[DRAGONLORD_2+1] = {
              1,   2,   3,   4,   8,  12,  16,  14,  15,  18,  20,  25,  28,
             31,  40,  42, 255,  47,  52,  58,  58,  64,  70,  72, 255,   6,
             78,  83,  90,  95, 135, 105, 120, 130, 180, 155, 172, 255, 0, 0
    };
    const uint8_t remake_gold[DRAGONLORD_2+1] = {
              2,   4,   6,   8,  16,  20,  25,  21,  19,  30,  25,  42,  50,
             48,  60,  62,   6,  75,  80,  95, 110, 105, 110, 120,  10, 255,
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
    const uint8_t remake_hp[DRAGONLORD_2+1] = {
              2,   3,   5,   7,  12,  13,  13,  22,  23,  20,  16,  24,  28,
             18,  33,  39,   3,  33,  37,  35,  44,  37,  40,  40, 153,  35,
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
    uint8_t len, space, dw_text[33];

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

    needed = MIN(end - pos - reserved, 32);
    memset(text, 0x5f, needed);
    pos = ppatch(pos, needed, (uint8_t*)text);
    if (needed == 32) {
        pos = pvpatch(pos, 1, 0xfc);
    } else {
        pos = pvpatch(pos, 4, 0xf7, 32 - needed, 0x5f, 0xfc);
    }
    return pos;
}

/**
 * Updates the title screen with the randomizer version, flags, and seed number.
 *
 * @param rom The rom struct
 */
static void update_title_screen(dw_rom *rom)
{
    int  needed;
    char *f, *fo, text[33];
    uint64_t flags;
    uint8_t *pos, *end;
    
    pos = &rom->raw[0x3f36];
    end = &rom->raw[0x3fc5];
    text[32] = '\0';
    flags = rom->flags;
    f = text;
    fo = (char*)flag_order;

    pos = pvpatch(pos, 4, 0xf7, 32, 0x5f, 0xfc); /* blank line */
    pos = center_title_text(pos, "RANDOMIZER");  /* RANDOMIZER text */
    pos = pvpatch(pos, 4, 0xf7, 32, 0x5f, 0xfc); /* blank line */
    pos = center_title_text(pos, VERSION);       /* version number */

    pos = pvpatch(pos, 4, 0xf7, 32, 0x5f, 0xfc); /* blank line */
    pos = pvpatch(pos, 4, 0xf7, 32, 0x5f, 0xfc); /* blank line */
    pos = pvpatch(pos, 4, 0xf7, 32, 0x5f, 0xfc); /* blank line */
    pos = pvpatch(pos, 4, 0xf7, 32, 0x5f, 0xfc); /* blank line */

    /* parse the flags back to a string */
    while (flags) {
        if (flags & 1) *(f++) = *fo;
        flags >>= 1;
        fo++;
        if (f - text >= 32) break;
    }
    *f = '\0';

    pos = center_title_text(pos, text);          /* flags */
//    pos = pvpatch(pos, 4, 0xf7, 32, 0x5f, 0xfc); /* blank line */
    snprintf((char *)text, 33, "%"PRIu64, rom->seed);
    pos = pad_title_screen(pos, end, 15 + strlen(text)); /* blank line */
    pos = center_title_text(pos, text);         /* seed number */

    pos = pad_title_screen(pos, end, 8); /* blank line */
//    needed = MIN(end - pos - 8, 32);
//    memset(text, 0x5f, needed);
//    pos = ppatch(pos, needed, (uint8_t*)text);
//    if (needed == 32) {
//        pos = pvpatch(pos, 1, 0xfc);
//    } else {
//        pos = pvpatch(pos, 4, 0xf7, 32 - needed, 0x5f, 0xfc);
//    }

    needed = MAX(end - pos - 4, 0);
    if (needed > 0)
        pos = ppatch(pos, needed, (uint8_t*)text);
    pos = pvpatch(pos, 4, 0xf7, 32 - needed, 0x5f, 0xfc);

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
    vpatch(rom, 0x1f10c, 4, 0x20, 0x7d, 0xff, 0xea);
    vpatch(rom, 0x1ff8d, 17,
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
    vpatch(rom, 0x1ff64, 3,
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

    if (!DEATH_NECKLACE(rom) && !random_percent(rom, 67))
        return;

    printf("Adding functionality to the death necklace...\n");

    vpatch(rom, 0x1ff64, 31,
            /* ff54: */
            0x24, 0xcf,  /* AND #$80  ; check bit 8 (death necklace)       */
            0x10, 0x18,  /* BPL $FF71                                      */
            0xa5, 0xca,  /* LDA $00CA ; load max HP                        */
            0x46, 0xca,  /* LSR $00CA ; divide max HP by 4                 */
            0x46, 0xca,  /* LSR $00CA                                      */
            0x18,        /* CLC       ; Clear the carry bit                */
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
 * Other various patches for gameplay, such as silver harp enemies, town and
 * dungeon map changes and moving some NPCs.
 *
 * @param rom The rom struct
 */
static void other_patches(dw_rom *rom)
{
    printf("Applying various other patches...\n");

    /* convert PRG1 to PRG0 */
    vpatch(rom, 0xaf7c, 1,  0xef);

    /* move the golem encounter to charlock */
    rom->golem_run->map = rom->golem->map = CHARLOCK_THRONE_ROOM;
    rom->golem_run->x = rom->golem->x = 25;
    rom->golem_run->y = rom->golem->y = 22;
    vpatch(rom, 0x1cdab, 2, 0xea, 0xea); /* make sure he always appears */
    /* make swamp cave encounter always appear */
    vpatch(rom, 0x1cd8e, 2, 0xea, 0xea);

    vpatch(rom, 0x1de33, 10,  /* Changes the enemies summoned by the harp. */
        /* de23: */
        0x29, 0x7,  /* AND #$07    ; limit the random number to 0-7 */
        0xaa,  /* TAX         ; move the value to the X register */
        0xbd, 0x64, 0xf5,  /* LDA #F564,X ; load the monster from the zone table */
        0xea,  /* NOP         ; fill in the rest with NOP */
        0xea,  /* NOP         ; */
        0xea,  /* NOP         ; */
        0xea   /* NOP         ; */
    );
    vpatch(rom, 0x43a, 1, 0x47);  /* add new stairs to the throne room */
    vpatch(rom, 0x2b9, 1, 0x45);  /* add new stairs to the 1st floor */
    vpatch(rom, 0x2d7, 1, 0x66);  /* add a new exit to the first floor */
    /* replace the usless grave warps with some for tantegel */
    vpatch(rom, 0x1f45f, 3, 5, 1, 8);
    vpatch(rom, 0x1f4f8, 3, 4, 1, 7);
    vpatch(rom, 0x1298, 1, 0x22);  /* remove the top set of stairs for the old warp in the grave */
    /* Sets the encounter rate of Zone 0 to be the same as other zones. */
    vpatch(rom, 0x1cecf, 3, 0x4c, 0x04, 0xcf);  /* skip over the zone 0 code */
    vpatch(rom, 0x1e270, 1, 0);  /* set death necklace chance to 100% */
    vpatch(rom, 0x1e75d, 1, 9);  /* buff the hurt spell */
    vpatch(rom, 0x1dbd1, 1, 18);  /* buff the heal spell */
    vpatch(rom, 0x1ea51, 5, 0xad, 0x07, 0x01, 0xea, 0xea); /* I forget what this does */
    /* fixing some annoying roaming npcs */
    vpatch(rom, 0x18fe, 1, 0xa7); /* move the stupid old man from the item shop */
    vpatch(rom, 0x91f,  1, 0x6f); /* quit ignoring the customers */
    vpatch(rom, 0x94c,  1, 0x6f); /* quit ignoring the customers */
    vpatch(rom, 0x17b2, 3, 0, 0, 0); /* delete roaming throne room guard */
    /* Change the player sprite color for fun */
    vpatch(rom, 0x1f141, 2, 0x69, 0x03); /* Lock the stat build modifier at 3 */
    /* Embed the randomizer version in the ROM */
//    vpatch(rom, 0xfff0, 16, "DWRANDOMIZER " BUILD);

    /* I always hated this wording */
    dwr_str_replace(rom, "The spell will not work", "The spell had no effect");
}

static void credits(dw_rom *rom)
{
    printf("Patching credits...\n");

    /* patch the credits a bit */
    vpatch(rom, 0x5844, 162,
           /* dwrandomizer */
           0x63, 0x20,
               0x27, 0x35, 0x24, 0x2a, 0x32, 0x31, 0x5f, 0x3a, 0x24, 0x35, 0x35,
               0x2c, 0x32, 0x35, 0x5f, 0x35, 0x24, 0x31, 0x27, 0x32, 0x30, 0x2c,
               0x3d, 0x28, 0x35,

            /* developer */
           0xfc, 0xc3, 0x20,
               0x27, 0x28, 0x39, 0x28, 0x2f, 0x32, 0x33, 0x28, 0x35,
           0xfc, 0xd7, 0x20,
               0x30, 0x26, 0x2a, 0x35, 0x28, 0x3a,

            /* contributor */
           0xfc, 0x23, 0x21,
               0x26, 0x32, 0x31, 0x37, 0x35, 0x2c, 0x25, 0x38, 0x37, 0x32, 0x35,
           0xfc, 0x34, 0x21,
               0x2a, 0x24, 0x30, 0x28, 0x25, 0x32, 0x3c, 0x29, 0x09,

           /* sprite work */
           0xfc, 0x83, 0x21,
               0x36, 0x33, 0x35, 0x2c, 0x37, 0x28, 0x5f, 0x3a, 0x32, 0x35, 0x2e,
           0xfc, 0x95, 0x21,
               0x3b, 0x24, 0x35, 0x31, 0x24, 0x3b, 0x04, 0x02,
           0xfc, 0xd7, 0x21,
               0x30, 0x26, 0x2a, 0x35, 0x28, 0x3a,

           /* promotion */
           0xfc, 0x23, 0x22,
               0x33, 0x35, 0x32, 0x30, 0x32, 0x37, 0x2c, 0x32, 0x31,
           0xfc, 0x6f, 0x22,
               0x31, 0x28, 0x36, 0x26, 0x24, 0x35, 0x27, 0x2c, 0x31, 0x24, 0x2f,
               0x2c, 0x37, 0x3c,
           0xfc, 0x34, 0x22,
               0x2a, 0x24, 0x30, 0x28, 0x25, 0x32, 0x3c, 0x29, 0x09,

           0x5f, 0x5f, 0x5f, 0x5f, 0x5f, 0x5f, 0x5f, 0x5f, 0x5f, 0x5f, 0x5f,
           0x5f, 0x5f, 0x5f, 0x5f, 0x5f, 0x5f, 0x5f, 0x5f, 0x5f, 0x5f, 0x5f,
           0x5f, 0x5f, 0x5f, 0x5f, 0x5f, 0x5f, 0x5f, 0x5f, 0x5f, 0x5f, 0x5f,
           0x5f, 0x5f, 0x5f, 0x5f, 0x5f, 0x5f, 0x5f, 0x5f, 0x5f, 0x5f, 0x5f,
           0x5f, 0x5f, 0x5f, 0x5f, 0x5f, 0x5f
    );
    /* credit palette edits */
    vpatch(rom, 0x58eb, 1, 0x55);
    vpatch(rom, 0x58ee, 2, 0x04, 0xa3);
    vpatch(rom, 0x58f1, 2, 0x05, 0x55);
    vpatch(rom, 0x58f8, 1, 0x55);
    vpatch(rom, 0x5901, 1, 0xa5);
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
    vpatch(rom, 0x69f0, 4,
        /* 69e0 */
        0x4c, 0xa0, 0xbe,  /* JMP $BEA0 */
        0xea               /* NOP */
    );
    vpatch(rom, 0x6a33, 4, 
        /* 6a23: */
        0x4c, 0xcd, 0xbe,  /* JMP #BECD */
        0xea               /* NOP */
    );
    vpatch(rom, 0x7eb0, 45,
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
    vpatch(rom, 0x7edd, 46,
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
    vpatch(rom, 0x7f0b, 7,
        /* 7efb: */
        0xa5, 0xd9,        /* LDA $00D9 ; load map number */
        0xf0, 0xfb,        /* BEQ $7EF8 ; if it's 0, branch to $7EF8 (return) */
        0x4c, 0xe4, 0xa9   /* JMP $A9E4 ; jump to $A9E4 */
    );
    vpatch(rom, 0x7f12, 8,
        /* 7f02: */
        0x68,              /* PLA       ; pull A from stack */
        0xc5, 0xd9,        /* CMP $00D9 ; compare to map number */
        0xf0, 0xf3,        /* BEQ $7EF8 ; if equal, jump to $7EF8 (return) */
        0x4c, 0x27, 0xaa   /* JMP $A9E4 ; jump to $A9E4 */
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
        vpatch(rom, 0x7a43, 3, 0xea, 0xea, 0xea);
    }

    if (!SPEED_HACKS(rom))
        return;

    printf("Enabling speed hacks...\n");

    /* Following are some speed hacks from @gameboy9 */
    /* speed up encounter intros */
    vpatch(rom, 0x1e41a, 3, 0xea, 0xea, 0xea);
    vpatch(rom, 0x1e44d, 3, 0xea, 0xea, 0xea);
    vpatch(rom, 0x1c53f, 3, 0xea, 0xea, 0xea);
    vpatch(rom, 0x1ef49, 1, 2);  /* speed up the player attack animation */
    vpatch(rom, 0x1ed45, 1, 3);  /* speed up the enemy attack animation */
    /* speed up the death music */
    vpatch(rom, 0x4d38, 1, 0x1); 
    vpatch(rom, 0x4d3c, 1, 0x6); 
    vpatch(rom, 0x4d4b, 1, 0x7); 
    vpatch(rom, 0x4d4d, 1, 0x8); 
    vpatch(rom, 0x4d4f, 1, 0x8);
    vpatch(rom, 0x4d51, 1, 0x8);
    vpatch(rom, 0x4d53, 1, 0x2);
    vpatch(rom, 0x4d55, 1, 0x2);
    vpatch(rom, 0x4d57, 1, 0x10);
    /* speed up the level up music */
    vpatch(rom, 0x463b, 1, 0xff),
    /* speed up the battle win music */
    vpatch(rom, 0x4724, 1, 1);
    vpatch(rom, 0x472a, 1, 1);
    vpatch(rom, 0x472c, 1, 1);
    vpatch(rom, 0x472e, 1, 1);
    /* speed up the fairy flute */
    vpatch(rom, 0x4cb1, 1, 1);
    vpatch(rom, 0x4cb3, 1, 1);
    vpatch(rom, 0x4cc5, 1, 1);
    vpatch(rom, 0x4cc7, 1, 1);
    vpatch(rom, 0x4cc9, 1, 1);
    vpatch(rom, 0x4ccd, 1, 1);
    vpatch(rom, 0x4ce2, 1, 1);
    vpatch(rom, 0x4ce4, 1, 1);
    vpatch(rom, 0x4ce6, 1, 1);
    vpatch(rom, 0x4ce8, 1, 1);
    vpatch(rom, 0x4cea, 1, 1);
    vpatch(rom, 0x4cec, 1, 1);
    /* speed up the inn music */
    vpatch(rom, 0x46e4, 1, 1);
    vpatch(rom, 0x46e6, 1, 1);
    vpatch(rom, 0x46e8, 1, 1);
    vpatch(rom, 0x46ea, 1, 1);
    vpatch(rom, 0x46ec, 1, 1);
    vpatch(rom, 0x46ee, 1, 1);
    vpatch(rom, 0x46f0, 1, 1);
    vpatch(rom, 0x46f2, 1, 1);
    vpatch(rom, 0x46f4, 1, 1);
    vpatch(rom, 0x46f8, 1, 1);
    vpatch(rom, 0x4701, 1, 1);
    vpatch(rom, 0x4703, 1, 1);
    vpatch(rom, 0x4705, 1, 1);
    vpatch(rom, 0x4707, 1, 1);
    vpatch(rom, 0x4709, 1, 1);
    vpatch(rom, 0x470b, 1, 1);
    vpatch(rom, 0x470d, 1, 1);
    vpatch(rom, 0x470f, 1, 1);
    vpatch(rom, 0x4711, 1, 1);
    vpatch(rom, 0x4715, 1, 1);
    /* speed up spell casting */
    vpatch(rom, 0x436b, 1, 0x9e);
    vpatch(rom, 0x1db49, 6, 0xea, 0xea, 0xea, 0xea, 0xea, 0xea);
    vpatch(rom, 0x1db54, 9, 0xea, 0xea, 0xea, 0xea, 0xea, 0xea, 0xea, 0xea, 0xea);
}

static void no_keys(dw_rom *rom)
{
    int i;
    dw_chest *chest;

    if (!NO_KEYS(rom) && !random_percent(rom, 67))
        return;

    printf("Removing the need for keys...\n");
    /* Don't require keys to open the door */
    vpatch(rom, 0x1dca9, 2, 0xa9, 0x01);
    vpatch(rom, 0x1dcb8, 2, 0xea, 0xea);

    chest = rom->chests;
    for (i=0; i < CHEST_COUNT; i++) {
        if (chest->item == KEY)
            chest->item = GOLD_120;
        chest++;
    }

    /* remove the key shopkeepers */
    vpatch(rom, 0x1793, 3, 0, 0, 0);
    vpatch(rom, 0x186c, 3, 0, 0, 0);
    vpatch(rom, 0x182b, 3, 0, 0, 0);
}

static void no_equipment(dw_rom *rom)
{
    int i;
    dw_chest *chest;

    if (!NO_EQUIPMENT(rom) && !random_percent(rom, 33))
        return;

    printf("Removing all equipment...\n");
    /* Don't require keys to open the door */
    chest = rom->chests;
    for (i=0; i < CHEST_COUNT; i++) {
        if (chest->item == SWORD || chest->item == DRAGON_SCALE || chest->item == RING || chest->item == NECKLACE)
            chest->item = GOLD_500;
        chest++;
    }

    /* remove the equipment shopkeepers */
    vpatch(rom, 0x18a4, 3, 0, 0, 0); // Brecconary
    vpatch(rom, 0x1920, 3, 0, 0, 0); // Garinham
    vpatch(rom, 0x1837, 3, 0, 0, 0); // Cantlin - behind key door
    vpatch(rom, 0x1843, 3, 0, 0, 0); // Cantlin - South
    vpatch(rom, 0x1815, 3, 0, 0, 0); // Cantlin - Facebook guy
    vpatch(rom, 0x18e8, 3, 0, 0, 0); // Kol
    vpatch(rom, 0x186f, 3, 0, 0, 0); // Rimuldar

    // Remove Dragon's Scale from stores
    vpatch(rom, 0x19ce, 13, 0x15, 0xfd,
           0x11, 0x13, 0xfd,
           0x11, 0x13, 0xfd,
           0x11, 0x13, 0xfd,
           0x15, 0xfd);

    // Remove Erdrick's Armor from drop spot
    rom->armor->map = NO_MAP;
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
    dw_searchable *searchable;
    uint8_t text1[24], text2[75];
    int dx, dy;

    searchable = rom->token;
    if (searchable->map != OVERWORLD) {
        searchable = rom->flute;
    }
    if (searchable->map != OVERWORLD) {
        searchable = rom->armor;
    }
    if (searchable->map != OVERWORLD) {
        strcpy((char*)text1, "Thou must go and fight!");
        strcpy((char*)text2, "Go forth, descendant of Erdrick, " 
                "I have complete faith in thy victory! ");
        ascii2dw(text1);
        patch(rom, 0xa238, 23, text1);
        vpatch(rom, 0xa298, 1, 0x53); /* replace .' with ' */
    } else {
        dx = searchable->x - rom->map.warps_from[WARP_TANTEGEL].x;
        dy = searchable->y - rom->map.warps_from[WARP_TANTEGEL].y;
//        strcpy((char*)text1, "Thou may go and search.");
        snprintf((char*)text2, 73, "From Tantegel Castle travel %2d leagues "
                "to the %s and %2d to the %s",
                ABS(dy), (dy < 0) ? "north" : "south", 
                ABS(dx), (dx < 0) ? "west" : "east");
    }
    ascii2dw(text2);
    patch(rom, 0xa252, 70, text2);
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
    fwrite(rom->raw, 1, ROM_SIZE, output);
    fclose(output);
    return TRUE;
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
    snprintf(output_file, 1024, "%s/DWRando.%"PRIu64".%s.nes", output_dir, seed,
            flags);

    mt_init(seed);
    dw_rom rom;
    if (!dwr_init(&rom, input_file, flags)) {
        return 0;
    }
    rom.seed = seed;

    while(!map_generate_terrain(&rom)) {}
    shuffle_searchables(&rom);
    shuffle_chests(&rom);
    no_equipment(&rom);
    open_charlock(&rom);
    short_charlock(&rom);
    no_keys(&rom);
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
    dwr_token_dialogue(&rom);
    chaos_mode(&rom);
    other_patches(&rom);
    credits(&rom);
    crc = crc64(0, rom.raw, 0x20010);

    sprite(&rom, sprite_name);
    update_title_screen(&rom);
    randomize_music(&rom);
    disable_music(&rom);

    printf("Checksum: %016"PRIx64"\n", crc);
    if (!dwr_write(&rom, output_file)) {
        return 0;
    }
    free(rom.raw);
    return crc;
}
