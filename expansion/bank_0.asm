
FILLVALUE $ff

CURRENT_TILE  EQU #$E0
SELECTED_ITEM EQU #$D7
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


.org $c000
