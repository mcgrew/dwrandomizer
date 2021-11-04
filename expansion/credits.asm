
; .include defs.asm
; .include macros.asm

TIMERS_COUNT EQU 8
TIMERS_ADDR  EQU $d0

; *** VARIABLES ***
.enum $00
    SCROLLX      .dsb 2
    SCROLLY      EQU #0   ; Y scroll is always 0
.ende
.enum TIMERS_ADDR
    TIMERS       .dsb TIMERS_COUNT*2
.ende
; .enum $200
;     SPRITES    .dsb 256
; .ende

; *** PRG ROM ***
.include bank_0.asm
.include empty.asm
.include empty.asm
.include exp_bank_3.asm
.include exp_bank_4.asm
.include exp_bank_5.asm
.include empty.asm
.include bank_7.asm

; *** CHR ROM ***
.incbin chr_rom.chr

