
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

#include "dwr.h"
#include "patch.h"
#include "chaos.h"
#include "map.h"
#include "challenge.h"
#include "crc64.h"
#include "mt64.h"
#include "polyfit.h"

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
    rom->map.flags = parse_flags(rom, flags);
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
    rom->axe_knight = (dw_forced_encounter*)&rom->content[0xcd51];
    rom->green_dragon = (dw_forced_encounter*)&rom->content[0xcd68];
    rom->golem = (dw_forced_encounter*)&rom->content[0xcd85];
    rom->axe_knight_run = (dw_forced_encounter*)&rom->content[0xe8d4];
    rom->green_dragon_run = (dw_forced_encounter*)&rom->content[0xe8fb];
    rom->golem_run = (dw_forced_encounter*)&rom->content[0xe928];
    rom->encounter_types[0] = &rom->content[0xcd64];
    rom->encounter_types[1] = &rom->content[0xcd81];
    rom->encounter_types[2] = &rom->content[0xcd9e];
    // FIXME: these should be removed or modified for the new treasure code.
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
 * Patches the game code to allow Erdrick's Armor to be in a chest and other
 * items to be found on "searchable" spots
 *
 * @param rom The rom struct
 */
static void rewrite_search_take_code(dw_rom *rom, uint8_t *items)
{
    /* patch a few branch addresses */
    vpatch(rom, 0x0e110,    1,  0x19);
    vpatch(rom, 0x0e116,    1,  0x13);
    vpatch(rom, 0x0e11c,    1,  0x0d);
    vpatch(rom, 0x0e231,    1,  0x55);
    vpatch(rom, 0x0e23b,    1,  0x55);
    vpatch(rom, 0x0e26a,    1,  0x55);
    vpatch(rom, 0x0e2ad,    1,  0x55);
    vpatch(rom, 0x0e378,    1,  0x55);

    /* new "SEARCH" code */
    vpatch(rom, 0x0e11d,  109,
        0xa9, items[0],   /*   lda item0                                      */
        0xf0, 0x09,       /*   beq +                                          */
        0x48,             /* - pha                                            */
        0xa9, 0xff,       /*   lda #$ff                                       */
        0x85, 0x0e,       /*   sta $0e                                        */
        0x68,             /*   pla                                            */
        0x4c, 0x24, 0xe2, /*   jmp $e224                                      */
        0xa5, 0x45,       /* + lda $45     ; Load the current map             */
        0xc9, 0x07,       /*   cmp #$07    ; Is this Kol?                     */
        0xd0, 0x10,       /*   bne +                                          */
        0xa5, 0x3a,       /*   lda $3a     ; Load the X coordinate            */
        0xc9, 0x09,       /*   cmp #$09    ; Is it 9?                         */
        0xd0, 0x0a,       /*   bne +                                          */
        0xa5, 0x3b,       /*   lda $3b     ; Load the Y coordinate            */
        0xc9, 0x06,       /*   cmp #$06    ; Is it 6?                         */
        0xd0, 0x04,       /*   bne +                                          */
        0xa9, items[1],   /*   lda item1                                      */
        0xd0, 0xe1,       /*   bne -                                          */
        0xa5, 0x45,       /* + lda $45     ; Load the current map             */
        0xc9, 0x03,       /*   cmp #$03    ; Is this Hauksness?               */
        0xd0, 0x44,       /*   bne $e18c                                      */
        0xa5, 0x3a,       /*   lda $3a     ; Load the X coordinate            */
        0xc9, 0x12,       /*   cmp #$12    ; is it 18?                        */
        0xd0, 0x3e,       /*   bne $e18c                                      */
        0xa5, 0x3b,       /*   lda $3b    ; Load the Y coordinate             */
        0xc9, 0x0c,       /*   cmp #$0c   ; Is it 13?                         */
        0xd0, 0x38,       /*   bne $e13a                                      */
        0xa9, items[2],   /*   lda item2                                      */
        0xd0, 0xcb,       /*   bne -                                          */
        0xf0, 0x32,       /*   beq $e18c  ; Go on to next search location     */

        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
    );

    /* new "TAKE" code */
    vpatch(rom, 0x0e2ea, 123,
        0xc9, 0x01,       /*   cmp #$01        ; Is it the armor?             */
        0xd0, 0x1b,       /*   bne +                                          */
        0xa5, 0xbe,       /*   lda $be         ; Load the equipment byte      */
        0x29, 0x1c,       /*   and #$1c        ; See if we have the armor     */
        0xc9, 0x1c,       /*   cmp #$1c                                       */
        0xf0, 0x4f,       /*   beq chest_empty ; The chest is empty           */
        0xa5, 0xbe,       /*   lda $be         ; Load the equipment byte      */
        0x09, 0x1c,       /*   ora #$1c        ; Add the armor                */
        0x85, 0xbe,       /*   sta $be         ; Save it                      */
        0x20, 0x55, 0xe3, /*   jsr take_content                               */
        0xa9, 0x28,       /*   lda #$28                                       */
        0x20, 0xf0, 0xdb, /*   jsr $dbf0       ; "You got the armor"          */
        0xa9, 0xd5,       /*   lda #$d5                                       */
        0x4c, 0x42, 0xd2, /*   jmp finish_text                                */
        0xc9, 0x08,       /* + cmp #$08    ; If it's not the flute.           */
        0xd0, 0x0b,       /*   bne +       ; Jump to the next check           */
        0xa9, 0x05,       /*   lda #$05    ; Load the flute inventory value   */
        0x20, 0x55, 0xe0, /*   jsr $e055   ; Jump to inventory check          */
        0xc9, 0xff,       /*   cmp #$ff    ; If return value is $ff (found)   */
        0xd0, 0x2f,       /*   bne chest_empty ; The chest is empty           */
        0xa9, 0x08,       /*   lda #$08    ; Reset the flute chest value      */
        0xc9, 0x0a,       /* + cmp #$0a    ; If it's not the token            */
        0xd0, 0x14,       /*   bne +       ; Jump to the next check           */
        0xa9, 0x07,       /*   lda #$07    ; Load the token inventory value   */
        0x20, 0x55, 0xe0, /*   jsr #$e055  ; Jump to inventory check          */
        0xc9, 0xff,       /*   cmp #$ff    ; If return value is $ff (found)   */
        0xd0, 0x20,       /*   bne chest_empty ; The chest is empty           */
        0xa9, 0x0e,       /*   lda #$0a    ; Reset the token chest value      */
        0x20, 0x55,  0xe0,
        0xc9, 0xff,
        0xd0, 0x17,
        0xa9, 0x0a,
        0xc9, 0x11,       /* + cmp #$11    ; If value is >= $11               */
        0xb0, 0x03,       /*   bcs +       ; Jump to the next check           */
        0x4c, 0x69, 0xe2, /*   jmp B3_e269 ; Add the item to the inventory    */
        0xa9, 0xff,       /* + lda #$ff    ;                                  */
        0x85, 0x3e,       /*   sta $3e     ;                                  */
        0xa9, 0xf4,       /*   lda #$f4    ;                                  */
        0x85, 0x00,       /*   sta $00     ;                                  */
        0xa9, 0x01,       /*   lda #$01    ;                                  */
        0x85, 0x01,       /*   sta $01     ;                                  */
        0xd0, 0x20,
        0xa5, 0x0e,
        0xc9, 0xff,
        0xa9, 0x00,
        0x85, 0x0e,
        0xb0, 0x03,
        0x4c, 0xa3, 0xe2,
        0x4c, 0xc8, 0xe1,
        0xa5, 0x0e,
        0xc9, 0xff,
        0xa9, 0x00,
        0x85, 0x0e,
        0xb0, 0x66,
        0xf0, 0x39,

        0xff, 0xff, 0xff, 0xff);
}

/**
 * Determines if a chest should be left alone when randomizing
 *
 * @param chest The chest to be checked
 */
static inline BOOL is_static_chest(dw_chest *chest)
{
    return (chest->map == TANTEGEL_THRONE_ROOM && chest->item == KEY) ||
                    chest->item == STAFF;
}

/**
 * Called when the player chooses not to shuffle items in chests. This sets
 * them to items found in the original game with a few exceptions:
 *   All gold chests aside from 500-755 gold no longer exist, so are replaced
 *     with big gold chests
 *   The tablet no longer exists, so is replaced with a Dragon's Scale
 *
 * @param rom The rom struct
 */
static void no_chest_shuffle(dw_rom *rom)
{
    size_t i;
    dw_chest *chest;
    uint8_t contents[] = {
            /* Tantegel */
            GOLD, GOLD, GOLD, GOLD, GOLD, TORCH,
            /* Rimuldar */
            WINGS,
            /* Garinham */
            GOLD, HERB, TORCH,
            /* Charlock Throne */
            HERB, GOLD, WINGS, KEY, CURSED_BELT, HERB,
            /* Tantegel basement */
            STONES,
            /* Garin's Grave */
            HERB, GOLD, GOLD, CURSED_BELT, HARP,
            /* Charlock dungeon */
            SWORD,
            /* Mountain Cave */
            NECKLACE, TORCH, RING, GOLD, HERB,
            /* Tablet cave (there is no longer a tablet in the game) */
            DRAGON_SCALE
    }, *cont = contents;
    uint8_t  search_items[] = {
           TOKEN, ARMOR, FLUTE
    };

    rewrite_search_take_code(rom, search_items);
    chest = rom->chests;
    for (i=0; i < CHEST_COUNT; i++) {
        /* don't move the staff or starting key */
        if (is_static_chest(chest)) {
            chest++;
            continue;
        }
        (chest++)->item = *(cont++);
    }
}

/**
 * Shuffles the contents of all chests in the game with the exception of the
 * starting throne room key and the staff of rain.
 *
 * @param rom The rom struct
 */
static void shuffle_chests(dw_rom *rom)
{
    size_t i;
    dw_chest *chest;
    uint8_t contents[] = {
            GOLD, GOLD, GOLD, GOLD,
            CURSED_BELT, CURSED_BELT, CURSED_BELT,
            GOLD, GOLD, GOLD, GOLD, GOLD, GOLD,
            WINGS, WINGS,
            KEY, KEY, KEY,
            HERB, HERB, HERB, HERB,
            TORCH, TORCH, TORCH,
            FAIRY_WATER, FAIRY_WATER,
            DRAGON_SCALE, RING
    }, *cont = contents;
    uint8_t  key_items[] = {
            SWORD, ARMOR, NECKLACE, HARP, STONES, FLUTE, TOKEN
    }, *key_item = key_items;
    uint8_t search_items[] = { 0, 0, 0 };

    if (!SHUFFLE_CHESTS(rom))
        return no_chest_shuffle(rom);

    do {
        mt_shuffle(key_items, sizeof(key_items), sizeof(uint8_t));
    } while (THREES_COMPANY(rom) && is_quest_item(key_items[0]));

    for (i=0; i < 3; i++) {
        /* fill in the search spots with a 80% chance unless cursed princess */
        /* is on, then always place to maximize cursed belts. */
        if (mt_rand(0, 4) || CURSED_PRINCESS(rom)) {
            search_items[i] = *(key_item++);
        }
    }
    rewrite_search_take_code(rom, search_items);

    while (key_item < (key_items + sizeof(key_items))) {
        /* overwite cursed belts with the remaining key items */
        *cont = *key_item++; 
        cont++;
    }

    printf("Shuffling chest contents...\n");
    cont = contents;
    chest = rom->chests;

    /* shuffle the contents and place them in chests */
    mt_shuffle(contents, sizeof(contents), sizeof(uint8_t));
    chest = rom->chests;
    cont = contents;
    for (i=0; i < CHEST_COUNT; i++) {
        /* don't move the staff or starting key */
        if (is_static_chest(chest)) {
            chest++;
            continue;
        }
        (chest++)->item = *(cont++);
    }

    check_quest_items(rom);
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

    if (!RANDOMIZE_PATTERNS(rom))
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

    if (!RANDOMIZE_SHOPS(rom))
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

    if (!RANDOMIZE_GROWTH(rom))
        return;

    printf("Randomizing stat growth...\n");

    for (i=0; i < 30; i++) {
        hp[i] =  (uint8_t)polyfit(mt_rand_double_ranged(1,30), &hero_hp_fac);
        mp[i] =  (uint8_t)polyfit(mt_rand_double_ranged(0,30), &hero_mp_fac);
        str[i] = (uint8_t)polyfit(mt_rand_double_ranged(1,30), &hero_str_fac);
        agi[i] = (uint8_t)polyfit(mt_rand_double_ranged(1,30), &hero_agi_fac);
    }

    qsort(hp,  30, sizeof(uint8_t), &compare);
    qsort(mp,  30, sizeof(uint8_t), &compare);
    qsort(str, 30, sizeof(uint8_t), &compare);
    qsort(agi, 30, sizeof(uint8_t), &compare);

    /* Give a little hp boost for swamp mode */
    if (BIG_SWAMP(rom)) {
        hp[0] += 10;
        for (i=1; i < 10; i++) {
            hp[i] = MAX(hp[i-1], hp[i] + 10 - i);
        }
        for (i=10; i < 30; i++) {
            hp[i] = MAX(hp[i-1], hp[i]);
        }
    }

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
                (j == HURTMORE && NO_HURTMORE(rom)))
                continue;
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
    // FIXME: this doesn't work with the new treasure code
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
    char *f, *fo, text[33];
    uint64_t flags;
    uint8_t *pos, *end;

//    pos = rom->title_text;
//    end = &rom->title_text + 143;
    pos = &rom->content[0x3f26];
    end = &rom->content[0x3fb5];
    text[32] = '\0';
    flags = rom->flags;
    f = text;
    fo = (char*)flag_order;

    printf("Updating title screen...\n");
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
    snprintf((char *)text, 33, "%"PRIu64, rom->seed);

    pos = pad_title_screen(pos, end, 15 + strlen(text)); /* blank line */
    pos = center_title_text(pos, text);         /* seed number */
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

    /* move the golem encounter to charlock */
    rom->golem_run->map = rom->golem->map = CHARLOCK_THRONE_ROOM;
    rom->golem_run->x = rom->golem->x = 25;
    rom->golem_run->y = rom->golem->y = 22;
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
    /* replace the usless grave warps with some for tantegel */
    vpatch(rom, 0xf44f, 3, 5, 1, 8);
    vpatch(rom, 0xf4e8, 3, 4, 1, 7);
    vpatch(rom, 0x1288, 1, 0x22);  /* remove the top set of stairs for the old warp in the grave */
    /* Sets the encounter rate of Zone 0 to be the same as other zones. */
    vpatch(rom, 0xcebf, 3, 0x4c, 0x04, 0xcf);  /* skip over the zone 0 code */
    vpatch(rom, 0xe260, 1, 0);  /* set death necklace chance to 100% */
    vpatch(rom, 0xe74d, 1, 9);  /* buff the hurt spell */
    vpatch(rom, 0xdbc1, 1, 18);  /* buff the heal spell */
    vpatch(rom, 0xea41, 5, 0xad, 0x07, 0x01, 0xea, 0xea); /* I forget what this does */
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

    vpatch(rom, 0x05834,  162,
        0x63,  0x20, /* PPU ADDR */
        /* D      R      A      G      O      N       */
        0x27,  0x35,  0x24,  0x2a,  0x32,  0x31,  0x5f,
        /* W      A      R      R      I      O      R        */
        0x3a,  0x24,  0x35,  0x35,  0x2c,  0x32,  0x35,  0x5f,
        /* R      A      N      D      O      M      I      Z      E     R */
        0x35,  0x24,  0x31,  0x27,  0x32,  0x30,  0x2c,  0x3d,  0x28, 0x35,
        /* END */
        0xfc,

        0xc3,  0x20, /* PPU ADDR */
        /* D      E      V      E      L      O      P      E      R    END */
        0x27,  0x28,  0x39,  0x28,  0x2f,  0x32,  0x33,  0x28,  0x35,  0xfc,

        0xd7,  0x20, /* M      C      G      R      E      W    END */
        0x30,  0x26,  0x2a,  0x35,  0x28,  0x3a,  0xfc,

        0x23,  0x21, /* PPU ADDR */
        /* C      O      N      T      R      I      B      U      T      O */
        0x26,  0x32,  0x31,  0x37,  0x35,  0x2c,  0x25,  0x38,  0x37,  0x32,
        /* R      S    END */
        0x35,  0x36,  0xfc,

        0x34,  0x21, /* PPU ADDR */
        /* G      A      M      E      B      O      Y      F      9    END */
        0x2a,  0x24,  0x30,  0x28,  0x25,  0x32,  0x3c,  0x29,  0x09,  0xfc,

        0x74,  0x21, /* PPU ADDR */
        /* C      A      I      T      S      I      T      H      2    END */
        0x26,  0x24,  0x2c,  0x37,  0x36,  0x2c,  0x37,  0x2b,  0x02,  0xfc,

        0xc3,  0x21, /* PPU ADDR */
        /* S      P      R      I      T      E        */
        0x36,  0x33,  0x35,  0x2c,  0x37,  0x28,  0x5f,
        /* W      O      R      K    END */
        0x3a,  0x32,  0x35,  0x2e,  0xfc,

        0xd5,  0x21, /* PPU ADDR */
        /* X      A      R      N      A      X      4      2    END */
        0x3b,  0x24,  0x35,  0x31,  0x24,  0x3b,  0x04,  0x02,  0xfc,

        0x13,  0x22, /* PPU ADDR */
        /* R      Y      U      S      E      I      S      H      I      N */
        0x35,  0x3c,  0x38,  0x36,  0x28,  0x2c,  0x36,  0x2b,  0x2c,  0x31,
        /* END */
        0xfc,

        0x57,  0x22, /* PPU ADDR */
        /* M      C      G      R      E      W    END */
        0x30,  0x26,  0x2a,  0x35,  0x28,  0x3a,  0xfc,

        /* blank space */
        0x5f,  0x5f,  0x5f,  0x5f,  0x5f,  0x5f,  0x5f,  0x5f,  0x5f,  0x5f,
        0x5f,  0x5f,  0x5f,  0x5f,  0x5f,  0x5f,  0x5f,  0x5f,  0x5f,  0x5f,
        0x5f,  0x5f,  0x5f,  0x5f,  0x5f,  0x5f,  0x5f
    );

    /* palette edits */
    vpatch(rom, 0x058db,    8,  0x55,  0x07,  0xf7,  0x04,  0xa3,  0xf7,  0x05,  0x55);
    vpatch(rom, 0x058e8,    1,  0x55);
    vpatch(rom, 0x058f1,    1,  0xaa);

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
        vpatch(rom, 0xcf20, 2, 0xa9, 0x01);

    if (PERMANENT_REPEL(rom))
        vpatch(rom, 0xcf26, 2, 0xa9, 0xff);
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
        if (dx > 100 || dy > 100) {
            snprintf((char*)text2, 74, "From Tantegel Castle travel %3d "
                    "leagues to the %s and %3d %s/    ",
                    ABS(dy), (dy < 0) ? "north" : "south",
                    ABS(dx), (dx < 0) ? "west" : "east");
        } else {
            snprintf((char*)text2, 74, "From Tantegel Castle travel %2d "
                    "leagues to the %s and %2d to the %s/",
                    ABS(dy), (dy < 0) ? "north" : "south",
                    ABS(dx), (dx < 0) ? "west" : "east");
        }
    }
    ascii2dw(text2);
    patch(rom, 0xa242, 70, text2);
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

    while(!map_generate_terrain(&rom)) {}
    shuffle_chests(&rom);
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
    open_charlock(&rom);
    short_charlock(&rom);
    no_keys(&rom);
    cursed_princess(&rom);
    threes_company(&rom);
    scared_metal_slimes(&rom);
    torch_in_battle(&rom);
    repel_mods(&rom);
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
    noir_mode(&rom);

    printf("Checksum: %016"PRIx64"\n", crc);
    if (!dwr_write(&rom, output_file)) {
        return 0;
    }
    free(rom.header);
    return crc;
}
