
; start_dwr_credits:
; This writes code to RAM address $200 and executes it.
; This code needs to be in RAM since we're changing both banks at once
    lda #$a9         ; lda instruction
    sta $200
    lda #%00000010   ; Change banking mode value
    sta $201
    ldx #$12         ; copy the banking routine from the original game to RAM
-   lda $fe0c, x
    sta $202,x
    dex
    bpl -

    lda #$a9         ; lda instruction
    sta $215
    lda #5           ; value to load load bank 5
    sta $216
    ldx #$15
-   lda $ff96, x   ; copy the banking routine from the original game to RAM
    sta $217,x
    dex
    bpl -
    lda #>(credit_start-1)  ; set the return address to the credits code
    pha                     ; by pushing it to the stack
    lda #<(credit_start-1)
    pha
    jmp $200                ; jump to the bank swap code in RAM

