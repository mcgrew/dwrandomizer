
.org $c288

.word exp_start_dwr_credits    ;  0
.word exp_inc_counter          ;  2
.word exp_inc_enemy_death_ctr  ;  4
.word exp_count_win            ;  6
.word exp_count_encounter      ;  8
.word exp_inc_mon_counter      ; 10
.word exp_count_spell_use      ; 12
.word exp_load_last_bank       ; 14
.word exp_sort_inventory       ; 16
; .word exp_torch_in_battle      ; 18

start_dwr_credits:
    ldy #0
    beq exec_expansion_sub

inc_attack_ctr:
    brk
    hex 04 17
    lda #2
    tay
    jsr exec_expansion_sub
    rts

inc_crit_ctr:
    brk
    hex 04 17
    lda #4
    ldy #2
    bne exec_expansion_sub

inc_miss_ctr:
    brk
    hex 04 17
    lda #6
    ldy #2
    bne exec_expansion_sub

inc_dodge_ctr:
    brk
    hex 04 17
    lda #8
    ldy #2
    bne exec_expansion_sub

inc_bonk_ctr:
    brk
    hex 04 17
    lda #10
    ldy #2
    bne exec_expansion_sub

inc_death_ctr:
    brk
    hex 04 17
    lda #0
    ldy #2
    bne exec_expansion_sub

; inc_counter:
;     lda #3
;     jsr load_prg_bank  ; load prg_bank
;     pla
;     jmp exp_inc_counter

blocked_in_front:
    jsr do_dialog_lo  ; but was blocked in front
    hex f6
    ldy #2
    lda #12
    bne exec_expansion_sub

player_ambushed:
    jsr do_dialog_lo  ; attacked before ready
    hex e4
    lda #14
    ldy #2
;     bne exec_expansion_sub

exec_expansion_sub:
    pha
;     lda #$4c
;     sta $7f00
    lda $c288,y
    sta $7f01
    lda $c289,y
    sta $7f02
    lda #3
    jsr load_prg_bank
    pla
    jmp ($7f01)

inc_enemy_death_ctr:
    brk
    hex 04 17
    ldy #4
    bne exec_expansion_sub

count_encounter:
    brk
    hex 04 17
    ldy #8
    jsr exec_expansion_sub
    jmp exp_load_last_bank

count_win:
    ldy #6
    jsr exec_expansion_sub
    jmp exp_load_last_bank

count_spell_use:
    ldy #12
    jsr exec_expansion_sub
    jmp $c6f0   ; show window

sort_inventory:
    jsr b3_e01b ; add the new item
    cpx #$04
    bne + 
    rts
+   ldy #16
    bne exec_expansion_sub
; +   lda #3
;     jsr load_prg_bank  ; load prg_bank
;     jmp exp_sort_inventory

torch_in_battle:
;     ldy #18
;     bne exec_expansion_sub
    cmp #$04            ; is this a torch?
    bne +++             ; no, go to next check
    lda #$01
    jsr $e04b           ; remove the item from inventory
    jsr $c7c5           ; show the "hurled a torch" message
    hex 29
    lda $e0
    cmp #$10            ; are we fighting a metal slime?
    bne ++              ; no, go to higher damage
-   jsr $c55b           ; run the rng cycle routine
    lda $95             ; load a random number
    and #$01            ; limit to 0-1
    bne +               ; did we hit it?
    jmp $e658           ; A miss!
+   jmp $e694           ; yes, go to damage routine
++  jsr $c55b           ; run the rng cycle routine
    lda $95             ; load a random number
    and #$03            ; limit to 0-3
    clc
    adc #$06            ; add 6
    jmp $e694           ; jump to damage routine
+++ cmp #$05            ; c9 04      ; Fairy Water
    bne +
    lda #$02
    jsr $e04b           ; remove the item from inventory
    jsr $c7c5           ; show the "sprinked fairy water" message
    hex 2a
    lda $e0
    cmp #$10            ; metal slime
    beq - 
    jmp $e744
+   jmp $e6fd           ; 4c fd e6

threes_company_check:
    lda $df   ; Load the status bits
    and #$01  ; Are we carrying Gwaelin?
    beq +     ; No, back to original code
    jsr $c7cb ; Show dialogue
    .db $a1   ; I have some great ideas
    jmp $ccb8 ; Jump to the ending credits

+   jsr $c7cb ; Show dialoge
    .db $a1   ; Now take a long rest
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
    lda $662c      ; continue counting if the timer is greater than zero.
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
    


.org $c4f5
