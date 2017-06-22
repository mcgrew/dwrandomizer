
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/sha.h>

#include "dwr.h"
#include "mt64.h"
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
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ__'______.,-_?!_)(_______________  ";

#if 0
const uint16_t token_dialogue_slice[] = {0xa238, 0xa299};
const uint16_t will_not_work_slice[] = {0xad95, 0xadad};
const uint16_t enemy_stats_slice[] = {0x5e5b, 0x60db};
const uint16_t warps_slice[] = {0xf3d8, 0xf50b};
const uint16_t zones_slice[] = {0xf55f, 0xf5c3};
const uint16_t zone_layout_slice[] = {0xf532, 0xf552};
const uint16_t mp_req_slice[] = {0x1d63, 0x1d6d}; /* mp requirements for spells */
const uint16_t xp_req_slice[] = {0xf36b, 0xf3a7};
const uint16_t player_stats_slice[] = {0x60dd, 0x6191};
const uint16_t weapon_shop_inv_slice[] = {0x19a1, 0x19cc};
const uint16_t new_spell_slice[] = {0xeaf9, 0xeb1f, 4};
const uint16_t chests_slice[] = {0x5ddd, 0x5e59};
const uint16_t title_text_slice[] = {0x3f36, 0x3fc5};

const uint16_t token_slice[] = {0xe11e, 0xe12b, 6};
const uint16_t flute_slice[] = {0xe15d, 0xe16a, 6};
const uint16_t armor_slice[] = {0xe173, 0xe180, 6};

const uint16_t encounter_1_slice[] = {0xcd64, 0xcd71, 6}; /* axe knight  */
const uint16_t encounter_2_slice[] = {0xcd7b, 0xcd88, 6}; /* green dragon */
const uint16_t encounter_3_slice[] = {0xcd98, 0xcda5, 6}; /* golem        */
const uint16_t encounter_1_run_slice[] = {0xe8e7, 0xe8f4, 6}; /* axe knight */
const uint16_t encounter_2_run_slice[] = {0xe90e, 0xe91b, 6}; /* green dragon */
const uint16_t encounter_3_run_slice[] = {0xe93b, 0xe948, 6}; /* golem */
const uint16_t encounter_enemies_slice[] = {0xcd74, 0xcdaf, 29};
const uint16_t encounter_2_kill_slice[] = {0xe97e, 0xe985, 6}; /* green dragon */
const uint16_t encounter_3_kill_slice[] = {0xe990, 0xe997, 6}; /* golem */
#endif

const char flags[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                     "abcdefghijklmnopqrstuvwxyz";

/* for qsort() */
static int compare(const void *a, const void *b)
{
    return *(uint8_t*)a - *(uint8_t*)b;
}

static void parse_flags(dw_rom *rom, const char *flags)
{
}

bool dwr_init(dw_rom *rom, const char *input_file, const char *flags)
{
  FILE *input;

    memset(rom->data, 0, ROM_SIZE);
    input = fopen(input_file, "rb");
    if (!input) {
        printf("Unable to open %s", input_file);
        exit(1);
    }
    fread(rom->data, 1, ROM_SIZE, input);
    fclose(input);
    /* subtract 0x9d5d from these pointers */
    rom->map.pointers = (uint16_t*)&rom->data[0x2663];
    rom->map.encoded = &rom->data[0x1d6d];
    rom->map.meta = (dw_map_meta*)&rom->data[0x2f];
    rom->map.warps_from = (dw_warp*)&rom->data[0xf3d8];
    rom->map.warps_to   = (dw_warp*)&rom->data[0xf471];
    rom->stats = (dw_stats*)&rom->data[0x60dd];
    rom->mp_reqs = &rom->data[0x1d63];
    rom->xp_reqs = (uint16_t*)&rom->data[0xf36b];
    rom->enemies = (dw_enemy*)&rom->data[0x5e5b];
    rom->zones = &rom->data[0xf55f];
    rom->zone_layout = &rom->data[0xf532];
    rom->chests = (dw_chest*)&rom->data[0x5ddd];
    rom->love_calc = (dw_love_calc*)&rom->data[0xdb11];
    rom->map.return_point = (dw_return_point*)&rom->data[0xdf4b];
    rom->map.rainbow_drop = (dw_rainbow_drop*)&rom->data[0xde9b];
    rom->map.rainbow_bridge = (dw_rainbow_drop*)&rom->data[0x2c4b];
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

    map_decode(rom);
}

static inline void sha1(uint8_t *buffer, size_t bufsize, char *hash)
{
    SHA1(buffer, bufsize, hash);
}

void ascii2dw(char *string, uint8_t *bytes)
{
    int i, j, len, alphalen;

    len = strlen(string);
    alphalen = strlen(alphabet);
    for (i=0; i < len; i++) {
        bytes[i] = (uint8_t)0x59;
        for (j=0; j < alphalen; j++) {
            if (string[i] = alphabet[j]) {
                bytes[i] = (uint8_t)j;
            }
        }
    }
}

void dw2ascii(char *string, uint8_t *bytes, size_t bufsize)
{
    int i, alphalen;
    for (i=0; i < bufsize; i++) {
        if (bytes[i] < alphalen) {
            string[i] = alphabet[bytes[i]];
        } else {
            string[i] = ' ';
        }
    }
}

void shuffle_chests(dw_rom *rom) {
    uint8_t *chests;
    dw_chest *c;
    long addr;
    size_t size;
    uint8_t *staff; /* the staff of rain chest */
    uint8_t *key; /* the magic key in the throne room */
    const int cci[] = {11, 12, 13, 14, 15, 16, 24}; /* charlock chest indices */
    const int qi[] = {0xa, 0xd, 0xf, 0x10}; /* quest items */

    printf("Shuffling chest contents...\n");
    
//    chests = malloc(size);
//    memcpy(&rom->data[addr], size);

    for (c = rom->chests; c < rom->chests + 31; c++ ) {
        switch((dw_chest_content)c->item) {
            case GOLD_5: /* change all gold to large gold */
            case GOLD_6:
            case GOLD_10:
                c->item = GOLD_500;
                break;
            case GOLD_120:
                c->item = MAGIC_KEY;
                break;
            case TABLET: /* remove the tablet */
                if (mt_rand(0, 1)) { /* 50/50 chance */
                    rom->token->map = NO_MAP; /* remove token */
                    c->item = TOKEN; /* put it in a chest */
                } else {
                    c->item = MAGIC_KEY; /* replace with a key */
                }
                break;
            default:
                break;
        }
    }
    /**** Still more to do here *****/

}

void randomize_attack_patterns(dw_rom *rom)
{

}

void randomize_towns(dw_rom *rom)
{
}

void randomize_music(dw_rom *rom)
{
    int i;
    uint8_t *new_music;
    uint8_t choices[] = {0x1, 0x1, 0x1, 0x2, 0x2, 0x2, 0x3, 0x3, 0x3, 0x4, 0x4, 
                         0x4, 0x5, 0x5, 0x5, 0xf, 0xf, 0xf, 0x10, 0x10, 0x10,
                         0x6, 0x7, 0x8, 0x9, 0xa, 0xb, 0xc, 0xd};

    new_music = malloc(29);

    for (i=0; i < 29; i++) {
        new_music[i] = choices[mt_rand(0, sizeof(choices))];
    }
}

void disable_music(dw_rom *rom)
{
    uint8_t *new_music;
    new_music = calloc(29, 1);
}

static inline void set_ow_zone(dw_rom *rom)
{
}

void randomize_zone_layout(dw_rom *rom)
{
}

void randomize_zones(dw_rom *rom)
{
}

void randomize_shops(dw_rom *rom)
{
}

void randomize_searchables(dw_rom *rom)
{
}

void randomize_growth(dw_rom *rom)
{
}

void randomize_spells(dw_rom *rom)
{
}

static void update_spell_masks(dw_rom *rom)
{
}

void update_drops(dw_rom *rom)
{
}

void update_mp_reqs(dw_rom *rom)
{
}

void lower_xp_reqs(dw_rom *rom)
{
}

void update_enemy_hp(dw_rom *rom)
{
}

void update_title_screen(dw_rom *rom)
{
}

void dwr_death_necklace(dw_rom *rom)
{
}

void dwr_menu_wrap(dw_rom *rom)
{
}

void dwr_speed_hacks(dw_rom *rom)
{
}

void dwr_token_dialogue(dw_rom *rom)
{
}

void dwr_write(dw_rom *rom, const char *output_file)
{
    FILE *output;

    output = fopen(output_file, "wb");
    fwrite(rom->data, 1, ROM_SIZE, output);
    fclose(output);
}

void dwr_inverted_power_curve(int min, int max, double power, uint8_t *buffer)
{
}

void dwr_randomize(const char* input_file, uint64_t seed, const char *flags, 
        const char* output_dir)
{
    mt_init(seed);
//    mt_init(3);
    dw_rom rom;
    dwr_init(&rom, input_file, flags);
    while(!map_generate_terrain(&rom)) {}
    shuffle_chests(&rom);
    randomize_zone_layout(&rom);
    randomize_zones(&rom);
    randomize_shops(&rom);
    randomize_searchables(&rom);
    randomize_growth(&rom);
    randomize_spells(&rom);
    update_spell_masks(&rom);
    update_drops(&rom);
    update_mp_reqs(&rom);
    lower_xp_reqs(&rom);
    update_enemy_hp(&rom);
    update_title_screen(&rom);
    dwr_death_necklace(&rom);
    dwr_menu_wrap(&rom);
    dwr_speed_hacks(&rom);
    dwr_token_dialogue(&rom);
    dwr_write(&rom, "/tmp/DWRando_v2.nes");

}
