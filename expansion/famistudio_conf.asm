
FAMISTUDIO_CFG_EXTERNAL = 1
;======================================================================================================================
; 1) SEGMENT CONFIGURATION
;======================================================================================================================

FAMISTUDIO_ASM6_ZP_ENUM   = $00F0
FAMISTUDIO_ASM6_BSS_ENUM  = $0400
; FAMISTUDIO_ASM6_CODE_BASE = $8000

;======================================================================================================================
; 2) AUDIO EXPANSION CONFIGURATION
;======================================================================================================================

; Konami VRC6 (2 extra square + saw)
; FAMISTUDIO_EXP_VRC6          = 1

; Konami VRC7 (6 FM channels)
; FAMISTUDIO_EXP_VRC7          = 1

; Nintendo MMC5 (2 extra squares, extra DPCM not supported)
; FAMISTUDIO_EXP_MMC5          = 1

; Sunsoft S5B (2 extra squares, advanced features not supported.)
; FAMISTUDIO_EXP_S5B           = 1

; Famicom Disk System (extra wavetable channel)
; FAMISTUDIO_EXP_FDS           = 1

; Namco 163 (between 1 and 8 extra wavetable channels) + number of channels.
; FAMISTUDIO_EXP_N163          = 1
; FAMISTUDIO_EXP_N163_CHN_CNT  = 4

;======================================================================================================================
; 3) GLOBAL ENGINE CONFIGURATION
;======================================================================================================================

; FAMISTUDIO_CFG_PAL_SUPPORT   = 1
FAMISTUDIO_CFG_NTSC_SUPPORT  = 1

; Support for sound effects playback + number of SFX that can play at once.
FAMISTUDIO_CFG_SFX_SUPPORT   = 0
FAMISTUDIO_CFG_SFX_STREAMS   = 0

; Blaarg's smooth vibrato technique. Eliminates phase resets ("pops") on square channels. Will be ignored if SFX are
; enabled since they are currently incompatible with each other. This might change in the future.
FAMISTUDIO_CFG_SMOOTH_VIBRATO = 1

; Enables DPCM playback support.
FAMISTUDIO_CFG_DPCM_SUPPORT   = 1

; Must be enabled if you are calling sound effects from a different thread than the sound engine update.
; FAMISTUDIO_CFG_THREAD         = 1

;======================================================================================================================
; 4) SUPPORTED FEATURES CONFIGURATION
;======================================================================================================================
; FAMISTUDIO_USE_FAMITRACKER_TEMPO = 1

FAMISTUDIO_USE_PITCH_TRACK   = 1
FAMISTUDIO_USE_RELEASE_NOTES = 1
FAMISTUDIO_USE_SLIDE_NOTES   = 1
FAMISTUDIO_USE_VIBRATO       = 1
FAMISTUDIO_USE_VOLUME_TRACK  = 1
MISTUDIO_USE_ARPEGGIO        = 1
; FAMISTUDIO_DPCM_OFF = $c000

