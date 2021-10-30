
.base $c000

game_start:

    ; write some code to ram and jump to it
    ; This will switch bank modes and load the expansion bank
    lda #$a9
    sta $7fd0
    lda #%00011010
    sta $7fd1
    ldx #$12
-   lda b3_fe09+3, x
    sta $7fd2,x
    dex
    bpl -

    lda #$a9
    sta $7fe5
    lda #3
    sta $7fe6
    ldx #$15
-   lda b3_ff96, x
    sta $7fe7,x
    dex
    bpl -
    lda #>(credit_start-1)
    pha
    lda #<(credit_start-1)
    pha
    jmp $7fd0

set_first_bank:
    sta $dfff
    lsr a
    sta $dfff
    lsr a
    sta $dfff
    lsr a
    sta $dfff
    lsr a
    sta $dfff
    nop
    nop
    rts

.org $c288
start_dwr_credits:
    lda #4
    jsr b3_ff96
    jmp exp_start_dwr_credits

inc_attack_ctr:
            sta $0e
            lda #2
            pha
            bne inc_counter

inc_crit_ctr:
            sta $0e
            lda #4
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
            lda #4
            jsr b3_ff96
            jsr exp_inc_enemy_death_ctr
            lda $6004
            jsr b3_ff96
            pla
inc_death_ctr:
            sta $0e
            lda #0
            pha
inc_counter:
            lda $0e
            brk
            hex 04 17
            lda #4
            jsr b3_ff96
            pla
            jmp exp_inc_counter

count_encounter:
            brk
            hex 04 17
            lda #4
            jsr b3_ff96
            jsr exp_count_encounter
            jmp exp_load_last_bank
count_win:
            lda #4
            jsr b3_ff96
            jsr exp_count_win
            jmp exp_load_last_bank

count_spell_use:
            lda #4
            jsr b3_ff96
            jsr exp_count_spell_use
            jmp show_window
sort_inventory:
            jsr b3_e01b ; add the new item
            cpx #$04
            bne + 
            rts
        +   lda #4
            jsr b3_ff96
            jmp exp_sort_inventory

.org $c7cb
show_window: rts ; dummy subroutine

.org $e01b
b3_e01b:    rts  ; dummy subroutine

.org $fe09
b3_fe09:
    sta $6001
    sta $9fff
    lsr a
    sta $9fff
    lsr a
    sta $9fff
    lsr a
    sta $9fff
    lsr a
    sta $9fff
    rts

.org $ff96
b3_ff96:
    sta $ffff
    lsr a
    sta $ffff
    lsr a
    sta $ffff
    lsr a
    sta $ffff
    lsr a
    sta $ffff
    nop
    nop
    rts

.org $ffd8
;     sei
;     inc $ffdf
    lda #0
    jsr set_first_bank
    jmp reset

.org $fffa

.word nmi
.word reset
.word irq

