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
  BLOCKED_IN_FRONT = 0xC2D5,
  COUNT_ENCOUNTER = 0xC30A,
  COUNT_FRAME = 0xC3DD,
  COUNT_SPELL_USE = 0xC31D,
  COUNT_WIN = 0xC315,
  CURSED_PRINCESS_CHECK = 0xC390,
  DISPLAY_DEATHS = 0xC3BC,
  EXEC_EXPANSION_SUB = 0xC2E7,
  INC_ATTACK_CTR = 0xC29E,
  INC_BONK_CTR = 0xC2C3,
  INC_CRIT_CTR = 0xC2A8,
  INC_DEATH_CTR = 0xC2CC,
  INC_DODGE_CTR = 0xC2BA,
  INC_ENEMY_DEATH_CTR = 0xC303,
  INC_MISS_CTR = 0xC2B1,
  INIT_SAVE_RAM = 0xC3B2,
  MODIFY_RUN_RATE = 0xC412,
  PLAYER_AMBUSHED = 0xC2DF,
  SCARED_SLIMES = 0xC3A5,
  SNAPSHOT_TIMER = 0xC404,
  SORT_INVENTORY = 0xC325,
  START_DWR_CREDITS = 0xC29A,
  STORE_PRINCESS_ITEM = 0xC38B,
  THREES_COMPANY_CHECK = 0xC379,
  TORCH_IN_BATTLE = 0xC331,
};

void add_hook(dw_rom *rom, enum hooktype type, uint16_t address,
        enum subroutine to_addr);
void bank_3_patch(dw_rom *rom);
void fill_expansion(dw_rom *rom);

#endif
