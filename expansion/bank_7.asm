
.base $c000

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

game_start:
    jmp start_dwr_credits
.include bank_3_patch.asm

first_vblank:
    jsr load_pal

    lda #$20
    sta PPUADDR
    lda #$00
    sta PPUADDR
    jsr load_bg
    lda #$24
    sta PPUADDR
    lda #$00
    sta PPUADDR
    jsr load_bg

    lda #$00
;     sta SCROLLX
    sta PPUSCROLL
;     lda SCROLLY
    sta PPUSCROLL
    rts

load_pal:
    lda #$3f
    sta PPUADDR
    lda #$00
    sta PPUADDR
    ldx #$00
-   lda bg_palette, x
    sta PPUDATA
    inx
    cpx #$10
    bne -
    rts

load_bg:
    ldy #$04
--  ldx #$f0
-   lda #$5f
++  sta PPUDATA
    dex
    bne -
    dey
    bmi +    ; this will trigger on our final write loop
    bne --
;     ldx #$c0 ; start the final write loop
; -   sta PPUDATA
;     dex
;     bne -
    ldx #0
--  lda the_end,x
    inx
    cmp #$fd
    beq +
    pha
    lda the_end,x
    inx
    sta PPUADDR
    pla
    sta PPUADDR
-   lda the_end,x
    inx
    cmp #$fc
    beq --
    sta PPUDATA
    bne -
+   rts

game_init:

+   rts

bg_palette:
    .hex 0f 0f 30 30
    .hex 0f 0f 24 24
    .hex 0f 0f 27 27
    .hex 0f 0f 2a 2a

the_end:
    ; The follwing is for "THE END" text
    .word $21aa ; PPU address
    .hex 3e 3f 40 41 42 43 44 45 46 47 48 49 fc
    .word $21ca ; PPU address
    .hex 4a 4b 4c 4d 4e 4f 50 51 52 53 54 55 fc
    .hex fd

.include base.asm

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

