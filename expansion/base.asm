
.enum $e0
    TMP         .dsb 1
    TMP2        .dsb 1
    RANDOM      .dsb 2
    BUTTONS     .dsb 2
    NEW_BTNS    .dsb 2
    RELEASED_BTNS .dsb 2
;     VBLANK      .dsb 1
.ende

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

    ; init rng with whatever is in A
    sta RANDOM+1
;     jsr init_apu
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

    jmp game_start

; wait_for_sprite_0:
; -   bit PPUSTATUS   ; wait for end of blank
;     bvs -
; -   bit PPUSTATUS   ; wait for sprite 0 hit
;     bvc -
;     rts

wait_for_nmi:
    lda #$00
    sta VBLANK
-   lda VBLANK
    beq -
    rts

;  cycle_rng:
;      pha
;      tya
;      pha
;      ldy #8     ; iteration count (8 bits)
;      lda RANDOM
;  
;  -   asl        ; shift the register
;      rol RANDOM+1
;      bcc +
;      eor #$39   ; apply XOR feedback whenever a 1 bit is shifted out
;  
;  +   dey
;      bne -
;      sta RANDOM
;      pla
;      tay
;      pla
;      rts
;  
; read_joy:
;     lda BUTTONS
;     sta TMP
;     jsr get_joy_buttons
; -   lda BUTTONS
;     pha
;     jsr get_joy_buttons
;     pla
;     cmp BUTTONS
;     bne -
;     lda TMP
;     eor BUTTONS
;     and BUTTONS
;     sta NEW_BTNS  ; now contains buttons newly pressed in the last frame
;     lda TMP
;     eor BUTTONS
;     and TMP
;     sta RELEASED_BTNS ; now contains the buttons released in the last frame
;     rts

; get_joy_buttons:
;     ldx #$01
;     stx JOY
;     dex
;     stx JOY
;     inx
; --  lda #$01
;     sta BUTTONS,x
;     lsr a        ; now A is 0
; -   lda JOY,x
;     lsr a	     ; bit 0 -> Carry
;     rol BUTTONS,x  ; Carry -> bit 0; bit 7 -> Carry
;     bcc -
;     dex
;     beq --
;     rts

; init_apu:
;     ; Init $4000-4013
;     ldy #$13
; -   lda @regs,y
;     sta SQ1_VOL,y
;     dey
;     bpl -
; 
;     ; We have to skip over $4014 (OAMDMA)
;     lda #$0f
;     sta SND_CHN
;     lda #$40
;     sta $4017
;     rts

@regs:
    .hex 30 08 00 00
    .hex 30 08 00 00
    .hex 80 00 00 00
    .hex 30 00 00 00
    .hex 00 00 00 00

; .org $fffa

.word nmi
.word reset
.word irq

