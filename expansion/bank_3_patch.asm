
.org $c288

start_dwr_credits:
    lda #3
    jsr $ff96  ; load prg_bank
    jmp exp_start_dwr_credits

inc_attack_ctr:
    sta $0e
    lda #2
    pha
    bne inc_counter

inc_crit_ctr:
    sta $0e
    lda #3
    pha
    bne inc_counter

inc_miss_ctr:
    sta $0e
    lda #6
    pha
    bne inc_counter

inc_dodge_ctr:
    sta $0e
    lda #8
    pha
    bne inc_counter
inc_bonk_ctr:
    sta $0e
    lda #10
    pha
    bne inc_counter

inc_enemy_death_ctr:
    pha
    lda #3
    jsr $ff96  ; load prg_bank
    jsr exp_inc_enemy_death_ctr
    lda $6004
    jsr $ff96  ; load prg_bank
    pla
inc_death_ctr:
    sta $0e
    lda #0
    pha
inc_counter:
    lda $0e
    brk
    hex 04 17
    lda #3
    jsr $ff96  ; load prg_bank
    pla
    jmp exp_inc_counter

count_encounter:
    brk
    hex 04 17
    lda #3
    jsr $ff96  ; load prg_bank
    jsr exp_count_encounter
    jmp exp_load_last_bank
count_win:
    lda #3
    jsr $ff96  ; load prg_bank
    jsr exp_count_win
    jmp exp_load_last_bank

count_spell_use:
    lda #3
    jsr $ff96  ; load prg_bank
    jsr exp_count_spell_use
    jmp $c6f0   ; show window
sort_inventory:
    jsr b3_e01b ; add the new item
    cpx #$04
    bne + 
    rts
+   lda #3
    jsr $ff96  ; load prg_bank
    jmp exp_sort_inventory

; .org $c30a
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

; .org $c475
torch_in_battle:
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
    adc #$06            ; add 6  (should I have cleared the carry bit?)
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

; .org $c4bc
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

; .org $c4ce
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

; .org $c4e8
scared_slimes:
    cmp $0100 ; Replace the code from $efba
    bcs +     ; Return, everything is fine
    lda $e0   ; Load the enemy type
    cmp #$10  ; Is it a metal slime?
    beq +
    clc       ; It's not, enemy doesn't run
+   rts

.org $c4f5
