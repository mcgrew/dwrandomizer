
FILLVALUE $ff

CURRENT_TILE  EQU $E0
SELECTED_ITEM EQU $D7
ITEMS         EQU $C1

.db "NES", $1a
.db 8  ; PRG ROM banks
.db 2  ; CHR ROM banks
.db $12
.db $00
.db $00
.db $00
.db $00
.db 0,0,0,0,0

PPUCTRL    EQU $2000
PPUMASK    EQU $2001
PPUSTATUS  EQU $2002
PPUSCROLL  EQU $2005
PPUADDR    EQU $2006
PPUDATA    EQU $2007
DMC_FREQ   EQU $4010
OAMDMA     EQU $4014

.base $8000

; DW labels

X_POS EQU $003A
X_POS_2 EQU $008E
Y_POS EQU $003B
Y_POS_2 EQU $008F
; _UNUSED_D5 EQU $00D5
; _UNUSED_DC EQU $00DC
; _UNUSED_DD EQU $00DD
; 
; b3_hero_used_harp       EQU $DE18
; b3_hero_used_herb       EQU $DCFE
; b3_hero_used_take       EQU $E200
; b3_irq                  EQU $FD3A
; b3_nmi                  EQU $FE67
; b3_reset                EQU $FFD8
; buy_sell_win            EQU $B0FB
; calc_stats              EQU $F050
; cave_zones              EQU $F542
; combat_cmd_win          EQU $B095
; command_win             EQU $B054
; cont_chg_win            EQU $B20B
; credit_data             EQU $948D
; cycle_rng               EQU $C55B
; d3_d1b2                 EQU $D1B2
; dec_radiant_counter     EQU $CA56
; desc_ptr_table          EQU $A830
; detect_vowel            EQU $BA3C
; dialog_win              EQU $B04B
do_dialog_hi            EQU $C7BD
do_dialog_lo            EQU $C7CB
; do_item                 EQU $B757
; do_spell                EQU $B7D8
; 
; end_credit_ptr          EQU $948B
; enemy_names_1           EQU $BC70
; enemy_names_2           EQU $BDA2
; finish_text             EQU $D242
; full_menu_win           EQU $B249
; heal_spell              EQU $DA47
; healmore_spell          EQU $DAE3
; hero_used_heal          EQU $DBB8
; hero_used_healmore      EQU $DBD7
; hurt_spell              EQU $DA51
; input_name_win          EQU $B1E0
; inv_add_item            EQU $E01B
; inv_check               EQU $E055
; inv_del_item            EQU $E04B
; item_names_1_1          EQU $BAB7
; item_names_1_2          EQU $BBB7
; item_names_2_1          EQU $BB8F
; item_names_2_2          EQU $BC4F
; item_prices             EQU $BE10
; item_prices_ptr         EQU $BE0E
; items_win               EQU $B0CC
; jmp_amt                 EQU $B6CD
; jmp_amt_pts             EQU $B6CA
; jmp_copy                EQU $B6B2
; jmp_desc                EQU $B6BE
; jmp_enemy               EQU $B6B8
; jmp_enemy_2             EQU $B6C1
; jmp_item                EQU $B6C4
; jmp_name                EQU $B6B5
; jmp_points              EQU $B6C7
; jmp_spell               EQU $B6BB
; ken_masuta              EQU $FC76
; load_chr_bank           EQU $FE09
; store_and_load_prg_bank EQU $FF91
load_prg_bank           EQU $FF96
; monster_stats           EQU $9E4B
; monster_stats_ptr       EQU $9E49
; msg_speed_win           EQU $B194
; music_notes             EQU $8205
; name_entry_win          EQU $B1F7
; new_quest_win           EQU $B2A8
; outside_spell           EQU $DA9F
; player_stats            EQU $A0CD
; radiant_spell           EQU $DA84
; read_joy_1              EQU $C608
; repel_spell             EQU $DA94
; repel_table             EQU $F4FA
; reset                   EQU $FD86
; return_spell            EQU $DAED
; save_game               EQU $F6F4
; shop_inv_win            EQU $B0DA
; show_window             EQU $C6F0
; sleep_spell             EQU $DA5C
; spell_names             EQU $BE56
; spell_win               EQU $B0BA
; start_combat            EQU $E4DF
; stats_win               EQU $AFB0
; status_win              EQU $AFC7
; 
; tab_brecconary_map      EQU $8716
; tab_cantlin_map         EQU $88D8
; tab_charlock1_map       EQU $80B0
; tab_charlock_b1_map     EQU $8DBA
; tab_charlock_b2_map     EQU $8E82
; tab_charlock_b3_map     EQU $8EB4
; tab_charlock_b4_map     EQU $8EE6
; tab_charlock_b5_map     EQU $8F18
; tab_charlock_b6_map     EQU $8F4A
; tab_charlock_throne_map EQU $8434
; tab_chest_contents      EQU $9DCD
; tab_erdrick_cave_b1_map EQU $92B0
; tab_erdrick_cave_b2_map EQU $92E2
; tab_garinham_map        EQU $8A9A
; tab_garins_grave_b1_map EQU $909A
; tab_garins_grave_b2_map EQU $925C
; tab_garins_grave_b3_map EQU $9162
; tab_garins_grave_b4_map EQU $922A
; tab_hauksness_map       EQU $8178
; tab_item_shop_inv       EQU $99BC
; tab_kol_map             EQU $85F6
; tab_map_data            EQU $801A
; tab_mt_cave_b1_map      EQU $8FD6
; tab_mt_cave_b2_map      EQU $9038
; tab_north_shrine_map    EQU $8D56
; tab_point               EQU $B751
; tab_rimuldar_map        EQU $8B62
; tab_south_shrine_map    EQU $8D88
; tab_stones_shrine_map   EQU $8D24
; tab_swamp_cave_map      EQU $8F7C
; tab_tant_throne_map     EQU $8402
; tab_tantegel_map        EQU $8240
; tab_warp_from           EQU $F3C8
; tab_warp_to             EQU $F461
; tab_weapon_prices       EQU $9947
; tab_weapon_shop_inv     EQU $9991
; tab_world_map           EQU $9D5D
; tab_world_map_pointers  EQU $A653
; take_content            EQU $E39A
; text_block_1            EQU $8028
; text_block_2            EQU $8286
; text_block_3            EQU $8519
; text_block_4            EQU $8713
; text_block_5            EQU $894C
; text_block_6            EQU $8D12
; text_block_7            EQU $906E
; text_block_8            EQU $9442
; text_block_9            EQU $981E
; text_block_10           EQU $9C88
; text_block_11           EQU $9F3F
; text_block_12           EQU $A28A
; text_block_13           EQU $A6DC
; text_block_14           EQU $AA2E
; text_block_15           EQU $AC61
; text_block_16           EQU $AE28
; text_block_17           EQU $AFEE
; text_block_18           EQU $B68B
; text_block_19           EQU $BA65
; title_routine           EQU $BCB0
; update_sound            EQU $8028
; xp_requirements         EQU $F35B
; yes_no_win_1            EQU $B0EB
; zone_enemies            EQU $F54F
; zone_layout             EQU $F522

.org $c000
