
.base $8000

exp_start_dwr_credits:
    lda #$a9        ; lda
    sta $200
    lda #%00000010  ; Change banking mode
    sta $201
    ldx #$12
-   lda b3_fe09+3, x
    sta $202,x
    dex
    bpl -

    lda #$a9
    sta $215
    lda #5         ; load bank 5
    sta $216
    ldx #$15
-   lda b3_ff96, x
    sta $217,x
    dex
    bpl -
    lda #>(credit_start-1)
    pha
    lda #<(credit_start-1)
    pha
    jmp $200

exp_inc_enemy_death_ctr:
    lda CURRENT_TILE
    asl
    clc
    adc #160
    jsr exp_inc_mon_counter
    lda #0
    beq exp_inc_counter

exp_count_win:
    lda CURRENT_TILE
    asl
    clc
    adc #80
    bne exp_inc_mon_counter
exp_count_encounter:
    lda CURRENT_TILE
    asl
exp_inc_mon_counter:
    tax
    ldy $6670,x
    iny
    bne +
    inc $6671,x
+   tya
    sta $6670,x
    rts

exp_count_spell_use:
    lda SELECTED_ITEM
    asl
    clc
    adc #$2c
exp_inc_counter:
    tax
    ldy $6630,x
    iny
    bne +
    inc $6631,x
+   tya
    sta $6630,x
exp_load_last_bank:
    lda $6004
    jmp b3_ff96

exp_sort_inventory:
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
    beq +
    jsr b3_e01b ; add the next item
    ldx $a7
    lda #$00
    sta $a8,x
    beq --
+   jmp exp_load_last_bank

; exp_torch_in_battle:
;     cmp #$04            ; is this a torch?
;     bne +++             ; no, go to next check
;     lda #$01
;     jsr $e04b           ; remove the item from inventory
;     jsr $c7c5           ; show the "hurled a torch" message
;     hex 29
;     lda $e0
;     cmp #$10            ; are we fighting a metal slime?
;     bne ++              ; no, go to higher damage
; -   jsr $c55b           ; run the rng cycle routine
;     lda $95             ; load a random number
;     and #$01            ; limit to 0-1
;     bne +               ; did we hit it?
;     jmp $e658           ; A miss!
; +   jmp $e694           ; yes, go to damage routine
; ++  jsr $c55b           ; run the rng cycle routine
;     lda $95             ; load a random number
;     and #$03            ; limit to 0-3
;     clc
;     adc #$06            ; add 6
;     jmp $e694           ; jump to damage routine
; +++ cmp #$05            ; c9 04      ; Fairy Water
;     bne +
;     lda #$02
;     jsr $e04b           ; remove the item from inventory
;     jsr $c7c5           ; show the "sprinked fairy water" message
;     hex 2a
;     lda $e0
;     cmp #$10            ; metal slime
;     beq - 
;     jmp $e744
; +   jmp $e6fd           ; 4c fd e6

.org $c000
