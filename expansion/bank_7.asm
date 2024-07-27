
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

reset:
    sei
    cld
    ldx #$40
    stx $4017
    ldx #$ff
    txs            ; set up the stack
    inx
    stx PPUCTRL   ; disable NMI
    stx DMC_FREQ  ; disable DMC IRQs
    lda #$06
    sta PPUMASK   ; disable rendering

    ; Optional (omitted):
    ; Set up mapper and jmp to further init code here.

    ; The vblank flag is in an unknown state after reset,
    ; so it is cleared here to make sure that vblankwait1
    ; does not exit immediately.
    bit PPUSTATUS

    ; First of two waits for vertical blank to make sure that the
    ; PPU has stabilized

; vblankwait1
-   bit PPUSTATUS
    bpl -

    ; We now have about 30,000 cycles to burn before the PPU stabilizes.
    ; One thing we can do with this time is put RAM in a known state.
    ; Here we fill it with $00, which matches what (say) a C compiler
    ; expects for BSS.  Conveniently, X is still 0.
    txa

@clrmem:
    sta $00,x
    sta $100,x
    sta $200,x
    sta $300,x
    sta $400,x
    sta $500,x
    sta $600,x
    sta $700,x
    inx
    bne @clrmem

    ; Other things you can do between vblank waits are set up audio
    ; or set up other mapper registers.

IFDEF game_init ; If this subroutine exists, jump to it here.
    jsr game_init
ENDIF

   ; wait for vblank
-   bit $2002
    bpl -

IFDEF first_vblank
    jsr first_vblank
ENDIF

    lda #%10000000
    sta PPUCTRL     ; enable NMI
    lda #%00011010
    sta PPUMASK    ; enable rendering


.include credit_start.asm


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

.org $ff91

store_and_load_prg_bank:
b3_ff91:
    sta $6004
    nop
    nop
load_prg_bank:
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

