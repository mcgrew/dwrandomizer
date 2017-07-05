
#ifndef _DWR_H_
#define _DWR_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define FLAG_0 0x0000000000000001
#define FLAG_1 0x0000000000000002
#define FLAG_2 0x0000000000000004
#define FLAG_3 0x0000000000000008
#define FLAG_4 0x0000000000000010
#define FLAG_5 0x0000000000000020
#define FLAG_6 0x0000000000000040
#define FLAG_7 0x0000000000000080
#define FLAG_8 0x0000000000000100
#define FLAG_9 0x0000000000000200
#define FLAG_A 0x0000000000000400
#define FLAG_B 0x0000000000000800
#define FLAG_C 0x0000000000001000
#define FLAG_D 0x0000000000002000
#define FLAG_E 0x0000000000004000
#define FLAG_F 0x0000000000008000
#define FLAG_G 0x0000000000010000
#define FLAG_H 0x0000000000020000
#define FLAG_I 0x0000000000040000
#define FLAG_J 0x0000000000080000
#define FLAG_K 0x0000000000100000
#define FLAG_L 0x0000000000200000
#define FLAG_M 0x0000000000400000
#define FLAG_N 0x0000000000800000
#define FLAG_O 0x0000000001000000
#define FLAG_P 0x0000000002000000
#define FLAG_Q 0x0000000004000000
#define FLAG_R 0x0000000008000000
#define FLAG_S 0x0000000010000000
#define FLAG_T 0x0000000020000000
#define FLAG_U 0x0000000040000000
#define FLAG_V 0x0000000080000000
#define FLAG_W 0x0000000100000000
#define FLAG_X 0x0000000200000000
#define FLAG_Y 0x0000000400000000
#define FLAG_Z 0x0000000800000000
#define FLAG_a 0x0000001000000000
#define FLAG_b 0x0000002000000000
#define FLAG_c 0x0000004000000000
#define FLAG_d 0x0000008000000000
#define FLAG_e 0x0000010000000000
#define FLAG_f 0x0000020000000000
#define FLAG_g 0x0000040000000000
#define FLAG_h 0x0000080000000000
#define FLAG_i 0x0000100000000000
#define FLAG_j 0x0000200000000000
#define FLAG_k 0x0000400000000000
#define FLAG_l 0x0000800000000000
#define FLAG_m 0x0001000000000000
#define FLAG_n 0x0002000000000000
#define FLAG_o 0x0004000000000000
#define FLAG_p 0x0008000000000000
#define FLAG_q 0x0010000000000000
#define FLAG_r 0x0020000000000000
#define FLAG_s 0x0040000000000000
#define FLAG_t 0x0080000000000000
#define FLAG_u 0x0100000000000000
#define FLAG_v 0x0200000000000000
#define FLAG_w 0x0400000000000000
#define FLAG_x 0x0800000000000000
#define FLAG_y 0x1000000000000000
#define FLAG_z 0x2000000000000000

void dwr_randomize(const char* input_file, uint64_t seed, char *flags, 
        const char* output_dir);

#ifdef __cplusplus
}
#endif
#endif
