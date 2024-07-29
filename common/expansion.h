/** This file is generated by build_expansion.py */
#ifndef _EXPANSION_H_
#define _EXPANSION_H_

#include "dwr_types.h"

enum hooktype {
    DIALOGUE,
    JSR = 0x20,
    JMP = 0x4c,
};

enum subroutine {
  SAVE_PREVIOUS_COORDS = 0xBFCA,
  START_DWR_CREDITS = 0xC288,
  INC_ATTACK_CTR = 0xC2BB,
  INC_CRIT_CTR = 0xC2C2,
  INC_MISS_CTR = 0xC2C9,
  INC_DODGE_CTR = 0xC2D0,
  INC_BONK_CTR = 0xC2D7,
  BLOCKED_IN_FRONT = 0xC2DE,
  PLAYER_AMBUSHED = 0xC2E6,
  INC_COUNTER = 0xC2EC,
  INC_ENEMY_DEATH_CTR = 0xC2FB,
  INC_DEATH_CTR = 0xC309,
  COUNT_WIN = 0xC314,
  COUNT_ENCOUNTER = 0xC31C,
  INC_MON_COUNTER = 0xC322,
  COUNT_SPELL_USE = 0xC331,
  SORT_INVENTORY = 0xC33D,
  TORCH_IN_BATTLE = 0xC37A,
  THREES_COMPANY_CHECK = 0xC3C2,
  STORE_PRINCESS_ITEM = 0xC3D4,
  CURSED_PRINCESS_CHECK = 0xC3D9,
  SCARED_SLIMES = 0xC3EE,
  INIT_SAVE_RAM = 0xC3FB,
  DISPLAY_DEATHS = 0xC405,
  COUNT_FRAME = 0xC426,
  SNAPSHOT_TIMER = 0xC44D,
  MODIFY_RUN_RATE = 0xC45B,
  SCALE_MSLIME_XP = 0xC46B,
  SAVE_CURRENT_SPELLS = 0xC483,
  SPIKE_MAP = 0xCD7A,
  SPIKE_TABLE = 0xCD7A,
  SPIKE_X = 0xCD82,
  SPIKE_Y = 0xCD8A,
  SPIKE_FLAGS = 0xCD92,
  SPIKE_MONSTER = 0xCD9A,
  SCALED_MSLIME_HOOK_POINT = 0xEA14,
  CREDIT_MUSIC_ATTRIBUTION = 0x6C11,
};

void add_hook(dw_rom *rom, enum hooktype type, uint16_t address,
              enum subroutine to_addr);
void patch_save_previous_coords(dw_rom *rom);
void patch_free_3a(dw_rom *rom);
void patch_free_3b(dw_rom *rom);
void patch_free_3c(dw_rom *rom);
void patch_spike_begin(dw_rom *rom);
void patch_spike_run(dw_rom *rom);
void patch_spike_defeat(dw_rom *rom);
void patch_handle_2_byte_xp_gold(dw_rom *rom);
void patch_print_new_spells(dw_rom *rom);
void patch_free_3d(dw_rom *rom);
void fill_expansion(dw_rom *rom);

#endif
