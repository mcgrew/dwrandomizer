.base $c000

FAMISTUDIO_DPCM_OFF:
; .incbin music/music.dmc

.org $fffa

.word nmi
.word $200 ; reset vector
.word irq
