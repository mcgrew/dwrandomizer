
.base $c000
.org $c288
patch_free_3a_start:

start_dwr_credits:
.include credit_start.asm

inc_attack_ctr:
    brk
    hex 04 17
    lda #2
    bne inc_counter  ; always branch

inc_crit_ctr:
    brk
    hex 04 17
    lda #4
    bne inc_counter  ; always branch

inc_miss_ctr:
    brk
    hex 04 17
    lda #6
    bne inc_counter  ; always branch

inc_dodge_ctr:
    brk
    hex 04 17
    lda #8
    bne inc_counter  ; always branch

inc_bonk_ctr:
    brk
    hex 04 17
    lda #10
    bne inc_counter  ; always branch

blocked_in_front:
    jsr do_dialog_lo  ; but was blocked in front
    hex f6            ; subroutine argument
    lda #12
    bne inc_counter   ; always branch

player_ambushed:
    jsr do_dialog_lo  ; attacked before ready
    hex e4            ; subroutine argument
    lda #14           ; fall through to next subroutine

inc_counter:
    tax
    ldy $6630,x
    iny
    bne +
    inc $6631,x
+   tya
    sta $6630,x
    rts

inc_enemy_death_ctr:
    brk
    hex 04 17
    lda CURRENT_TILE
    asl
    clc
    adc #160
    jsr inc_mon_counter
    beq inc_counter  ; is this needed?:

inc_death_ctr:
    lda #0
    jsr inc_counter
    lda #$14
    brk
    hex 04 17
    rts

count_win:
    lda CURRENT_TILE
    asl
    clc
    adc #80
    bne inc_mon_counter

count_encounter:
    brk
    hex 04 17
    lda CURRENT_TILE
    asl

inc_mon_counter:
    tax
    ldy $6670,x
    iny
    bne +
    inc $6671,x
+   tya
    sta $6670,x
    rts

count_spell_use:
    lda SELECTED_ITEM
    asl
    clc
    adc #$2c
    jsr inc_counter
    jmp show_window

sort_inventory:
    jsr inv_add_item ; add the new item
    cpx #$04
    beq ++
    ldx #$03
-   lda ITEMS,x
    and #$0f
    sta $a8,x
    lda ITEMS,x
    lsr a
    lsr a
    lsr a
    lsr a
    sta $ac,x
    lda #$00
    sta ITEMS,x
    dex
    bpl -
    lda #$00
--  ldx #$07
-   cmp $a8,x
    bcs +
    lda $a8,x
    stx $a7
+   dex
    bpl -
    cmp #$00
    beq ++
    jsr inv_add_item ; add the next item
    ldx $a7
    lda #$00
    sta $a8,x
    beq --
++  rts


torch_in_battle:
    cmp #$04            ; is this a torch?
    bne +++             ; no, go to next check
    lda #$01
    jsr $e04b           ; remove the item from inventory
    jsr do_dialog_hi    ; show the "hurled a torch" message
    hex 29
    lda $e0
    cmp #$10            ; are we fighting a metal slime?
    bne ++              ; no, go to higher damage
-   jsr cycle_rng       ; run the rng cycle routine
    lda $95             ; load a random number
    and #$01            ; limit to 0-1
    bne +               ; did we hit it?
    jmp $e658           ; A miss!
+   jmp $e694           ; yes, go to damage routine
++  jsr cycle_rng       ; run the rng cycle routine
    lda $95             ; load a random number
    and #$03            ; limit to 0-3
    clc
    adc #$06            ; add 6
    jmp $e694           ; jump to damage routine
+++ cmp #$05            ; c9 04      ; Fairy Water
    bne +
    lda #$02
    jsr inv_del_item    ; remove the item from inventory
    jsr do_dialog_hi    ; show the "sprinked fairy water" message
    hex 2a
    lda $e0
    cmp #$10            ; metal slime
    beq - 
    jmp $e744
+   jmp $e6fd           ; 4c fd e6

threes_company_check:
    lda $df          ; Load the status bits
    and #$01         ; Are we carrying Gwaelin?
    beq +            ; No, back to original code
    jsr $c7cb        ; Show dialogue
    .db $a1          ; I have some great ideas
    jmp $ccb8        ; Jump to the ending credits

+   jsr do_dialog_lo ; Show dialoge
    .db $a1          ; Now take a long rest
    rts

store_princess_item:
    sta $0e    ; Store the item Gwaelin took
    jmp $e04b  ; Continue on
cursed_princess_check:
    pha        ; Store register
    lda $0e    ; Load the item Gwaelin took
    cmp #$09   ; It is a cursed belt?
    bne +      ; No, keep playing
    jsr $c7cb  ; Yes, Display dialogue
    .db $a0    ; What a lovely corset
    jsr $dffa  ; Play the curse music
    jmp $ccb8  ; Jump to the ending
+   pla        ; Restore register and continue
    jmp $d433

scared_slimes:
    cmp $0100 ; Replace the code from $efba
    bcs +     ; Return, everything is fine
    lda $e0   ; Load the enemy type
    cmp #$10  ; Is it a metal slime?
    beq +
    clc       ; It's not, enemy doesn't run
+   rts

init_save_ram:
    sta $62e8,x
    sta $662c,x
    sta $672c,x
    rts

display_deaths:
    lda $64df
    bne +
    jmp $a8ba
+   lda #05
    sta $64e2
    lda $6630
    sta $1a
    lda $6631
    sta $1b
    lda #00
    sta $1c
    jsr $a753
    jmp $a764

count_frame:
    ldx MAP_INDEX  ; Start the timer once the map is no longer zero
    bne +
    lda $662c      ; continue counting if the timer has already started
    ora $662d
    ora $662e
    beq ++
+   ldx $662c      ; increment the timer
    inx
    stx $662c
    bne ++
    ldx $662d
    inx
    stx $662d
    bne ++
    inc $662e
++  jmp $fdf4

snapshot_timer:
    ldx #2
-   lda $662c,x
    sta $6f00,x
    dex
    bpl -
    jmp do_dialog_hi

modify_run_rate:
    and #$7f       ; limit the value in reg A to 0-127
    sta $3e        ; save to $3E
    jsr cycle_rng  ; generate a new random number
    lda RANDOM_H   ; load the random number
    and #$3f       ; limit to 0-63
    adc $3e        ; add to the previous number
    jmp $eec7

scale_mslime_xp:
    lda $e0        ; load the enemy index
    cmp #16        ; is it a metal slime?
    bne +
    lda $c7        ; load the player level
    cmp #8         ; is it less than 8?
    bcs +
    asl            ; multiply the level by 32 and use that for xp
    asl
    asl
    asl
    asl
    sta $00
    lda #0
    sta $01
+   rts

save_current_spells:
  lda $ce        ; load current spells (lo)
  sta $dc        ; store at $dc
  lda $cf        ; load current spells (hi)
  sta $dd        ; store at $dd
  jmp calc_stats ; update to new level stats

.org $c4f5
patch_free_3a_end:

.org $c4f5
    ; potentially unused data
.org $c529

.org $c6c9
patch_free_3b_start:
    ; unused data
.org $c6f0
patch_free_3b_end:

.org $c7ec
patch_free_3c_start:
    ; unused data
.org $c9b5
patch_free_3c_end:

.org $ea0a
patch_handle_2_byte_xp_gold_start:
    lda $106       ; load the low byte of xp from this fight
    sta $00        ; store in tmp0 for display
    lda $0107      ; load the high byte of xp
    sta $01        ; store at tmp1 for display
scaled_mslime_hook_point:
    nop            ; make room for a jsr hook here for scaled slimes
    nop
    nop
+   jsr $c7cb      ; print xp gained
    hex ef         ; message $EF
    lda $00        ; load low byte of xp from this fight
    clc
    adc $ba        ; add low byte of xp to total
    sta $ba
    lda $01        ; load high byte of xp from this fight
    adc $bb        ; add high byte of xp to total
    sta $bb
    bcc +          ; if xp rolled over, set to 65535
    lda #$ff
    sta $ba
    sta $bb
+   lda $0108      ; load low byte of gold from this fight
    sta $00        ; store at tmp0 for display
    lda $0109      ; load hight byte of gold from this fight
    sta $01        ; store at tmp1 for display
    jsr $c7cb      ; print gold gained
    hex f0         ; message $F0
    lda $00        ; load low byte of gold from this fight
    clc
    adc $bc        ; add low byte of gold to total
    sta $bc
    lda $01        ; load high byte of gold from this fight
    adc $bd        ; add high byte of gold to total
    sta $bd
    lda #$ff
    bcc +          ; if gold rolled over, set to 65535
    sta $bc
    sta $bd
+   bne $ea63      ; branch back to original code (always)
patch_handle_2_byte_xp_gold_end:

.org $eae6
patch_print_new_spells_start:

  ldy #$10         ; Start at 16
  lda $dc          ; load old spells known
  eor $ce          ; xor with new spells known
- pha              ; push on the stack
  and #1           ; is the current spell new?
  beq +            ; no, go to next spell
  tya              ; transfer the index to A
  pha              ; push to the stack
  jsr $dbf0        ; prepare spell name
  jsr do_dialog_lo ; print new spell text
  hex f2
  pla              ; pull the Y value
  tay              ; transfer to Y
+ pla              ; pull stored new spells
  lsr              ; shift right
  iny              ; increment spell index
  cpy #$1a         ; if Y is 26, we're done
  beq +
  cpy #$18         ; if Y is 24, continue to next spell byte
  bne -
  lda $dd          ; load second spell byte
  eor $cf          ; xor with new spells
  and #$3          ; we only need 2 bits here
  bne -            ; if it's not empty, continue
+

.org $eb14, $ea

patch_print_new_spells_end:

.org $f232
patch_free_3d_start:
    ; unused data
.org $f35b
patch_free_3d_end:

.org $10000

