.base $8000

.enum $00
    PPU_POSITION   .dsb 2
    INTPTR_ADDR     .dsb 2
    TMP_ADDR       .dsb 2
    BCD_RESULT     .dsb 5
    BCD_INPUT      .dsb 2
    NEXT_TILE      .dsb 1
    FRAMECOUNT     .dsb 1
    VBLANK         .dsb 1
    NMI_FLAGS      .dsb 1
    RAW_CHAR_FLAG  .dsb 1
    PPUATTR_LEN    .dsb 1

    PRODUCT
    REMAINDER      .dsb 3
    MULTIPLIER
    DIVISOR        .dsb 3
    MULTIPLICAND
    DIVIDEND       .dsb 3
    PZTEMP         .dsb 1
    FRAMES         .dsb 1
    SECONDS        .dsb 1
    MINUTES        .dsb 1
    HOURS          .dsb 1
.ende

NMI_INIT_PPU       EQU %00010000
NMI_SKIP_TILE      EQU #$f7

;  INSTR_LOAD_CHR     EQU $f8
INSTR_PLAY_TIME    EQU $f7
INSTR_MON_STAT     EQU $f8
INSTR_WRITE_ZP     EQU $f9
INSTR_SET_PPUATTR  EQU $fa
INSTR_REPEAT_CHAR  EQU $fb
INSTR_SHOW_NUMBER  EQU $fc
INSTR_SET_CURSOR   EQU $fd
INSTR_WAIT         EQU $fe
INSTR_FINISH       EQU $ff

music_data:
;  .include music.asm
IFNDEF DWR_BUILD
    .db 0 ; so famistudio won't choke if there's no music
ENDIF


.org $a800


; game_start:
credit_start:

    ; clear Zero page and RAM famistudio uses
    lda #0
    ldx #$f
-   sta FAMISTUDIO_ASM6_ZP_ENUM,x
    dex
    bpl -
    tax
-   sta FAMISTUDIO_ASM6_BSS_ENUM,x
    sta $00,x
    dex
    bne -

    ; set up the reset subroutine
    lda #%00011110
    sta $201
    lda #0
    sta $216
    lda #$df
    sta $218
    sta $21c
    sta $220
    sta $224
    sta $228
    lda #$4c
    sta $22c
    lda #$d8
    sta $22d
    lda #$ff
    sta $22e

    lda #0
    sta FRAMECOUNT
    ; wait for 240 frames
    ldx #30
-   jsr exp_wait_for_nmi
    dex
    bne -
    
IFDEF test_frames
    ldx #2
    lda test_frames,x
    sta $6f00,x
ENDIF

    jsr calculate_play_time
;     lda #1
;     ldx #<sounds
;     ldy #>sounds
;     jsr famistudio_sfx_init
start_music:
    lda #1
    ldx #<music_data
    ldy #>music_data
    jsr famistudio_init
    lda #0
    jsr famistudio_music_play

;     ; set up the PPU attributes
;     lda #NMI_INIT_PPU
;     sta NMI_FLAGS
;     jsr exp_wait_for_nmi

    lda #<credits_data
    sta INTPTR_ADDR
    lda #>credits_data
    sta INTPTR_ADDR+1

-   ldx #0
    lda (INTPTR_ADDR,x)
    pha
    jsr inc_intptr_addr
    pla
    cmp #$f0
    bcc + 
    jsr handle_instruction
    jmp -
+   jsr convert_character
    sta NEXT_TILE
    jsr exp_wait_for_nmi
    jsr inc_ppu_position
    jmp - 

table_char_conv:
    .db $5f, $4c, $5f, $5f, $5f, $5f, $5f, $53
    .db $4f, $4e, $41, $5f, $48, $49, $47, $52
    .db $00, $01, $02, $03, $04, $05, $06, $07
    .db $08, $09, $44, $4d, $5f, $45, $42, $4b
    .db $5f, $24, $25, $26, $27, $28, $29, $2a
    .db $2b, $2c, $2d, $2e, $2f, $30, $31, $32
    .db $33, $34, $35, $36, $37, $38, $39, $3a
    .db $3b, $3c, $3d, $3e, $5f, $3f, $5f, $5f
    .db $51, $0a, $0b, $0c, $0d, $0e, $0f, $10
    .db $11, $12, $13, $14, $15, $16, $17, $18
    .db $19, $1a, $1b, $1c, $1d, $1e, $1f, $20
    .db $21, $22, $23, $5f, $5f, $5f, $46, $5f

convert_character:
    pha
    cmp #32
    bcc +
    cmp #129
    bcs +
    lda RAW_CHAR_FLAG
    bne +
    stx PZTEMP
    pla
    tax
    lda table_char_conv-32, x
    ldx PZTEMP
    rts
+   pla
    rts

handle_instruction:
    cmp #INSTR_REPEAT_CHAR
    bne +next_instr
    ldy #0
    lda (INTPTR_ADDR),y
    tax
    iny
-   lda (INTPTR_ADDR),y
    jsr convert_character
    sta NEXT_TILE
    jsr exp_wait_for_nmi
    jsr inc_ppu_position
    dex
    bpl -
    iny
    tya
    jsr add_intptr_addr
    rts

;  +next_instr:
;      cmp #INSTR_LOAD_CHR
;      bne +next_instr
;      ldy #0
;      lda (INTPTR_ADDR),y
;      jsr load_chr_bank_0
;      jsr inc_intptr_addr
;      rts

+next_instr:
    cmp #INSTR_WRITE_ZP
    bne +next_instr
    ldy #0
    lda (INTPTR_ADDR),y
    tax
    iny
    lda (INTPTR_ADDR),y
    sta $00,x
    iny
    tya
    jsr add_intptr_addr
    rts

+next_instr:
    cmp #INSTR_SET_PPUATTR
    bne +next_instr
    ldy #0
    lda (INTPTR_ADDR),y
    pha
    and #$7
    asl
    asl
    asl
    sta PZTEMP
    pla
    and #$70
    lsr
    lsr
    lsr
    lsr
    ora PZTEMP
    ora #$c0
    sta PPU_POSITION
    lda #$23
    sta PPU_POSITION+1
    iny
    lda (INTPTR_ADDR),y
    sta PPUATTR_LEN
    pha
    iny
    lda (INTPTR_ADDR),y
    sta NEXT_TILE
    iny
    tya
    jsr add_intptr_addr
    jsr exp_wait_for_nmi
    pla
    jsr add_ppu_position
    rts

+next_instr:
    cmp #INSTR_WAIT
    bne +next_instr
    ldx #0
    lda (INTPTR_ADDR,x)      ; load the number of frames to wait into X
    tax
    jsr inc_intptr_addr      ; increment the stats script pointer
-   jsr exp_wait_for_nmi    ; wait one frame
    dex                     ; decrement X
    bne -                   ; If X is not zero, wait longer
    rts

+next_instr:
    cmp #INSTR_MON_STAT
    bne +next_instr
    lda #$5f                ; queue up a blank space
    sta NEXT_TILE           ; print the blank space
    jsr exp_wait_for_nmi
    lda #$5f                ; queue up a blank space
    sta NEXT_TILE           ; print the blank space
    jsr exp_wait_for_nmi
    ldx #0
    lda (INTPTR_ADDR,x)      ; load the monster index
    asl                     ; double it (2 bytes per monster)
    clc
    adc #$70                ; Add @$6670 to this number and store in the
    sta TMP_ADDR            ; temporary address variable
    lda #$66
    adc #00
    sta TMP_ADDR+1
    jsr inc_intptr_addr      ; increment the stats script pointer

    jsr addr_to_bcd         ; convert the value at TMP_ADDR to a BCD
    jsr print_bcd           ; print the result to the screen
    lda #$5f                ; queue up a blank space
    sta NEXT_TILE           ; print the blank space
    jsr exp_wait_for_nmi
    jsr inc_ppu_position    ; move the cursor
    lda TMP_ADDR
    clc
    adc #80                 ; add 80 to the temporary address
    sta TMP_ADDR
    bcc +
    inc TMP_ADDR+1
+
    jsr addr_to_bcd         ; convert the value at TMP_ADDR to a BCD
    jsr print_bcd           ; print the result to the screen
    lda #$5f                ; queue up a blank space
    sta NEXT_TILE           ; print the blank space
    jsr exp_wait_for_nmi
    jsr inc_ppu_position
    lda TMP_ADDR
    clc
    adc #80                 ; add 80 to the temporary address
    sta TMP_ADDR
    bcc +
    inc TMP_ADDR+1
+
    jsr addr_to_bcd         ; convert the value at TMP_ADDR to a BCD
    jsr print_bcd           ; print the result to the screen
    rts

+next_instr:
    cmp #INSTR_PLAY_TIME
    bne +next_instr
    lda #0
    sta BCD_INPUT+1
    lda HOURS
    sta BCD_INPUT
    jsr bin_to_bcd
    jsr clear_bcd_zeros
    ldx #2
    jsr print_bcd_x

    lda #$44
    sta NEXT_TILE
    jsr exp_wait_for_nmi
    jsr inc_ppu_position

    lda MINUTES
    sta BCD_INPUT
    jsr bin_to_bcd
    ldx #2
    jsr print_bcd_x

    lda #$44
    sta NEXT_TILE
    jsr exp_wait_for_nmi
    jsr inc_ppu_position

    lda SECONDS
    sta BCD_INPUT
    jsr bin_to_bcd
    ldx #2
    jsr print_bcd_x

    lda #$47
    sta NEXT_TILE
    jsr exp_wait_for_nmi
    jsr inc_ppu_position

    lda FRAMES
    sta BCD_INPUT
    jsr bin_to_bcd
    ldx #2
    jsr print_bcd_x

+next_instr:
    cmp #INSTR_SHOW_NUMBER
    bne +next_instr
    ldy #0
    lda (INTPTR_ADDR),y      ; copy the data at the stats pointer into TMP_ADDR
    sta TMP_ADDR
    iny
    lda (INTPTR_ADDR),y
    sta TMP_ADDR + 1
    jsr addr_to_bcd         ; convert the value at the address to BCD
    ldx #4
    jsr print_bcd           ; print the last 4 digits of the BCD to the screen
    lda #2
    jsr add_intptr_addr      ; Add 2 to the script pointer
    rts

+next_instr:
    cmp #INSTR_SET_CURSOR
    bne +next_instr
    ldy #1
    lda (INTPTR_ADDR),y      ; move the cursor to the indicated position
    dey
    pha
    asl                     ; set the low byte of ppu addr to h * 32 + l
    asl
    asl
    asl
    asl
    ora (INTPTR_ADDR),y
    sta PPU_POSITION
    pla
    lsr                     ; set the high ppu addr to $20 + (h/32)
    lsr
    lsr
    ora #$20
    sta PPU_POSITION+1
    iny
    iny
    tya
    jsr add_intptr_addr
    rts

+next_instr:
    cmp #INSTR_FINISH 
    bne +
    lda #<stats_data        ; we've reached the end of the script, so return
    sta INTPTR_ADDR          ; to the beginning
    lda #>stats_data
    sta INTPTR_ADDR+1
+   rts

addr_to_bcd:                ; converts a 2 byte value at TMP_ADDR to a BCD
    pha
    ldy #0
    lda (TMP_ADDR),y
    sta BCD_INPUT
    iny
    lda (TMP_ADDR),y
    sta BCD_INPUT,y
    jsr bin_to_bcd
    jsr clear_bcd_zeros
    pla
    tay
    rts

print_bcd:                  ; prints 4 characters of the BCD to the screen
    ldx #4
print_bcd_x:                ; prints X characters of the BCD to the screen
    dex
-   lda BCD_RESULT,x
    sta NEXT_TILE
    jsr exp_wait_for_nmi
    jsr inc_ppu_position
    dex
    bpl -
    rts

inc_ppu_position:           ; increments the position of the cursor
    lda #1
add_ppu_position:           ; adds A to the position of the cursor
    clc
    adc PPU_POSITION
    sta PPU_POSITION
    lda PPU_POSITION+1
    adc #0
    sta PPU_POSITION+1
    rts

inc_intptr_addr:             ; increments the script pointer
    lda #1
add_intptr_addr:             ; adds A to the script pointer
    clc
    adc INTPTR_ADDR
    sta INTPTR_ADDR
    lda INTPTR_ADDR+1
    adc #0
    sta INTPTR_ADDR+1
    rts

credits_data:

    .db INSTR_SET_CURSOR, 10, 13
    .db INSTR_REPEAT_CHAR, 12, " "
    .db INSTR_SET_CURSOR, 10, 14
    .db INSTR_REPEAT_CHAR, 12, " "

    .db INSTR_WRITE_ZP, NMI_FLAGS, NMI_INIT_PPU

    .db INSTR_SET_PPUATTR, $00, 8, $55
    .db INSTR_SET_PPUATTR, $01, 4, $f5
    .db INSTR_SET_PPUATTR, $41, 1, $f9
    .db INSTR_SET_PPUATTR, $51, 3, $fa

    .db INSTR_WRITE_ZP, RAW_CHAR_FLAG, 1
    .db INSTR_SET_CURSOR, 3, 2
    .db $74, $75, $76, $77, $78, $79, $7a, $7b, $7c, $7d, $7e, $7f, $80
    .db $81, $82, $83, $84, $85, $86, $85, $86, $87, $88, $89, $8a
    .db INSTR_SET_CURSOR, 3, 3
    .db $8d, $8e, $8f, $90, $91, $92, $93, $94, $95, $96, $97, $98
    .db $99, $9a, $9b, $9c, $9d, $9e, $9f, $9e, $9f, $a0, $a1, $a2, $a3
    .db INSTR_SET_CURSOR, 7, 4
    .db $a5, $5f, $a6, $a7
    .db INSTR_SET_CURSOR, 17, 4
    .db $a8
    .db INSTR_WRITE_ZP, RAW_CHAR_FLAG, 0

    .db "RANDOMIZER"

    .db INSTR_SET_PPUATTR, $22, 4, $ff

    .db INSTR_SET_CURSOR, 8, 10
    .db " DEVELOPED BY"
    .db INSTR_SET_CURSOR, 8, 12
    .db "    MCGREW   "

    .db INSTR_WAIT, 240

    .db INSTR_SET_CURSOR, 8, 7
    .db "CONTRIBUTORS "
    .db INSTR_SET_PPUATTR, $22, 4, $0

    .db INSTR_SET_CURSOR, 8, 10
    .db "gameboyf9     "
    .db INSTR_SET_CURSOR, 8, 12
    .db "CaitSith2     "
    .db INSTR_SET_CURSOR, 8, 14
    .db "dvj           "
    .db INSTR_SET_CURSOR, 8, 16
    .db "bunder2015    "
    .db INSTR_SET_CURSOR, 8, 18
    .db "Dwedit        "
    .db INSTR_SET_CURSOR, 8, 20
    .db "ArchfieldMonk "
    .db INSTR_SET_CURSOR, 8, 22
    .db "Crumps Brother"

    .db INSTR_WAIT, 240

    .db INSTR_SET_CURSOR, 8, 7
    .db "SPRITE WORK   "
    .db INSTR_SET_CURSOR, 8, 10
    .db "Xarnax42      "
    .db INSTR_SET_CURSOR, 8, 12
    .db "Ryuseishin    "
    .db INSTR_SET_CURSOR, 8, 14
    .db "mcgrew        "
    .db INSTR_SET_CURSOR, 8, 16
    .db "MisterHomes   "
    .db INSTR_SET_CURSOR, 8, 18
    .db "Invenerable   "
    .db INSTR_SET_CURSOR, 8, 20
    .db "EdgeTalvaron  "
    .db INSTR_SET_CURSOR, 8, 22
    .db "shMike        "
    .db INSTR_SET_CURSOR, 8, 24
    .db "bunder2015    "

    .db INSTR_WAIT, 240

export_credit_music_attribution:
    .db INSTR_SET_CURSOR, 8, 7
    .db "              "
    .db INSTR_SET_CURSOR, 1, 10
    .db "                            "
    .db INSTR_SET_CURSOR, 8, 12
    .db "              "
    .db INSTR_SET_CURSOR, 1, 14
    .db "                            "
    .db INSTR_SET_CURSOR, 8, 16
    .db "              "
    .db INSTR_SET_CURSOR, 1, 18
    .db "                            "
    .db INSTR_SET_CURSOR, 8, 20
    .db "              "
    .db INSTR_SET_CURSOR, 1, 22
    .db "                            "
    .db INSTR_SET_CURSOR, 1, 24
    .db "                            "

stats_data:
    .db INSTR_WAIT, 240

    ; The follwing is for stat display
    .db INSTR_SET_CURSOR, 8, 7
    .db INSTR_REPEAT_CHAR, 12, " "

    .db INSTR_SET_CURSOR, 1, 6
    .db " MONSTER     FOUGHT  WON DIED"

    .db INSTR_SET_CURSOR, 1, 8
    .db " Slime         "      ; /* 0 */
    .db INSTR_MON_STAT, 0

    .db INSTR_SET_CURSOR, 1, 10
    .db " Red Slime     "
    .db INSTR_MON_STAT, 1

    .db INSTR_SET_CURSOR, 1, 12
    .db " Drakee        "
    .db INSTR_MON_STAT, 2

    .db INSTR_SET_CURSOR, 1, 14
    .db " Ghost         "
    .db INSTR_MON_STAT, 3

    .db INSTR_SET_CURSOR, 1, 16
    .db " Magician      "
    .db INSTR_MON_STAT, 4

    .db INSTR_SET_CURSOR, 1, 18
    .db " Magidrakee    "
    .db INSTR_MON_STAT, 5

    .db INSTR_SET_CURSOR, 1, 20
    .db " Scorpion      "
    .db INSTR_MON_STAT, 6

    .db INSTR_SET_CURSOR, 1, 22
    .db " Druin         "
    .db INSTR_MON_STAT, 7

    .db INSTR_SET_CURSOR, 1, 24
    .db " Poltergeist   "
    .db INSTR_MON_STAT, 8

    .db INSTR_SET_CURSOR, 1, 26
    .db " Droll         "
    .db INSTR_MON_STAT, 9

    .db INSTR_WAIT, 240

    .db INSTR_SET_CURSOR, 1, 8
    .db " Drakeema      "  ;  /* 10 */
    .db INSTR_MON_STAT, 10

    .db INSTR_SET_CURSOR, 1, 10
    .db " Skeleton      "
    .db INSTR_MON_STAT, 11

    .db INSTR_SET_CURSOR, 1, 12
    .db " Warlock       "
    .db INSTR_MON_STAT, 12

    .db INSTR_SET_CURSOR, 1, 14
    .db " Metal Scorpion"
    .db INSTR_MON_STAT, 13

    .db INSTR_SET_CURSOR, 1, 16
    .db " Wolf          "
    .db INSTR_MON_STAT, 14

    .db INSTR_SET_CURSOR, 1, 18
    .db " Wraith        "
    .db INSTR_MON_STAT, 15

    .db INSTR_SET_CURSOR, 1, 20
    .db " Metal Slime   "
    .db INSTR_MON_STAT, 16

    .db INSTR_SET_CURSOR, 1, 22
    .db " Specter       "
    .db INSTR_MON_STAT, 17

    .db INSTR_SET_CURSOR, 1, 24
    .db " Wolflord      "
    .db INSTR_MON_STAT, 18

    .db INSTR_SET_CURSOR, 1, 26
    .db " Druinlord     "
    .db INSTR_MON_STAT, 19

    .db INSTR_WAIT, 240

    .db INSTR_SET_CURSOR, 1, 8
    .db " Drollmagi     "  ; /* 20 */
    .db INSTR_MON_STAT, 20

    .db INSTR_SET_CURSOR, 1, 10
    .db " Wyvern        "
    .db INSTR_MON_STAT, 21

    .db INSTR_SET_CURSOR, 1, 12
    .db " Rogue Scorpion"
    .db INSTR_MON_STAT, 22

    .db INSTR_SET_CURSOR, 1, 14
    .db " Wraith Knight "
    .db INSTR_MON_STAT, 23

    .db INSTR_SET_CURSOR, 1, 16
    .db " Golem         "
    .db INSTR_MON_STAT, 24

    .db INSTR_SET_CURSOR, 1, 18
    .db " Goldman       "
    .db INSTR_MON_STAT, 25

    .db INSTR_SET_CURSOR, 1, 20
    .db " Knight        "
    .db INSTR_MON_STAT, 26

    .db INSTR_SET_CURSOR, 1, 22
    .db " Magiwyvern    "
    .db INSTR_MON_STAT, 27

    .db INSTR_SET_CURSOR, 1, 24
    .db " Demon Knight  "
    .db INSTR_MON_STAT, 28

    .db INSTR_SET_CURSOR, 1, 26
    .db " Werewolf      "
    .db INSTR_MON_STAT, 29

    .db INSTR_WAIT, 240

    .db INSTR_SET_CURSOR, 1, 8
    .db " Green Dragon  " ; /* 30 */
    .db INSTR_MON_STAT, 30

    .db INSTR_SET_CURSOR, 1, 10
    .db " Starwyvern    "
    .db INSTR_MON_STAT, 31

    .db INSTR_SET_CURSOR, 1, 12
    .db " Wizard        "
    .db INSTR_MON_STAT, 32

    .db INSTR_SET_CURSOR, 1, 14
    .db " Axe Knight    "
    .db INSTR_MON_STAT, 33

    .db INSTR_SET_CURSOR, 1, 16
    .db " Blue Dragon   "
    .db INSTR_MON_STAT, 34

    .db INSTR_SET_CURSOR, 1, 18
    .db " Stoneman      "
    .db INSTR_MON_STAT, 35

    .db INSTR_SET_CURSOR, 1, 20
    .db " Armored Knight"
    .db INSTR_MON_STAT, 36

    .db INSTR_SET_CURSOR, 1, 22
    .db " Red Dragon    "
    .db INSTR_MON_STAT, 37

    .db INSTR_SET_CURSOR, 1, 24
    .db " Dragonlord 1  "
    .db INSTR_MON_STAT, 38

    .db INSTR_SET_CURSOR, 1, 26
    .db " Dragonlord 2  "
    .db INSTR_MON_STAT, 39

    .db INSTR_WAIT, 240

    .db INSTR_SET_CURSOR, 1, 6
    .db " SPELL USAGE"
    .db INSTR_REPEAT_CHAR, 19, " "

    .db INSTR_SET_CURSOR, 1, 8
    .db " HEAL     "
    .db INSTR_SHOW_NUMBER
    .word $665c ; Data address
    .db INSTR_REPEAT_CHAR, 16, " "

    .db INSTR_SET_CURSOR, 1, 10
    .db " HURT     "
    .db INSTR_SHOW_NUMBER
    .word $665e ; Data address
    .db INSTR_REPEAT_CHAR, 16, " "

    .db INSTR_SET_CURSOR, 1, 12
    .db " SLEEP    "
    .db INSTR_SHOW_NUMBER
    .word $6660 ; Data address
    .db INSTR_REPEAT_CHAR, 16, " "

    .db INSTR_SET_CURSOR, 1, 14
    .db " RADIANT  "
    .db INSTR_SHOW_NUMBER
    .word $6661 ; Data address
    .db INSTR_REPEAT_CHAR, 16, " "

    .db INSTR_SET_CURSOR, 1, 16
    .db " STOPSPELL"
    .db INSTR_SHOW_NUMBER
    .word $6664 ; Data address
    .db INSTR_REPEAT_CHAR, 16, " "

    .db INSTR_SET_CURSOR, 1, 18
    .db " OUTSIDE  "
    .db INSTR_SHOW_NUMBER
    .word $6666 ; Data address
    .db INSTR_REPEAT_CHAR, 16, " "

    .db INSTR_SET_CURSOR, 1, 20
    .db " RETURN   "
    .db INSTR_SHOW_NUMBER
    .word $6668 ; Data address
    .db INSTR_REPEAT_CHAR, 16, " "

    .db INSTR_SET_CURSOR, 1, 22
    .db " REPEL    "
    .db INSTR_SHOW_NUMBER
    .word $666a ; Data address
    .db INSTR_REPEAT_CHAR, 16, " "

    .db INSTR_SET_CURSOR, 1, 24
    .db " HEALMORE "
    .db INSTR_SHOW_NUMBER
    .word $666c ; Data address
    .db INSTR_REPEAT_CHAR, 16, " "

    .db INSTR_SET_CURSOR, 1, 26
    .db " HURTMORE "
    .db INSTR_SHOW_NUMBER
    .word $666e ; Data address
    .db INSTR_REPEAT_CHAR, 16, " "

    .db INSTR_SET_CURSOR, 18, 6
    .db "OTHER STATS"

    .db INSTR_SET_CURSOR, 17, 8
    .db "Deaths   "
    .db INSTR_SHOW_NUMBER
    .word $6630 ; Data address

    .db INSTR_SET_CURSOR, 17, 10
    .db "Attacks  "
    .db INSTR_SHOW_NUMBER
    .word $6632 ; Data address

    .db INSTR_SET_CURSOR, 17, 12
    .db "Crits    "
    .db INSTR_SHOW_NUMBER
    .word $6634 ; Data address

;     .db INSTR_SET_CURSOR, 17, 14
;     .db "Misses   "
;     .db INSTR_SHOW_NUMBER
;     .word $6636 ; Data address

    .db INSTR_SET_CURSOR, 17, 14
    .db "Dodges   "
    .db INSTR_SHOW_NUMBER
    .word $6638 ; Data address

    .db INSTR_SET_CURSOR, 17, 16
    .db "Blocks   "
    .db INSTR_SHOW_NUMBER
    .word $663c ; Data address

    .db INSTR_SET_CURSOR, 17, 18
    .db "Ambushes "
    .db INSTR_SHOW_NUMBER
    .word $663e ; Data address

    .db INSTR_SET_CURSOR, 17, 20
    .db "Bonks    "
    .db INSTR_SHOW_NUMBER
    .word $663a ; Data address

    .db INSTR_SET_CURSOR, 17, 24
    .db "Play Time"
    .db INSTR_SET_CURSOR, 19, 26
    .db INSTR_PLAY_TIME

    .db INSTR_WAIT, 240
    .db INSTR_FINISH

stats_done:
        jmp stats_done

calculate_play_time:
    ; divide frames into hours
    ldx #2
-   lda $6f00,x
    sta DIVIDEND,x
    lda hours_divisor,x
    sta DIVISOR,x
    dex
    bpl -
    jsr divide_24_bit
    lda DIVIDEND
    sta HOURS

    ; divide the remainder into minutes
    ldx #2
-   lda REMAINDER,x
    sta DIVIDEND,x
    lda minutes_divisor,x
    sta DIVISOR,x
    dex
    bpl -
    jsr divide_24_bit
    lda DIVIDEND
    sta MINUTES

    ; divide the remainder into seconds
    ; multiply seconds by 10 and divide by 601 for more accuracy
    ldx #2
-   lda REMAINDER,x
    sta MULTIPLICAND,x
    lda seconds_multiplier,x
    sta MULTIPLIER,x
    dex
    bpl -
    jsr multiply_16_bit
    ldx #2
-   lda PRODUCT,x
    sta DIVIDEND,x
    lda seconds_divisor,x
    sta DIVISOR,x
    dex
    bpl -
    jsr divide_24_bit
    lda DIVIDEND
    sta SECONDS

    ; the remainder is frames x 10 (range 0-600).
    ; if it's > 255, subtract 1 to get in the range 0-599.
    lda REMAINDER+1
    beq +
    lda REMAINDER
    sec
    sbc #1
    sta REMAINDER
    lda REMAINDER+1
    sbc #1
    sta REMAINDER+1
    +
    ; divide by 6 to get centiseconds
    ldx #2
-   lda REMAINDER,x
    sta DIVIDEND,x
    lda frames_divisor,x
    sta DIVISOR,x
    dex
    bpl -
    jsr divide_24_bit
    lda DIVIDEND
    sta FRAMES
+   rts


multiply_16_bit:
    ; multiplicand is 16 bits, multiplier is 8 bits.
    ; this function will not work properly if the result is > 16 bits
    lda #0
    sta PRODUCT
    sta PRODUCT+1
    sta PRODUCT+2
    ldx MULTIPLIER
    beq +
    lda MULTIPLICAND
    sta PRODUCT
    lda MULTIPLICAND+1
    sta PRODUCT+1
    dex
    beq +
-   lda PRODUCT
    clc
    adc MULTIPLICAND
    sta PRODUCT
    lda PRODUCT+1
    adc MULTIPLICAND+1
    sta PRODUCT+1
    dex
    bne -
+   rts

divide_24_bit:
    ldx #24           ;repeat for each bit
    lda #0            ;preset remainder to 0
    sta REMAINDER
    sta REMAINDER+1
    sta REMAINDER+2

-divloop:
    asl DIVIDEND    ;dividend lb & hb*2, msb -> Carry
    rol DIVIDEND+1
    rol DIVIDEND+2
    rol REMAINDER    ;remainder lb & hb * 2 + msb from carry
    rol REMAINDER+1
    rol REMAINDER+2
    lda REMAINDER
    sec
    sbc DIVISOR    ;substract divisor to see if it fits in
    tay            ;lb result -> Y, for we may need it later
    lda REMAINDER+1
    sbc DIVISOR+1
    sta PZTEMP
    lda REMAINDER+2
    sbc DIVISOR+2
    bcc +skip   ;if carry=0 then divisor didn't fit in yet

    sta REMAINDER+2    ;else save substraction result as new remainder,
    lda PZTEMP
    sta REMAINDER+1
    sty REMAINDER
    inc DIVIDEND     ;and INCrement result cause divisor fit in 1 times

+skip:
    dex
    bne -divloop
    rts

hours_divisor:
    hex 28 4d 03 ; 216,360 - frames per hour
minutes_divisor:
    hex 16 0e 00 ;   3,606 - frames per minute
seconds_multiplier:
    hex 0a 00 00 ;      10
seconds_divisor:
    hex 59 02 00 ;     601 - 60.1 frames per second
frames_divisor:
    hex 06 00 00 ;       6

bin_to_bcd:
    lda #$00
    sta BCD_RESULT+0
    sta BCD_RESULT+1
    sta BCD_RESULT+2
    sta BCD_RESULT+3
    sta BCD_RESULT+4
    ldx #$10

-   asl BCD_INPUT+0
    rol BCD_INPUT+1
    ldy BCD_RESULT+0
    lda bin_table, y
    rol a
    sta BCD_RESULT+0
    ldy BCD_RESULT+1
    lda bin_table, y
    rol a
    sta BCD_RESULT+1
    ldy BCD_RESULT+2
    lda bin_table, y
    rol a
    sta BCD_RESULT+2
    ldy BCD_RESULT+3
    lda bin_table, y
    rol a
    sta BCD_RESULT+3
    rol BCD_RESULT+4
    dex
    bne -
    rts

clear_bcd_zeros:
    ldx #$05
    dex
-   lda BCD_RESULT,x
    bne +
    lda #$5f
    sta BCD_RESULT,x
    dex
    bne -
+   rts

bin_table:
   .db $00, $01, $02, $03, $04, $80, $81, $82, $83, $84

load_chr_bank_0:
    sta $bfff
    lsr
    sta $bfff
    lsr
    sta $bfff
    lsr
    sta $bfff
    lsr
    sta $bfff
    rts

stats_palette:
    .hex 0f 31 0f 31
    .hex 0f 2a 0f 0f
    .hex 0f 10 0f 10
    .hex 0f 0f 27 27

; clear_screen:
;     lda #$00
;     sta PPUMASK     ; disable rendering
;     lda #$20
;     sta PPUADDR
;     lda #$00
;     sta PPUADDR
;     ldx #$c0
;     ldy #3
;     lda #$5f
; -   sta PPUDATA
;     dex
;     bpl -
;     dey
;     bpl -
;     lda #%00011010
;     rts

init_ppu:
    ; load the palette
    lda #$3f
    sta PPUADDR
    lda #$00
    sta PPUADDR
    ldx #$00
-   lda stats_palette, x
    sta PPUDATA
    inx
    cpx #$10
    bne -
    lda #$23
    sta PPUADDR
    ldx #$c0
    stx PPUADDR
    lda #0
-   sta PPUDATA
    inx
    bne -
    rts
    

exp_reset: 

irq:
nmi:
    pha
    txa
    pha
    tya
    pha
    inc VBLANK

;     lda #$00
;     sta PPUSCROLL
;     sta PPUSCROLL
    inc FRAMECOUNT

    lda #$00
    sta PPUMASK     ; disable rendering

    lda NMI_FLAGS
    and #NMI_INIT_PPU
    beq +
    jsr init_ppu
+   lda #0
    sta NMI_FLAGS

    lda NEXT_TILE      ; load the next tile
    ldx PPUATTR_LEN    ; if this is non-zero, we're doing PPU Attributes
    bne +
    cmp NMI_SKIP_TILE  ; otherwise see if a tile is set
    beq ++
    ldx #1
+   lda PPU_POSITION+1
    sta PPUADDR
    lda PPU_POSITION
    sta PPUADDR
    lda NEXT_TILE
-   sta PPUDATA        ; write it to the PPU
    dex
    bne -
    stx PPUATTR_LEN
    lda NMI_SKIP_TILE
    sta NEXT_TILE
++
    lda #$20
    sta PPUADDR
    lda #$00
    sta PPUADDR
    lda #%00011010
    sta PPUMASK     ; enable rendering
    jsr famistudio_update
    pla
    tay
    pla
    tax
    pla
    rti

exp_wait_for_nmi:
    lda #$00
    sta VBLANK
-   lda VBLANK
    beq -
    rts

;  .include famistudio_conf.asm
.include famistudio_conf.asm
.include famistudio_asm6.asm

.org $c000
