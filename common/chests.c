
#include <string.h>
#include <stdio.h>

#include "dwr.h"
#include "mt64.h"
#include "patch.h"
#include "dungeon_maps.h"

/**
 * Returns a random index of a chest that is not in charlock. For simplicity,
 * this function will also not return the starting throne room key chest or
 * the staff of rain chest.
 */
static inline int non_charlock_chest(dw_rom *rom)
{
    int chest;

    while (TRUE) {
        chest = (int)mt_rand(0, 30);
        if (!(rom->chest_access[chest] & 0x88))
            break;
    }
    return chest;
}

/**
 * Determines whether or not the chest item is a quest item.
 *
 * @param item The item member of a chest
 * @return A boolean indicated whether or not the item is crucial for the quest
 *      to obtain the rainbow drop.
 */
BOOL is_quest_item(uint8_t item)
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
 * Looks for quest items which may be in Charlock due to shuffling and moves
 * them to a chest outside Charlock.
 *
 * @param rom The rom struct.
 */
void check_quest_items(dw_rom *rom)
{
    int i, tmp_index;
    uint8_t tmp_item;

    printf("Checking quest item placement...\n");

    for (i=0; i < CHEST_COUNT; i++) {
        if (rom->chest_access[i] & 0x88) {
            if (is_quest_item(rom->chests[i].item)) {
                do {
                    tmp_index = non_charlock_chest(rom);
                } while (is_quest_item(rom->chests[tmp_index].item) 
                        || is_static_chest(&rom->chests[tmp_index]));
                tmp_item = rom->chests[tmp_index].item;
                rom->chests[tmp_index].item = rom->chests[i].item;
                rom->chests[i].item = tmp_item;
            }
        }
    }
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
    vpatch(rom, 0x0e231,    1,  0x55);
    vpatch(rom, 0x0e23b,    1,  0x55);
    vpatch(rom, 0x0e26a,    1,  0x55);
    vpatch(rom, 0x0e2ad,    1,  0x55);
    vpatch(rom, 0x0e378,    1,  0x55);

    /* new "SEARCH" code */
    vpatch(rom, 0x0e10b,  43,
        0xa2, 0x02,        /*     ldx #02                                    */
        0xa5, 0x45,        /* -   lda MAP_INDEX                              */
        0xdd, 0x36, 0xe1,  /*     cmp search_map,x                           */
        0xd0, 0x1c,        /*     bne +                                      */
        0xa5, 0x3a,        /*     lda X_POS                                  */
        0xdd, 0x39, 0xe1,  /*     cmp search_x,x                             */
        0xd0, 0x15,        /*     bne +                                      */
        0xa5, 0x3b,        /*     lda Y_POS                                  */
        0xdd, 0x3c, 0xe1,  /*     cmp search_y,x                             */
        0xd0, 0x0e,        /*     bne +                                      */
        0xbd, 0x3f, 0xe1,  /*     lda search_item,x                          */
        0xf0, 0x63,        /*     beq $e18a                                  */
        0x48,              /*     pha                                        */
        0xa9, 0xff,        /*     lda #$ff                                   */
        0x85, 0x0e,        /*     sta $0e                                    */
        0x68,              /*     pla                                        */
        0x4c, 0x24,        /*     jmp $e224                                  */
        0xe2, 0xca,        /* +   dex                                        */
        0x10, 0xda,        /*     bpl -                                      */
        0x4c, 0x8a, 0xe1   /*     jmp $e18a                                  */
    );

    /* Searchable item table */
    vpatch(rom, 0x0e136,  12,
    /* search_map:  */
        OVERWORLD,      KOL, HAUKSNESS,
    /* search_x:    */
             0x53,     0x09,      0x12,
    /* search_y:    */
             0x71,     0x06,      0x0c,
    /* search_item: */
         items[0], items[1],  items[2]
    );
    /* This is freed up space. Mark it with ff so we can find it later. */
    memset((void*)&rom->content[0xe142], 0xff, 72);

    /* new "TAKE" code */
    vpatch(rom, 0x0e2ea,  123,
        0xc9,  0x01,        /*     cmp #$01    ; Is it the armor?            */
        0xd0,  0x1b,        /*     bne +                                     */
        0xa5,  0xbe,        /*     lda $be     ; Load the equipment byte     */
        0x29,  0x1c,        /*     and #$1c    ; See if we have the armor    */
        0xc9,  0x1c,        /*     cmp #$1c                                  */
        0xf0,  0x4f,        /*     beq ++      ; The chest is empty          */
        0xa5,  0xbe,        /*     lda $be     ; Load the equipment byte     */
        0x09,  0x1c,        /*     ora #$1c    ; Add the armor               */
        0x85,  0xbe,        /*     sta $be     ; Save it                     */
        0x20,  0x55,  0xe3, /*     jsr +++                                   */
        0xa9,  0x28,        /*     lda #$28                                  */
        0x20,  0xf0,  0xdb, /*     jsr b3_dbf0 ; "You got the armor"         */
        0xa9,  0xd5,        /*     lda #$d5                                  */
        0x4c,  0x42,  0xd2, /*     jmp b3_d242                               */
                            /*                                               */
        0xc9,  0x08,        /* +   cmp #$08    ; If it's not the flute.      */
        0xd0,  0x0b,        /*     bne +       ; Jump to the next check      */
        0xa9,  0x05,        /*     lda #$05    ; Load the flute inv value    */
        0x20,  0x55,  0xe0, /*     jsr $e055   ; Jump to inventory check     */
        0xc9,  0xff,        /*     cmp #$ff    ; Return value is $ff (found) */
        0xd0,  0x2f,        /*     bne ++      ; The chest is empty          */
        0xa9,  0x08,        /*     lda #$08    ; Reset the flute chest value */
        0xc9,  0x0a,        /* +   cmp #$0a    ; If it's not the token       */
        0xd0,  0x14,        /*     bne +       ; Jump to the next check      */
        0xa9,  0x07,        /*     lda #$07    ; Load the token inv value    */
        0x20,  0x55,  0xe0, /*     jsr $e055   ; Jump to inventory check     */
        0xc9,  0xff,        /*     cmp #$ff    ; Return value is $ff (found) */
        0xd0,  0x20,        /*     bne ++      ; The chest is empty          */
        0xa9,  0x0e,        /*     lda #$0e    ; Reset the token chest value */
        0x20,  0x55,  0xe0, /*     jsr $e055                                 */
        0xc9,  0xff,        /*     cmp #$ff                                  */
        0xd0,  0x17,        /*     bne ++                                    */
        0xa9,  0x0a,        /*     lda #$0a                                  */
        0xc9,  0x11,        /* +   cmp #$11    ; If value is >= $11          */
        0xb0,  0x03,        /*     bcs +       ; Jump to the next check      */
        0x4c,  0x69,  0xe2, /*     jmp b3_e269 ; Add the item                */
                            /*                                               */
        0xa9,  0xff,        /* +   lda #$ff                                  */
        0x85,  0x3e,        /*     sta $3e                                   */
        0xa9,  0xf4,        /*     lda #$f4                                  */
        0x85,  0x00,        /*     sta $00                                   */
        0xa9,  0x01,        /*     lda #$01                                  */
        0x85,  0x01,        /*     sta $01                                   */
        0xd0,  0x20,        /*     bne b3_e365                               */
        0xa5,  0x0e,        /* ++  lda $0e                                   */
        0xc9,  0xff,        /*     cmp #$ff                                  */
        0xa9,  0x00,        /*     lda #$00                                  */
        0x85,  0x0e,        /*     sta $0e                                   */
        0xb0,  0x03,        /*     bcs +                                     */
        0x4c,  0xa3,  0xe2, /*     jmp b3_e2a3                               */
        0x4c,  0xc8,  0xe1, /* +   jmp b3_e1c8                               */
                            /*                                               */
                            /*     ; Check the "searched" flag and clear it. */
        0xa5,  0x0e,        /* +++ lda $0e     ; Load the flag               */
        0xc9,  0xff,        /*     cmp #$ff    ; FF indicates search item.   */
        0xa9,  0x00,        /*     lda #$00    ; Clear the flag              */
        0x85,  0x0e,        /*     sta $0e                                   */
        0xb0,  0x66,        /*     bcs b3_e3c5 ; Search: don't play sound    */
        0xf0,  0x39,        /*     beq b3_e39a ; It's a chest.               */ 
        0xff,  0xff,  0xff,  0xff);
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
           TOKEN, FLUTE, ARMOR
    };
    if (NO_NUMBERS(rom) || THREES_COMPANY(rom)) {
        contents[28] = TOKEN;
        search_items[0] = 0;
    }

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

static BOOL is_dungeon_tileset(dw_map_index map)
{
    switch(map) {
        case NO_MAP:
        case OVERWORLD:
        case CHARLOCK:
        case HAUKSNESS:
        case TANTEGEL:
        case TANTEGEL_THRONE_ROOM:
        case CHARLOCK_THRONE_ROOM:
        case KOL:
        case BRECCONARY:
        case GARINHAM:
        case CANTLIN:
        case RIMULDAR:
        case TANTEGEL_BASEMENT:
        case NORTHERN_SHRINE:
        case SOUTHERN_SHRINE:
            return FALSE;
        case CHARLOCK_CAVE_1:
        case CHARLOCK_CAVE_2:
        case CHARLOCK_CAVE_3:
        case CHARLOCK_CAVE_4:
        case CHARLOCK_CAVE_5:
        case CHARLOCK_CAVE_6:
        case SWAMP_CAVE:
        case MOUNTAIN_CAVE:
        case MOUNTAIN_CAVE_2:
        case GARINS_GRAVE_1:
        case GARINS_GRAVE_2:
        case GARINS_GRAVE_3:
        case GARINS_GRAVE_4:
        case ERDRICKS_CAVE:
        case ERDRICKS_CAVE_2:
        default:
                return TRUE;
    }
}

static int chest_bin(dw_map_index map) {
    switch(map) {
        case TANTEGEL:
            return 0;
        case GARINS_GRAVE_1:
        case GARINS_GRAVE_2:
        case GARINS_GRAVE_3:
        case GARINS_GRAVE_4:
            return 1;
        case MOUNTAIN_CAVE:
        case MOUNTAIN_CAVE_2:
            return 2;
        case CHARLOCK:
        case CHARLOCK_CAVE_1:
        case CHARLOCK_CAVE_2:
        case CHARLOCK_CAVE_3:
        case CHARLOCK_CAVE_4:
        case CHARLOCK_CAVE_5:
        case CHARLOCK_CAVE_6:
        case CHARLOCK_THRONE_ROOM:
            return 3;
        default:
            return -1;
    }
}

/** Checks to see if a chest already exists at the current x/y coordinates.
 * This will avoid conflicts since 2 chests cannot exist at the same
 * coordinates where the player can travel to both locations without crossing
 * the overwold. With stair shuffle coming, lets just avoid all conflicts.
 *
 * @param rom The rom struct
 * @param x The x coordinate
 * @param y The y coordinate
 * @param until The number of chests to check from the beginning.
 *
 * @return A pointer to the conflicting chest.
 */
static dw_chest *chest_at(dw_rom *rom, uint8_t x, uint8_t y, size_t until) {
    dw_chest *chest;
    dw_chest *stop = rom->chests + until;
    for (chest = rom->chests; chest < stop; chest++) {
        if(chest->x == x && chest->y == y) {
            return chest;
        }
    }
    return NULL;
}

/**
 * Randomizes the placment of chests to a subset of predetermined locations
 *
 * @param rom The rom struct
 */
static void move_chests(dw_rom *rom)
{
    size_t i, j;
    int bin, map;
    uint8_t *chest_spot, *new_door;
    uint8_t chest_bins[4] = { 0, 0, 0, 0};
    uint8_t chest_spots[][3] = {
//         {TANTEGEL_THRONE_ROOM,  4,  4}, /* vanilla */
//         {TANTEGEL_THRONE_ROOM,  5,  4}, /* vanilla */
//         {TANTEGEL_THRONE_ROOM,  6,  1}, /* vanilla */
        {TANTEGEL,              1, 13}, /* vanilla */
        {TANTEGEL,              2, 13},
        {TANTEGEL,              3, 13}, /* vanilla */
        {TANTEGEL,              1, 14},
        {TANTEGEL,              2, 14}, /* vanilla */
        {TANTEGEL,              3, 14},
        {TANTEGEL,              1, 15}, /* vanilla */
        {TANTEGEL,              2, 15},
        {TANTEGEL,              3, 15}, /* vanilla */
        {TANTEGEL,             16, 15},
        {TANTEGEL,             24, 21},
        {TANTEGEL_BASEMENT,     4,  5}, /* vanilla */
        {TANTEGEL_BASEMENT,     4,  8},
        {TANTEGEL_BASEMENT,     5,  8},
        {BRECCONARY,            3, 21},
        {BRECCONARY,           10, 23},
//         {BRECCONARY,           21,  3},
        {ERDRICKS_CAVE,         9,  0},
        {ERDRICKS_CAVE_2,       1,  9},
        {ERDRICKS_CAVE_2,       9,  3}, /* vanilla */
        {GARINHAM,              4,  5},
        {GARINHAM,              8,  5}, /* vanilla */
        {GARINHAM,              8,  6}, /* vanilla */
        {GARINHAM,              9,  5}, /* vanilla */
        {GARINHAM,             18, 18},
        {GARINS_GRAVE_1,       11,  0}, /* vanilla */
        {GARINS_GRAVE_1,       12,  0}, /* vanilla */
        {GARINS_GRAVE_1,       13,  0}, /* vanilla */
        {GARINS_GRAVE_1,        0, 14},
        {GARINS_GRAVE_1,        7,  5},
        {GARINS_GRAVE_1,        4, 15},
        {GARINS_GRAVE_3,        1,  1}, /* vanilla */
        {GARINS_GRAVE_3,       13,  6}, /* vanilla */
        {GARINS_GRAVE_3,       13, 10},
        {GARINS_GRAVE_3,       17, 17},
        {KOL,                  22,  1},
        {KOL,                   3, 13},
        {SWAMP_CAVE,            5,  0},
        {SWAMP_CAVE,            4, 19},
        {SWAMP_CAVE,            5, 29},
        {RIMULDAR,             24, 23}, /* vanilla */
        {RIMULDAR,             17, 23},
        {RIMULDAR,              3, 20},
        {MOUNTAIN_CAVE,        13,  5}, /* vanilla */
        {MOUNTAIN_CAVE,        13,  9},
        {MOUNTAIN_CAVE_2,       1,  1},
        {MOUNTAIN_CAVE_2,      13,  9},
        {MOUNTAIN_CAVE_2,       0,  3},
        {MOUNTAIN_CAVE_2,       1,  6}, /* vanilla */
        {MOUNTAIN_CAVE_2,       2, 11},
        {MOUNTAIN_CAVE_2,       2,  2}, /* vanilla */
        {MOUNTAIN_CAVE_2,       3,  2}, /* vanilla */
        {MOUNTAIN_CAVE_2,      10,  9}, /* vanilla */
        {HAUKSNESS,             6,  9},
        {CANTLIN,               2, 27},
        {CANTLIN,              12,  2},
        {CHARLOCK_CAVE_1,      19,  9},
        {CHARLOCK_CAVE_1,       2, 17},
        {CHARLOCK_CAVE_1,       8,  6},
        {CHARLOCK_CAVE_1,      11,  6},
        {CHARLOCK_CAVE_1,      19,  6},
        {CHARLOCK_CAVE_2,       5,  5}, /* vanilla */
        {CHARLOCK_CAVE_3,       3,  8},
        {CHARLOCK_CAVE_4,       6,  4},
        {CHARLOCK_CAVE_5,       2,  0},
        {CHARLOCK_CAVE_6,       9,  0},
        {CHARLOCK_THRONE_ROOM,  5,  2},
        {CHARLOCK_THRONE_ROOM,  6, 12},
        {CHARLOCK_THRONE_ROOM,  8, 12},
        {CHARLOCK_THRONE_ROOM, 11, 11}, /* vanilla */
        {CHARLOCK_THRONE_ROOM, 11, 12}, /* vanilla */
        {CHARLOCK_THRONE_ROOM, 11, 13}, /* vanilla */
        {CHARLOCK_THRONE_ROOM, 12, 11},
        {CHARLOCK_THRONE_ROOM, 12, 12}, /* vanilla */
        {CHARLOCK_THRONE_ROOM, 12, 13}, /* vanilla */
        {CHARLOCK_THRONE_ROOM, 13, 11},
        {CHARLOCK_THRONE_ROOM, 13, 12},
        {CHARLOCK_THRONE_ROOM, 13, 13}, /* vanilla */
        {CHARLOCK_THRONE_ROOM, 22, 26},
    };

    uint8_t new_doors[][3] = {
        {BRECCONARY,            8, 23},
        {GARINHAM,             16, 18},
        {KOL,                  20,  2},
        {RIMULDAR,             17, 21},
        {HAUKSNESS,            10,  8},
        {CANTLIN,               3, 24},
        {CANTLIN,              10,  4},
        {CHARLOCK_THRONE_ROOM,  4,  9},
        {CHARLOCK_THRONE_ROOM,  7, 22},
    };

    if (!RANDOM_CHEST_LOCATIONS(rom)) {
        return;
    }

    printf("Removing all chests...\n");
    for (i=0; i < sizeof(chest_spots) / 3; i++) {
        chest_spot = chest_spots[i];
        if (is_dungeon_tileset(chest_spot[0])) {
            set_dungeon_tile(rom, chest_spot[0], chest_spot[1], chest_spot[2],
                    DUNGEON_TILE_BRICK);
        } else {
            set_dungeon_tile(rom, chest_spot[0], chest_spot[1], chest_spot[2],
                    TOWN_TILE_BRICK);
        }
    }
    printf("Adding doors...\n");
    for (i=0; i < sizeof(new_doors) / 3; i++) {
        new_door = new_doors[i];
        if (is_dungeon_tileset(new_door[0])) {
            set_dungeon_tile(rom, new_door[0], new_door[1], new_door[2],
                    DUNGEON_TILE_DOOR);
        } else {
            set_dungeon_tile(rom, new_door[0], new_door[1], new_door[2],
                    TOWN_TILE_DOOR);
        }
    }
    printf("Adding back chests...\n");
chest_placement_retry:
    mt_shuffle(chest_spots, sizeof(chest_spots) / 3, 3);
    for (i=0, j=0; i < CHEST_COUNT; i++, j++) {
        map = rom->chests[i].map;
         /* don't mess with these */
        if (map == TANTEGEL_THRONE_ROOM || map == NORTHERN_SHRINE)
            continue;
        while (chest_at(rom, chest_spots[j][1], chest_spots[j][2], i+3)) {
            j++;
        }
        bin = chest_bin(chest_spots[j][0]);
        while (bin >= 0 && chest_bins[bin] > 8) {
            bin = chest_bin(chest_spots[++j][0]);
        }
        if (j >= sizeof(chest_spots) / 3) {
            /* this should never happen, but just in case */
            printf("Something went wrong, retrying chest placement...\n");
            goto chest_placement_retry;
        }
        if (bin >= 0) {
            chest_bins[bin]++;
        }
        rom->chests[i].map = chest_spots[j][0];
        rom->chests[i].x = chest_spots[j][1];
        rom->chests[i].y = chest_spots[j][2];
        if (is_dungeon_tileset(rom->chests[i].map)) {
            set_dungeon_tile(rom, rom->chests[i].map, rom->chests[i].x,
                    rom->chests[i].y, DUNGEON_TILE_CHEST);
        } else {
            set_dungeon_tile(rom, rom->chests[i].map, rom->chests[i].x,
                    rom->chests[i].y, TOWN_TILE_CHEST);
        }
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

    mt_shuffle(key_items, sizeof(key_items), sizeof(uint8_t));

    for (i=0; i < 3; i++) {
        /* If "No Numbers" is turned on, don't put anything on the overworld */
        if (NO_NUMBERS(rom) && !i) {
            /* If curse princess is on, add an extra cursed belt to make sure 
             * there are three */
            if (CURSED_PRINCESS(rom)) {
                contents[7] = CURSED_BELT;
            }
            continue;
        }

        /* fill in the search spots with a 80% chance unless cursed princess */
        /* is on, then always place to maximize cursed belts. */
        if (mt_rand(0, 4) || CURSED_PRINCESS(rom)) {
            /* If we're putting something on the overworld and cursed princess
             * is on, make sure it's not vital */
            while (!i && THREES_COMPANY(rom) && is_quest_item(key_items[0])) {
                mt_shuffle(key_items, sizeof(key_items), sizeof(uint8_t));
            }
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

}

void do_chest_flags(dw_rom *rom) {
    move_chests(rom);
    shuffle_chests(rom);
}

