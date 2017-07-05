
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <math.h>

#include "dwr.h"
#include "mt64.h"
#include "crc64.h"
#include "map.h"

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

const int CHEST_COUNT = 31;

const char flag_order[] = 
        "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

/* for qsort() */
static int compare(const void *a, const void *b)
{
    return *(uint8_t*)a - *(uint8_t*)b;
}

static void parse_flags(dw_rom *rom, char *flags)
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
}

void dwr_init(dw_rom *rom, const char *input_file, char *flags)
{
    FILE *input;
    int read;

    rom->data = malloc(ROM_SIZE);
    memset(rom->data, 0, ROM_SIZE);
    input = fopen(input_file, "rb");
    if (!input) {
        fprintf(stderr, "Unable to open ROM file '%s'", input_file);
        exit(1);
    }
    read = fread(rom->data, 1, ROM_SIZE, input);
    if (read < ROM_SIZE) {
        fprintf(stderr, "File '%s' is too small and may be corrupt, aborting.",
                input_file);
        exit(1);
    }
    fclose(input);

    parse_flags(rom, flags);
    /* subtract 0x9d5d from these pointers */
    rom->map.pointers = (uint16_t*)&rom->data[0x2663];
    rom->map.encoded = &rom->data[0x1d6d];
    rom->map.meta = (dw_map_meta*)&rom->data[0x2f];
    rom->map.warps_from = (dw_warp*)&rom->data[0xf3d8];
    rom->map.warps_to   = (dw_warp*)&rom->data[0xf471];
    rom->map.love_calc = (dw_love_calc*)&rom->data[0xdf4a];
    rom->map.return_point = (dw_return_point*)&rom->data[0xdb11];
    rom->map.rainbow_drop = (dw_rainbow_drop*)&rom->data[0xde9b];
    rom->map.rainbow_bridge = (dw_rainbow_drop*)&rom->data[0x2c4b];
    rom->stats = (dw_stats*)&rom->data[0x60dd];
    rom->new_spells = (dw_new_spell*)&rom->data[0xeaf8];
    rom->mp_reqs = &rom->data[0x1d63];
    rom->xp_reqs = (uint16_t*)&rom->data[0xf36b];
    rom->enemies = (dw_enemy*)&rom->data[0x5e5b];
    rom->zones = &rom->data[0xf55f];
    rom->zone_layout = &rom->data[0xf532];
    rom->chests = (dw_chest*)&rom->data[0x5ddd];
    rom->encounters[0] = (dw_forced_encounter*)&rom->data[0xcd61];
    rom->encounters[1] = (dw_forced_encounter*)&rom->data[0xcd78];
    rom->encounters[2] = (dw_forced_encounter*)&rom->data[0xcd95];
    rom->encounter_types[0] = &rom->data[0xcd74];
    rom->encounter_types[1] = &rom->data[0xcd87];
    rom->encounter_types[2] = &rom->data[0xcd95];
    rom->token = (dw_searchable*)&rom->data[0xe11b];
    rom->flute = (dw_searchable*)&rom->data[0xe15a];
    rom->armor = (dw_searchable*)&rom->data[0xe170];
    rom->weapon_shops = &rom->data[0x19a1];
    rom->music = &rom->data[0x31bf];
    rom->title_text = &rom->data[0x3f36];

    map_decode(&rom->map);
}

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

size_t dw2ascii(uint8_t *string, size_t bufsize)
{
    int i, alphalen;

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

/* 
 * Returns a random index of a chest that is not in charlock
 */
static inline int non_charlock_chest()
{
    int chest;

    chest = (int)mt_rand(0, 22);
    /* avoid 11-16 and chest 24 (they are in charlock) */
    if (chest >= 11) chest += 6;
    if (chest >= 24) chest += 1;
    return chest;
}

/*
 * Determines whether or not the chest item is a quest item.
 */
static inline bool is_quest_item(uint8_t item)
{
    switch(item) {
        case TOKEN:
        case STONES:
        case HARP:
        case STAFF:
            return true;
        default:
            return false;
    }
}

static inline void check_quest_items(dw_rom *rom)
{
    int i, tmp_index;
    uint8_t tmp_item;

    for (i=11; i <= 16; i++) {
        if (is_quest_item(rom->chests[i].item)) {
            do {
                tmp_index = non_charlock_chest();
            } while(is_quest_item(rom->chests[tmp_index].item));
            tmp_item = rom->chests[tmp_index].item;
            rom->chests[tmp_index].item = rom->chests[i].item;
            rom->chests[i].item = tmp_item;
        }
    }
    if (is_quest_item(rom->chests[24].item)) {
        do {
            tmp_index = non_charlock_chest();
        } while(is_quest_item(rom->chests[tmp_index].item));
        tmp_item = rom->chests[tmp_index].item;
        rom->chests[tmp_index].item = rom->chests[24].item;
        rom->chests[24].item = tmp_item;
    }
}

static void shuffle_chests(dw_rom *rom) {
    int i;
    dw_chest *chest;
    uint8_t *cont, contents[CHEST_COUNT-2];

    if (!(rom->flags & FLAG_C))
        return;

    printf("Shuffling chest contents...\n");
    cont = contents;
    chest = rom->chests;
    
    /* load up the chest contents into an array */
    for (i=0; i < CHEST_COUNT; i++) {
        /* don't move the staff or starting key*/
        if ((chest->map == TANTEGEL_THRONE_ROOM && chest->item == KEY) ||
                    chest->item == STAFF) { 
            chest++;
            continue;
        }
        switch((dw_chest_content)chest->item) {
            case GOLD_5: /* change all gold to large gold */
            case GOLD_6:
            case GOLD_10:
                *(cont++) = GOLD_500;
                break;
            case GOLD_120:
                *(cont++) = KEY;
                break;
            case TABLET: /* remove the tablet */
                if (mt_rand_bool()) { /* 50/50 chance */
                    rom->token->map = NO_MAP; /* remove token */
                    *(cont++) = TOKEN; /* put it in a chest */
                } else {
                    *(cont++) = KEY; /* replace with a key */
                }
                break;
            default:
                *(cont++) = chest->item;
                break;
        }
        chest++;
    }

    /* shuffle the contents and place them back in chests */
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

}

static void randomize_attack_patterns(dw_rom *rom)
{
    int i;

    if (!(rom->flags & FLAG_P))
        return;

    printf("Randomizing enemy attack patterns\n");

    for (i=SLIME; i < DRAGONLORD_1; i++) {
        if (mt_rand_bool()) {
            rom->enemies[i].pattern = mt_rand(0, 255);
            rom->enemies[i].s_ss_resist &= 0xf0;
            rom->enemies[i].s_ss_resist |= mt_rand(0, i/3);
        } else {
            rom->enemies[i].pattern = 0;
            /* no spells, max out resistance */
            rom->enemies[i].s_ss_resist |= 0xf;
        }
    }
}

//void randomize_towns(dw_rom *rom)
//{
//}

static void randomize_music(dw_rom *rom)
{
    int i;
    
    if (!(rom->flags & FLAG_K))
        return;

    printf("Randomizing game music\n");

    uint8_t choices[] = {0x1, 0x1, 0x1, 0x2, 0x2, 0x2, 0x3, 0x3, 0x3, 0x4, 0x4, 
                         0x4, 0x5, 0x5, 0x5, 0xf, 0xf, 0xf, 0x10, 0x10, 0x10,
                         0x6, 0x7, 0x8, 0x9, 0xa, 0xb, 0xc, 0xd};


    for (i=0; i < 29; i++) {
        rom->music[i] = choices[mt_rand(0, sizeof(choices)-1)];
    }
}

static void disable_music(dw_rom *rom)
{
    if (!(rom->flags & FLAG_Q))
        return;

    printf("Disabling game music\n");

    memset(rom->music, 0, 29);
}

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

static void randomize_zone_layout(dw_rom *rom)
{
    int i;
    dw_warp *tantegel = &rom->map.warps_from[WARP_TANTEGEL];

    printf("Randomizing enemy zone layout\n");

    for (i=0; i < 31; i++) {
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

static void randomize_zones(dw_rom *rom)
{
    int i, zone;


    if (!(rom->flags & FLAG_Z))
        return;

    printf("Randomizing monsters in enemy zones\n");

    zone = 0;  /* tantege zone */
    for (i=0; i < 5; i++) { 
        rom->zones[zone * 5 + i] = mt_rand(SLIME, SCORPION);
    }

    for (zone=1; zone < 2; zone++) { /* tantegel adjacent zones */
        for (i=0; i < 5; i++) {
            rom->zones[zone * 5 + i] = mt_rand(SLIME, WOLF);
        }
    }

    for (zone=3; zone < 15; zone++) { /* overworld/hybrid zones */
        for (i=0; i < 5; i++) {
            rom->zones[zone * 5 + i] = mt_rand(SLIME, RED_DRAGON);
        }
    }

    for (zone=16; zone < 18; zone++) { /* charlock */
        for (i=0; i < 5; i++) {
            rom->zones[zone * 5 + i] = mt_rand(WEREWOLF, RED_DRAGON);
        }
    }
    zone = 19;  /* swamp cave */
    for (i=0; i < 5; i++) {
        rom->zones[zone * 5 + i] = mt_rand(SLIME, RED_DRAGON);
    }
}

static void randomize_shops(dw_rom *rom)
{
    uint8_t *shop_start, *shop_item;
    int i, six_item_shop;
    dw_shop_item items[15] = {
            BAMBOO_POLE, CLUB, COPPER_SWORD, HAND_AXE, BROAD_SWORD, FLAME_SWORD,
            CLOTHES, LEATHER_ARMOR, CHAIN_MAIL, HALF_PLATE, FULL_PLATE,
            MAGIC_ARMOR, SMALL_SHIELD, LARGE_SHIELD, SILVER_SHIELD
    };

    if (!(rom->flags & FLAG_W))
        return;

    printf("Randomizing weapon shop inventory\n");
    
    six_item_shop = mt_rand(0, 6);
    shop_item = rom->weapon_shops;
    
    for (i=0; i < 7; i++) {
        shop_start = shop_item;
        *(shop_item++) = items[mt_rand(0, 14)];
        *(shop_item++) = items[mt_rand(0, 14)];
        *(shop_item++) = items[mt_rand(0, 14)];
        *(shop_item++) = items[mt_rand(0, 14)];
        *(shop_item++) = items[mt_rand(0, 14)];
        if (i == six_item_shop) {
            *(shop_item++) = items[mt_rand(0, 14)];
            qsort(shop_start, 6, sizeof(uint8_t), &compare);
        } else {
            qsort(shop_start, 5, sizeof(uint8_t), &compare);
        }
        *(shop_item++) = SHOP_END;
    }
}

static void shuffle_searchables(dw_rom *rom)
{
    dw_searchable searchables[3];

    if (!(rom->flags & FLAG_I))
        return;

    printf("Shuffling searchable items\n");

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

static uint8_t inverted_power_curve(uint8_t min, uint8_t max, double power)
{
    double p_range;
    
    p_range= pow((double)(max - min), 1 / power);
    return min + round(max - pow((mt_rand_double() * p_range), power));
}

static void randomize_growth(dw_rom *rom)
{
    int i;
    dw_stats *stats;
    uint8_t str[30];
    uint8_t agi[30];
    uint8_t  mp[30];
    uint8_t  hp[30];

    if (!(rom->flags & FLAG_G))
        return;

    printf("Randomizing stat growth\n");

    for (i=0; i < 30; i++) {
        str[i] = inverted_power_curve(4, 155, 1.18);
        agi[i] = inverted_power_curve(4, 145, 1.32);
        hp[i] =  inverted_power_curve(10, 230, 0.98);
        mp[i] =  inverted_power_curve(0, 220, 0.95);
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

static void randomize_spells(dw_rom *rom)
{
    int i, j;
    dw_stats *stats;

    if (!(rom->flags & FLAG_M))
        return;

    printf("Randomizing spell learning\n");

    /* choose levels for each spell */
    for (i=0; i < 10; i++) {
        rom->new_spells[i].level = mt_rand(1, 16);
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
        } else {
            stats->mp = 0;
        }
    }
}

static void update_drops(dw_rom *rom)
{
    int i;

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

static void update_mp_reqs(dw_rom *rom)
{
    int i;
    const uint8_t mp_reqs[10] = {3, 2, 2, 2, 2, 6, 8, 2, 8, 5};

    for (i=0; i < 10; i++) {
        rom->mp_reqs[i] = mp_reqs[i];
    }
}

static void lower_xp_reqs(dw_rom *rom)
{
    int i;

    if (rom->flags & FLAG_f) {
        printf("Changing required experience to 75%% of normal\n");
        for (i=0; i < 30; i++) {
            rom->xp_reqs[i] = rom->xp_reqs[i] * 3 / 4;
        }
    } else if (rom->flags & FLAG_F) {
        printf("Changing required experience to 50%% of normal\n");
        for (i=0; i < 30; i++) {
            rom->xp_reqs[i] = rom->xp_reqs[i] / 2;
        }
    }
}

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

/*
 * Patches the ROM and returns the next address after the patch.
 */
static void update_title_screen(dw_rom *rom)
{
    uint8_t blank_line[5] = { 0xf7, 0x20, 0x5f, 0xfc, 0xf0 };

}

static uint16_t vpatch(dw_rom *rom, uint16_t address, uint32_t size, ...)
{
    int i;
    va_list arg;
    uint8_t *p;

    p = &rom->data[address];
    va_start(arg, size);

    for (i=0; i < size; i++) {
        *(p++) = va_arg(arg, int);
    }
    va_end(arg);
    return p - rom->data;

}

static uint16_t patch(dw_rom *rom, uint16_t address, uint32_t size, uint8_t *data)
{
    int i;
    uint8_t *p;

    p = &rom->data[address];

    for (i=0; i < size; i++) {
        *(p++) = data[i];
    }
    return p - rom->data;
}

static void dwr_fighters_ring(dw_rom *rom)
{

    printf("Fixing the fighter's ring\n");
    /* fighter's ring fix */
    vpatch(rom, 0xf10c, 4, 0x20, 0x7d, 0xff, 0xea);
    vpatch(rom, 0xff8d, 17,
        /* ff7d: */
        0x85, 0xcd,  /* STA $00CD  ; replaces code removed from $F00C */
        0xa5, 0xcf,  /* LDA $00CF  ; load status bits */
        0x29, 0x20,  /* AND #$20   ; check bit 6 (fighter's ring) */
        0xf0, 0x06,  /* BEQ $FF8B */
        0xa5, 0xcc,  /* LDA $00CC  ; load attack power */
        0x69, 2,    /* ADC #$??   ; add fighter's ring power. */
        0x85, 0xcc,  /* STA $00CC  ; store new attack power */
        /* ff8b: */
        0x4c, 0x54, 0xff  /* JMP $FF54  ; jump to next section */
    );
    vpatch(rom, 0xff64, 3,
        /* ff54: */
        0xa5, 0xcf,  /* LDA $00CF   ; replaces code removed from $F00E */
        0x60   /* RTS */
    );
}

static void dwr_death_necklace(dw_rom *rom)
{

    if (!(rom->flags & FLAG_D))
        return;

    printf("Adding functionality to the death necklace\n");

    vpatch(rom, 0xff64, 32,
            /* ff54: */
            0xa5, 0xcf,  /* LDA $00CF ; load status bits                   */
            0x29, 0x80,  /* AND #$80  ; check bit 8 (death necklace)       */
            0xf0, 0x17,  /* BEQ $FF71                                      */
            0xa5, 0xca,  /* LDA $00CA ; load max HP                        */
            0x46, 0xca,  /* LSR $00CA ; divide max HP by 4                 */
            0x46, 0xca,  /* LSR $00CA                                      */
            /* ff60: */
            0xe5, 0xca,  /* SBC $00CA ; subtract the result                */
            0x85, 0xca,  /* STA $00CA ; rewrite max HP                     */
            0xc5, 0xc5,  /* CMP $00C5 ; compare to current HP              */
            0xb0, 0x02,  /* BCS $FF6A                                      */
            0x85, 0xc5,  /* STA $00C5 ; set current HP to max HP           */
            /* ff6a: */
            0x18,        /* CLC       ; Clear the carry bit                */
            0xa5, 0xcc,  /* LDA $00CC ; load attack power                  */
            0x69, 0x0a,  /* ADD #$0A  ; add 10                             */
            0x85, 0xcc,  /* STA $00CC ; rewrite attack power               */
            /* ff71: */
            0xa5, 0xcf,  /* LDA $00CF   ; replaces code removed from $F00E */
            0x60         /* RTS                                            */
        );
}

static void other_patches(dw_rom *rom)
{
    vpatch(rom, 0xde33, 10,  /* Changes the enemies summoned by the harp. */
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
    vpatch(rom, 0xf45f, 3, 5, 1, 8); 
    vpatch(rom, 0xf4f8, 3, 4, 1, 7); 
    vpatch(rom, 0x1298, 1, 0x22);  /* remove the top set of stairs for the old warp in the grave */
    /* Sets the encounter rate of Zone 0 to be the same as other zones. */
    vpatch(rom, 0xcecf, 3, 0x4c, 0x04, 0xcf);  /* skip over the zone 0 code */
    vpatch(rom, 0xe270, 1, 0);  /* set death necklace chance to 100% */
    vpatch(rom, 0xe75d, 1, 9);  /* buff the hurt spell */
    vpatch(rom, 0xdbd1, 1, 18);  /* buff the heal spell */
    vpatch(rom, 0xea51, 5, 0xad, 0x07, 0x01, 0xea, 0xea); /* I forget what this does */
    /* fixing some annoying roaming npcs */
    vpatch(rom, 0x18fe, 1, 0xa7); /* move the stupid old man from the item shop */
    vpatch(rom, 0x91f,  1, 0x6f); /* quit ignoring the customers */
    vpatch(rom, 0x94c,  1, 0x6f); /* quit ignoring the customers */
    vpatch(rom, 0x17b2,  3, 0, 0, 0); /* roaming throne room guard */
}

static void dwr_menu_wrap(dw_rom *rom)
{
    if (!(rom->flags & FLAG_R))
        return;

    printf("Enabling menu cursor wrap-around\n");

    /* implement up/down wraparound for most menus (from @gameboy9) */
    vpatch(rom, 0x69f0, 3,
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
        0xf0, 0x1e,        /* BEQ $7ED7 ; if they are equal, branch to $7ED7 (return) */
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
        0xf0, 0xfa,        /* BEQ $7EF8 ; if it's 0, branch to $7EF8 (return) */
        0x4c, 0xe4, 0xa9   /* JMP $A9E4 ; jump to $A9E4 */
    );
    vpatch(rom, 0x7f12, 8,
        /* 7f02: */
        0x68,              /* PLA       ; pull A from stack */
        0xc5, 0xd9,        /* CMP $00D9 ; compare to map number */
        0xf0, 0xf3,        /* BEQ $7EF8 ; if they are equal, jump to $7EF8 (return) */
        0x4c, 0x27, 0xaa   /* JMP $A9E4 ; jump to $A9E4 */
    );
}

static void dwr_speed_hacks(dw_rom *rom)
{
    if (!(rom->flags & FLAG_H))
        return;

    printf("Enabling speed hacks\n");

    /* Following are some speed hacks from @gameboy9 */
    /* speed up the text */
    vpatch(rom, 0x7a43, 3, 0xea, 0xea, 0xea);
    /* speed up encounter intros */
    vpatch(rom, 0xe41a, 3, 0xea, 0xea, 0xea);
    vpatch(rom, 0xe44d, 3, 0xea, 0xea, 0xea);
    vpatch(rom, 0xc53f, 3, 0xea, 0xea, 0xea);
    vpatch(rom, 0xef49, 3, 2);  /* speed up the player attack animation */
    vpatch(rom, 0xed45, 3, 3);  /* speed up the enemy attack animation */
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
    vpatch(rom, 0xdb49, 6, 0xea, 0xea, 0xea, 0xea, 0xea, 0xea);
    vpatch(rom, 0xdb54, 9, 0xea, 0xea, 0xea, 0xea, 0xea, 0xea, 0xea, 0xea, 0xea);
}

static void dwr_token_dialogue(dw_rom *rom)
{
    dw_searchable *searchable;
    uint8_t text1[24], text2[72];
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
        dx = rom->map.warps_to[TANTEGEL].x - searchable->x;
        dy = rom->map.warps_to[TANTEGEL].y - searchable->y;
//        strcpy((char*)text1, "Thou may go and search.");
        snprintf((char*)text2, 72, "From Tantegel Castle travel %2d leagues "
                "to the %s and %2d to the %s",
                ABS(dy), (dy < 0) ? "north" : "south", 
                ABS(dx), (dx < 0) ? "west" : "east");
    }
    ascii2dw(text2);
    patch(rom, 0xa252, 70, text2);
}

static void dwr_write(dw_rom *rom, const char *output_file)
{
    FILE *output;

    output = fopen(output_file, "wb");
    if (!output) {
        fprintf(stderr, "Unable to open file '%s' for writing", output_file);
        exit(1);
    }
    fwrite(rom->data, 1, ROM_SIZE, output);
    fclose(output);
}

void dwr_randomize(const char* input_file, uint64_t seed, char *flags, 
        const char* output_dir)
{
    uint64_t crc;
    char output_file[1024];
    snprintf(output_file, 1024, "%s/DWRando.%"PRIu64".%s.nes", output_dir, seed,
            flags);

    mt_init(seed);
    dw_rom rom;
    dwr_init(&rom, input_file, flags);

    while(!map_generate_terrain(&rom)) {}
    shuffle_searchables(&rom);
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
    other_patches(&rom);
    crc = crc64(0, rom.data, ROM_SIZE);
    printf("Checksum: %016"PRIX64"\n", crc);

    update_title_screen(&rom);
    randomize_music(&rom);
    disable_music(&rom);
    crc = crc64(0, rom.data, ROM_SIZE);
//    printf("Final Checksum: %016"PRIX64"\n", crc);

    dwr_write(&rom, output_file);
    free(rom.data);

}
