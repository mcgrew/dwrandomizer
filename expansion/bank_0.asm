
.base $8000

.org $bfca
patch_save_previous_coords_start:
save_previous_coords:
  lda X_POS
  sta TMP4
  lda Y_POS
  sta TMP5
  jmp $B1CC ; continue to where hooked code would have gone
patch_save_previous_coords_end:

.org $c000
