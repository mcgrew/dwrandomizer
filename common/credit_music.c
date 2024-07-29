/** This file is generated by build_music.py */

#include <stdint.h>
#include <stddef.h>
#include "credit_music.h"
#include "dwr_types.h"
#include "patch.h"
#include "expansion.h"

static const uint8_t music_bytes[] = {
    0x01, 0x13, 0x80, 0xc5, 0x80, 0xe0, 0x80, 0xdf, 0x83, 0xdb, 0x86, 0x99, 
    0x88, 0xe2, 0x89, 0xdd, 0x80, 0x00, 0x00, 0x5d, 0x80, 0x6e, 0x80, 0x88, 
    0x80, 0x50, 0x80, 0xb9, 0x80, 0x6e, 0x80, 0x88, 0x80, 0x8b, 0x80, 0xc2, 
    0x80, 0x83, 0x80, 0x88, 0x80, 0x4b, 0x80, 0x98, 0x80, 0x67, 0x80, 0x88, 
    0x80, 0x4b, 0x80, 0x58, 0x80, 0x6e, 0x80, 0x94, 0x80, 0x4b, 0x80, 0x72, 
    0x80, 0xad, 0x80, 0x88, 0x80, 0x4b, 0x80, 0x58, 0x80, 0x6e, 0x80, 0x88, 
    0x80, 0x4b, 0x80, 0x00, 0xc0, 0x7f, 0x00, 0x02, 0x00, 0xc5, 0xc3, 0xc1, 
    0xc0, 0xc1, 0x00, 0x05, 0x00, 0xcf, 0x7f, 0x00, 0x02, 0x00, 0xc5, 0xc7, 
    0xc6, 0xc5, 0xc5, 0xc4, 0xc3, 0x00, 0x07, 0xc0, 0x06, 0xc1, 0x02, 0xc2, 
    0x00, 0x04, 0xc0, 0x7f, 0x00, 0x01, 0x00, 0xca, 0xca, 0xc7, 0xc6, 0xc6, 
    0xc5, 0x04, 0xc4, 0xc4, 0xc3, 0xc3, 0xc2, 0xc2, 0xc1, 0x00, 0x0e, 0xc0, 
    0xc1, 0xc2, 0x00, 0x02, 0x7f, 0x00, 0x00, 0x00, 0xc0, 0x03, 0xc1, 0xc0, 
    0xbf, 0xc0, 0x00, 0x06, 0xc1, 0x7f, 0x00, 0x00, 0x00, 0xc7, 0x02, 0xc5, 
    0xc7, 0x05, 0xc6, 0x02, 0xc5, 0x02, 0xc4, 0x02, 0xc3, 0x02, 0xc2, 0x02, 
    0xc1, 0x02, 0xc0, 0x00, 0x12, 0xc0, 0xc1, 0x03, 0xc2, 0xc1, 0xc2, 0xc1, 
    0xc2, 0xc1, 0xc2, 0x00, 0x09, 0x00, 0xc5, 0xc3, 0x03, 0xc2, 0x04, 0xc1, 
    0x00, 0x06, 0x00, 0xc4, 0xc2, 0xc1, 0xc0, 0x00, 0x04, 0x00, 0x0b, 0x0f, 
    0x40, 0x03, 0x0c, 0x0f, 0x40, 0x06, 0x22, 0x0f, 0x40, 0x0f, 0x1c, 0x0f, 
    0x40, 0x16, 0x1c, 0x0f, 0x40, 0x03, 0x05, 0x80, 0x9b, 0x82, 0x18, 0x21, 
    0x81, 0x00, 0xa1, 0x1d, 0x21, 0x89, 0x00, 0x99, 0x18, 0x21, 0x89, 0x18, 
    0x21, 0x89, 0x18, 0x21, 0x89, 0x1d, 0x24, 0x89, 0x00, 0x8b, 0x18, 0x21, 
    0x89, 0x00, 0x8b, 0x15, 0x1a, 0x97, 0x15, 0x1a, 0x89, 0x15, 0x1a, 0x89, 
    0x15, 0x21, 0x89, 0x18, 0x21, 0x89, 0x18, 0x21, 0x89, 0x00, 0x99, 0x1f, 
    0x26, 0x89, 0x1f, 0x26, 0x89, 0x1d, 0x26, 0x89, 0x22, 0x26, 0x89, 0x00, 
    0x8b, 0x1d, 0x29, 0x97, 0x00, 0x8b, 0x48, 0x1d, 0x24, 0x97, 0x1d, 0x21, 
    0x89, 0x00, 0x99, 0x1a, 0x21, 0x83, 0x00, 0x9f, 0x41, 0x0e, 0xec, 0x80, 
    0x1d, 0x21, 0x89, 0x1a, 0x21, 0x97, 0x15, 0x1a, 0x97, 0x41, 0x0b, 0xea, 
    0x80, 0x18, 0x21, 0x89, 0x00, 0x99, 0x1d, 0x26, 0x83, 0x00, 0x9f, 0x18, 
    0x21, 0x97, 0x00, 0x8b, 0x48, 0x80, 0x09, 0x99, 0x14, 0x8b, 0x00, 0x8b, 
    0x15, 0x8b, 0x82, 0x1a, 0x21, 0x00, 0x87, 0x80, 0x14, 0x8b, 0x15, 0x8b, 
    0x82, 0x1d, 0x24, 0x89, 0x1a, 0x21, 0x89, 0x1a, 0x21, 0x89, 0x1a, 0x18, 
    0x89, 0x00, 0x8b, 0x1a, 0x41, 0x0a, 0xfb, 0x80, 0x80, 0x14, 0x8b, 0x00, 
    0x8b, 0x15, 0x8b, 0x00, 0x8b, 0x14, 0x8b, 0x15, 0x8b, 0x82, 0x1f, 0x26, 
    0x89, 0x1f, 0x26, 0x89, 0x1f, 0x26, 0x89, 0x1f, 0x26, 0x89, 0x00, 0x8b, 
    0x1d, 0x29, 0x97, 0x00, 0x8b, 0x48, 0x21, 0x24, 0x89, 0x1a, 0x21, 0x89, 
    0x80, 0x14, 0x8b, 0x82, 0x1a, 0x21, 0x83, 0x00, 0x83, 0x80, 0x15, 0x8b, 
    0x00, 0x8b, 0x14, 0x8b, 0x15, 0x8b, 0x82, 0x1d, 0x21, 0x83, 0x00, 0x83, 
    0x80, 0x15, 0x8b, 0x14, 0x8b, 0x82, 0x1a, 0x21, 0x81, 0x00, 0x85, 0x80, 
    0x17, 0x8b, 0x82, 0x21, 0x26, 0x81, 0x00, 0x85, 0x80, 0x15, 0x8d, 0x82, 
    0x1a, 0x97, 0x80, 0x15, 0x8b, 0x14, 0x8b, 0x00, 0x8b, 0x15, 0x8d, 0x82, 
    0x1a, 0x00, 0x87, 0x80, 0x14, 0x8b, 0x15, 0x8b, 0x00, 0x99, 0x82, 0x1a, 
    0x21, 0x81, 0x00, 0xa1, 0x1a, 0x21, 0x89, 0x15, 0x1a, 0x97, 0x41, 0x1f, 
    0x54, 0x81, 0x41, 0x0a, 0xfb, 0x80, 0x41, 0x4a, 0x7c, 0x81, 0x8b, 0x00, 
    0x8b, 0x0e, 0x8b, 0x00, 0x8b, 0x10, 0x8b, 0x00, 0x8b, 0x11, 0x8b, 0x00, 
    0x8b, 0x12, 0x8b, 0x13, 0x8b, 0x00, 0xb5, 0x13, 0xb5, 0x48, 0x19, 0x8b, 
    0x1a, 0x8b, 0x82, 0x26, 0x21, 0x00, 0x87, 0x80, 0x15, 0x8b, 0x00, 0x8b, 
    0x82, 0x15, 0x1a, 0x89, 0x15, 0x21, 0x89, 0x1a, 0x21, 0x89, 0x1a, 0x21, 
    0x00, 0x87, 0x80, 0x1a, 0x8b, 0x00, 0x8b, 0x82, 0x1a, 0x21, 0x89, 0x80, 
    0x15, 0x8d, 0x82, 0x1a, 0x89, 0x1d, 0x21, 0x89, 0x1a, 0x21, 0x89, 0x80, 
    0x41, 0x12, 0x1a, 0x82, 0x26, 0x41, 0x0d, 0x30, 0x82, 0x80, 0x1a, 0x8b, 
    0x82, 0x15, 0x1a, 0x89, 0x15, 0x24, 0x89, 0x1a, 0x21, 0x89, 0x48, 0x80, 
    0x41, 0x2a, 0x1a, 0x82, 0x80, 0x16, 0x8b, 0x82, 0x1a, 0x22, 0x00, 0x87, 
    0x80, 0x16, 0x8b, 0x82, 0x26, 0x22, 0x89, 0x80, 0x16, 0x8b, 0x17, 0x8b, 
    0x18, 0x8b, 0x16, 0x8b, 0x18, 0x8b, 0x82, 0x1f, 0x24, 0x00, 0x87, 0x80, 
    0x18, 0x8b, 0x82, 0x1f, 0x24, 0x89, 0x80, 0x18, 0x8b, 0x13, 0xa7, 0x41, 
    0x2a, 0x19, 0x82, 0x80, 0x41, 0x12, 0x1a, 0x82, 0x26, 0x41, 0x0d, 0x30, 
    0x82, 0x41, 0x0b, 0x55, 0x82, 0x48, 0x80, 0x41, 0x2a, 0x1a, 0x82, 0x80, 
    0x18, 0x8b, 0x00, 0x8b, 0x18, 0x8b, 0x00, 0x8b, 0x15, 0x8b, 0x0e, 0x8b, 
    0x13, 0x8b, 0x15, 0x8b, 0x00, 0x99, 0x13, 0x8b, 0x00, 0x8b, 0x13, 0x8b, 
    0x15, 0xa7, 0x48, 0x00, 0x8b, 0x1a, 0xa7, 0x18, 0x8b, 0x82, 0x1f, 0x22, 
    0x00, 0x87, 0x80, 0x18, 0x8b, 0x82, 0x1a, 0x1f, 0x00, 0x87, 0x80, 0x16, 
    0x8b, 0x00, 0x8b, 0x16, 0x8b, 0x00, 0x8b, 0x1a, 0x99, 0x00, 0x8b, 0x18, 
    0x8b, 0x00, 0x99, 0x82, 0x1c, 0x1f, 0x00, 0x95, 0x1f, 0x24, 0x89, 0x1f, 
    0x24, 0x89, 0x1c, 0x1f, 0x89, 0x18, 0x1f, 0x00, 0x87, 0x24, 0x28, 0x89, 
    0x24, 0x28, 0x89, 0x24, 0x2b, 0x89, 0x24, 0x28, 0x89, 0x00, 0x8b, 0x1f, 
    0x24, 0x81, 0x00, 0x93, 0x1f, 0x22, 0x89, 0x48, 0x8d, 0x80, 0x41, 0x1d, 
    0xc9, 0x82, 0xb5, 0x75, 0x4e, 0xff, 0x88, 0x2c, 0x72, 0x4e, 0x01, 0x20, 
    0x00, 0x4e, 0x00, 0x85, 0x71, 0x4e, 0xff, 0x2c, 0x75, 0x81, 0x72, 0x4e, 
    0x01, 0x20, 0x00, 0x4e, 0x00, 0x85, 0x71, 0x4e, 0xff, 0x2c, 0x00, 0x76, 
    0x4e, 0xfc, 0x2c, 0x74, 0x4e, 0xfd, 0x2a, 0x73, 0x28, 0x4e, 0xfc, 0x26, 
    0x24, 0x4e, 0xfd, 0x22, 0x72, 0x4e, 0xfe, 0x24, 0x22, 0x00, 0x71, 0x4e, 
    0xff, 0x30, 0x2e, 0x2c, 0x2a, 0x7f, 0x80, 0x13, 0x8b, 0x15, 0x8b, 0x13, 
    0x8b, 0x0e, 0x8b, 0x13, 0x8b, 0x15, 0x8b, 0x00, 0x8b, 0x15, 0x8b, 0x47, 
    0xdd, 0x80, 0x41, 0x1f, 0x56, 0x81, 0x41, 0x0a, 0xfb, 0x80, 0x41, 0x69, 
    0x7c, 0x81, 0x41, 0x1f, 0x54, 0x81, 0x41, 0x0a, 0xfb, 0x80, 0x41, 0x4a, 
    0x7c, 0x81, 0x41, 0x41, 0x02, 0x82, 0x80, 0x41, 0x12, 0x1a, 0x82, 0x26, 
    0x41, 0x0d, 0x30, 0x82, 0x41, 0x0b, 0x55, 0x82, 0x48, 0x80, 0x41, 0x2a, 
    0x1a, 0x82, 0x41, 0x22, 0x68, 0x82, 0x41, 0x2a, 0x19, 0x82, 0x80, 0x41, 
    0x12, 0x1a, 0x82, 0x26, 0x41, 0x0d, 0x30, 0x82, 0x41, 0x0b, 0x55, 0x82, 
    0x48, 0x80, 0x41, 0x2a, 0x1a, 0x82, 0x41, 0x62, 0xab, 0x82, 0x80, 0x41, 
    0x1d, 0xc9, 0x82, 0x41, 0x12, 0x1a, 0x83, 0x72, 0x4e, 0xfa, 0x20, 0x1e, 
    0x00, 0x71, 0x4e, 0xfc, 0x2c, 0x4e, 0xfd, 0x2a, 0x28, 0x4e, 0xfc, 0x26, 
    0x41, 0x10, 0x59, 0x83, 0x42, 0x6b, 0x83, 0x9b, 0x82, 0x1d, 0x24, 0x81, 
    0x00, 0xa1, 0x21, 0x24, 0x89, 0x00, 0x99, 0x1d, 0x24, 0x89, 0x1d, 0x24, 
    0x89, 0x1d, 0x24, 0x89, 0x1d, 0x24, 0x89, 0x00, 0x8b, 0x1d, 0x26, 0x89, 
    0x00, 0x8b, 0x1d, 0x21, 0x97, 0x41, 0x0c, 0xec, 0x80, 0x1d, 0x24, 0x89, 
    0x00, 0x99, 0x22, 0x2b, 0x89, 0x26, 0x2b, 0x89, 0x1f, 0x26, 0x89, 0x22, 
    0x2b, 0x89, 0x00, 0x8b, 0x21, 0x29, 0x97, 0x00, 0x8b, 0x21, 0x29, 0x97, 
    0x21, 0x26, 0x89, 0x00, 0x99, 0x1d, 0x26, 0x83, 0x00, 0x9f, 0x41, 0x0e, 
    0xeb, 0x83, 0x21, 0x24, 0x89, 0x1d, 0x26, 0x97, 0x1d, 0x21, 0x97, 0x00, 
    0x99, 0x1d, 0x26, 0x89, 0x21, 0x26, 0x89, 0x1d, 0x21, 0x89, 0x1d, 0x26, 
    0x89, 0x00, 0x99, 0x1d, 0x26, 0x83, 0x00, 0x9f, 0x1d, 0x26, 0x97, 0x00, 
    0x8b, 0x80, 0x0e, 0x99, 0x17, 0x8b, 0x00, 0x8b, 0x18, 0x8b, 0x82, 0x1d, 
    0x24, 0x00, 0x87, 0x80, 0x17, 0x8b, 0x18, 0x8b, 0x82, 0x41, 0x16, 0xeb, 
    0x83, 0x1d, 0x21, 0x89, 0x80, 0x17, 0x8b, 0x00, 0x8b, 0x18, 0x8b, 0x00, 
    0x8b, 0x17, 0x8b, 0x18, 0x8b, 0x82, 0x22, 0x2b, 0x89, 0x22, 0x41, 0x11, 
    0x0e, 0x84, 0x89, 0x21, 0x26, 0x89, 0x80, 0x17, 0x8b, 0x82, 0x21, 0x26, 
    0x83, 0x00, 0x83, 0x80, 0x18, 0x8b, 0x00, 0x8b, 0x17, 0x8b, 0x18, 0x8b, 
    0x82, 0x1d, 0x24, 0x83, 0x00, 0x83, 0x80, 0x18, 0x8b, 0x17, 0x8b, 0x82, 
    0x21, 0x24, 0x81, 0x00, 0x85, 0x80, 0x1a, 0x8b, 0x82, 0x21, 0x26, 0x81, 
    0x00, 0x85, 0x80, 0x18, 0x8b, 0x82, 0x1a, 0x21, 0x97, 0x80, 0x18, 0x8b, 
    0x17, 0x8b, 0x00, 0x8b, 0x18, 0x8b, 0x82, 0x1a, 0x21, 0x00, 0x87, 0x80, 
    0x17, 0x8b, 0x18, 0x8b, 0x00, 0x99, 0x82, 0x21, 0x26, 0x81, 0x00, 0xa1, 
    0x21, 0x26, 0x89, 0x1a, 0x21, 0x97, 0x41, 0x10, 0x51, 0x84, 0x82, 0x41, 
    0x16, 0xeb, 0x83, 0x41, 0x13, 0x69, 0x84, 0x41, 0x11, 0x0e, 0x84, 0x41, 
    0x2a, 0x82, 0x84, 0x00, 0x8b, 0x13, 0x8b, 0x00, 0x8b, 0x15, 0x8b, 0x00, 
    0x8b, 0x16, 0x8b, 0x00, 0x8b, 0x17, 0x8b, 0x18, 0x8b, 0x00, 0xb5, 0x18, 
    0xb5, 0x1c, 0x8b, 0x1d, 0x8b, 0x82, 0x21, 0x26, 0x00, 0x87, 0x80, 0x1a, 
    0x8b, 0x00, 0x8b, 0x82, 0x1a, 0x21, 0x89, 0x1d, 0x24, 0x89, 0x21, 0x26, 
    0x89, 0x21, 0x26, 0x00, 0x87, 0x80, 0x1d, 0x8b, 0x00, 0x8b, 0x82, 0x21, 
    0x26, 0x89, 0x80, 0x1a, 0x8b, 0x82, 0x15, 0x21, 0x89, 0x1d, 0x24, 0x89, 
    0x1a, 0x26, 0x89, 0x80, 0x41, 0x19, 0x05, 0x85, 0x80, 0x1f, 0x8b, 0x00, 
    0x8b, 0x82, 0x21, 0x26, 0x89, 0x80, 0x1d, 0x8b, 0x82, 0x1a, 0x21, 0x89, 
    0x1d, 0x24, 0x89, 0x1a, 0x26, 0x89, 0x80, 0x41, 0x16, 0x05, 0x85, 0x24, 
    0x41, 0x14, 0x1f, 0x85, 0x80, 0x1a, 0x8b, 0x82, 0x21, 0x24, 0x00, 0x87, 
    0x80, 0x1a, 0x8b, 0x82, 0x21, 0x26, 0x89, 0x80, 0x1a, 0x8b, 0x1c, 0x8b, 
    0x1d, 0x8b, 0x1a, 0x8b, 0x1c, 0x8b, 0x82, 0x24, 0x28, 0x00, 0x87, 0x80, 
    0x1c, 0x8b, 0x82, 0x24, 0x28, 0x89, 0x80, 0x1c, 0x8b, 0x18, 0xa7, 0x41, 
    0x2b, 0x05, 0x85, 0x80, 0x41, 0x19, 0x05, 0x85, 0x41, 0x12, 0x3c, 0x85, 
    0x80, 0x41, 0x16, 0x05, 0x85, 0x24, 0x41, 0x14, 0x1f, 0x85, 0x80, 0x1d, 
    0x8b, 0x00, 0x8b, 0x1d, 0x8b, 0x00, 0x8b, 0x1a, 0x8b, 0x15, 0x8b, 0x18, 
    0x8b, 0x1a, 0x8b, 0x00, 0x99, 0x18, 0x8b, 0x00, 0x8b, 0x18, 0x8b, 0x1a, 
    0xa7, 0x00, 0x8b, 0x1f, 0xa7, 0x1d, 0x8b, 0x82, 0x22, 0x26, 0x00, 0x87, 
    0x80, 0x1d, 0x8b, 0x82, 0x1f, 0x22, 0x00, 0x87, 0x80, 0x1a, 0x8b, 0x00, 
    0x8b, 0x1a, 0x8b, 0x00, 0x8b, 0x1d, 0x99, 0x00, 0x8b, 0x1c, 0x8b, 0x00, 
    0x99, 0x82, 0x1f, 0x24, 0x00, 0x95, 0x24, 0x28, 0x89, 0x24, 0x28, 0x89, 
    0x1f, 0x24, 0x89, 0x18, 0x24, 0x00, 0x87, 0x28, 0x2b, 0x89, 0x28, 0x2b, 
    0x89, 0x28, 0x2b, 0x89, 0x28, 0x2b, 0x89, 0x00, 0x8b, 0x24, 0x28, 0x81, 
    0x00, 0x93, 0x22, 0x26, 0x89, 0x8d, 0x80, 0x41, 0x1d, 0xbb, 0x85, 0xb5, 
    0x75, 0x88, 0x30, 0x72, 0x24, 0x00, 0x85, 0x71, 0x30, 0x75, 0x81, 0x72, 
    0x24, 0x00, 0x85, 0x71, 0x30, 0x00, 0x76, 0x4e, 0xfe, 0x30, 0x74, 0x4e, 
    0xff, 0x2e, 0x73, 0x4e, 0xfc, 0x2c, 0x4e, 0xfd, 0x2a, 0x28, 0x4e, 0xfc, 
    0x26, 0x72, 0x4e, 0xfe, 0x28, 0x4e, 0xff, 0x26, 0x00, 0x71, 0x34, 0x4e, 
    0x00, 0x32, 0x4e, 0xff, 0x30, 0x2e, 0x7f, 0x80, 0x18, 0x8b, 0x1a, 0x8b, 
    0x18, 0x8b, 0x15, 0x8b, 0x18, 0x8b, 0x1a, 0x8b, 0x00, 0x8b, 0x1a, 0x8b, 
    0x41, 0x10, 0x52, 0x84, 0x82, 0x41, 0x16, 0xeb, 0x83, 0x41, 0x13, 0x69, 
    0x84, 0x41, 0x11, 0x0e, 0x84, 0x41, 0x4a, 0x82, 0x84, 0x41, 0x10, 0x51, 
    0x84, 0x82, 0x41, 0x16, 0xeb, 0x83, 0x41, 0x13, 0x69, 0x84, 0x41, 0x11, 
    0x0e, 0x84, 0x41, 0x2a, 0x82, 0x84, 0x41, 0x41, 0xef, 0x84, 0x80, 0x41, 
    0x19, 0x05, 0x85, 0x41, 0x12, 0x3c, 0x85, 0x80, 0x41, 0x16, 0x05, 0x85, 
    0x24, 0x41, 0x14, 0x1f, 0x85, 0x41, 0x22, 0x5c, 0x85, 0x41, 0x2b, 0x05, 
    0x85, 0x80, 0x41, 0x19, 0x05, 0x85, 0x41, 0x12, 0x3c, 0x85, 0x80, 0x41, 
    0x16, 0x05, 0x85, 0x24, 0x41, 0x14, 0x1f, 0x85, 0x41, 0x62, 0x9e, 0x85, 
    0x80, 0x41, 0x1d, 0xbb, 0x85, 0xb5, 0x75, 0x88, 0x37, 0x41, 0x10, 0x0f, 
    0x86, 0x72, 0x24, 0x4e, 0xfd, 0x22, 0x00, 0x71, 0x4e, 0xfe, 0x30, 0x4e, 
    0xff, 0x2e, 0x4e, 0xfc, 0x2c, 0x4e, 0xfd, 0x2a, 0x41, 0x10, 0x42, 0x86, 
    0x42, 0x54, 0x86, 0x8c, 0x1a, 0x85, 0x00, 0xff, 0xff, 0xff, 0xb5, 0xff, 
    0xff, 0xff, 0x87, 0x1a, 0x81, 0x19, 0x81, 0x18, 0x81, 0x17, 0x81, 0x16, 
    0x81, 0x15, 0x81, 0x14, 0x81, 0x13, 0x81, 0x12, 0x81, 0x11, 0x81, 0x10, 
    0x81, 0x0f, 0x81, 0x0e, 0x81, 0x0d, 0x81, 0x1a, 0x99, 0x19, 0x81, 0x18, 
    0x81, 0x17, 0x81, 0x00, 0xb7, 0x15, 0x85, 0x00, 0x83, 0x1a, 0x97, 0x00, 
    0x8d, 0x1a, 0x85, 0x00, 0x83, 0x18, 0x8d, 0x00, 0x89, 0x19, 0x8d, 0x00, 
    0x89, 0x41, 0x17, 0x03, 0x87, 0x8f, 0x00, 0x87, 0x19, 0x8d, 0x00, 0x89, 
    0x41, 0x1b, 0x03, 0x87, 0x8f, 0x00, 0x87, 0x41, 0x1e, 0x03, 0x87, 0x41, 
    0x1e, 0x03, 0x87, 0x41, 0x17, 0x03, 0x87, 0x8f, 0x00, 0x87, 0x19, 0x8d, 
    0x00, 0x89, 0x41, 0x1e, 0x03, 0x87, 0x13, 0x8f, 0x00, 0x87, 0x15, 0x8f, 
    0x00, 0x87, 0x16, 0x8f, 0x00, 0x87, 0x17, 0x85, 0x00, 0x83, 0x18, 0x85, 
    0x00, 0xbb, 0x18, 0x9f, 0x00, 0x93, 0x1a, 0x8f, 0x00, 0x87, 0x15, 0x8f, 
    0x00, 0x87, 0x18, 0x8f, 0x00, 0x87, 0x1d, 0x85, 0x00, 0x83, 0x1a, 0x85, 
    0x00, 0x91, 0x1a, 0x85, 0x00, 0x83, 0x15, 0x85, 0x00, 0x91, 0x18, 0x8f, 
    0x00, 0x87, 0x19, 0x8f, 0x00, 0x87, 0x41, 0x1c, 0x62, 0x87, 0x13, 0x8f, 
    0x00, 0x87, 0x15, 0x8f, 0x00, 0x87, 0x41, 0x24, 0x62, 0x87, 0x16, 0xbb, 
    0x00, 0x85, 0x22, 0x99, 0x16, 0x8b, 0x15, 0xb9, 0x00, 0x87, 0x18, 0xa7, 
    0x41, 0x24, 0x62, 0x87, 0x41, 0x1c, 0x62, 0x87, 0x13, 0x8f, 0x00, 0x87, 
    0x15, 0x8f, 0x00, 0x87, 0x41, 0x24, 0x62, 0x87, 0x1a, 0x8d, 0x00, 0x89, 
    0x1a, 0x8d, 0x00, 0x89, 0x18, 0x85, 0x00, 0x83, 0x1a, 0x85, 0x00, 0x83, 
    0x18, 0x85, 0x00, 0x83, 0x15, 0x85, 0x00, 0x83, 0x18, 0x8f, 0x00, 0x87, 
    0x18, 0x8f, 0x00, 0x87, 0x18, 0x85, 0x00, 0x83, 0x1a, 0x97, 0x00, 0x8d, 
    0x13, 0xb5, 0x15, 0xb5, 0x16, 0xb5, 0x1a, 0xa7, 0x18, 0x85, 0x00, 0xff, 
    0xe3, 0x41, 0x0b, 0xe0, 0x87, 0xf3, 0x41, 0x11, 0xc0, 0x87, 0x41, 0x0b, 
    0x6f, 0x87, 0x41, 0x1b, 0x03, 0x87, 0x8f, 0x00, 0x87, 0x41, 0x1e, 0x03, 
    0x87, 0x41, 0x17, 0x03, 0x87, 0x8f, 0x00, 0x87, 0x19, 0x8d, 0x00, 0x89, 
    0x41, 0x17, 0x03, 0x87, 0x8f, 0x00, 0x87, 0x19, 0x8f, 0x00, 0x87, 0x41, 
    0x1e, 0x03, 0x87, 0x41, 0x1e, 0x03, 0x87, 0x41, 0x17, 0x03, 0x87, 0x8f, 
    0x00, 0x87, 0x19, 0x8f, 0x00, 0x87, 0x13, 0x8d, 0x00, 0x89, 0x15, 0x8d, 
    0x00, 0x89, 0x16, 0x8d, 0x00, 0x89, 0x17, 0x85, 0x00, 0x83, 0x18, 0x85, 
    0x00, 0xbb, 0x18, 0xa1, 0x00, 0x91, 0x41, 0x24, 0x62, 0x87, 0x41, 0x1c, 
    0x62, 0x87, 0x13, 0x8f, 0x00, 0x87, 0x15, 0x8f, 0x00, 0x87, 0x41, 0x24, 
    0x62, 0x87, 0x41, 0x0e, 0x96, 0x87, 0x41, 0x24, 0x62, 0x87, 0x41, 0x1c, 
    0x62, 0x87, 0x13, 0x8f, 0x00, 0x87, 0x15, 0x8f, 0x00, 0x87, 0x41, 0x24, 
    0x62, 0x87, 0x1a, 0x8f, 0x00, 0x87, 0x1a, 0x8f, 0x00, 0x87, 0x41, 0x11, 
    0xc0, 0x87, 0x8d, 0x00, 0x89, 0x18, 0x8d, 0x00, 0x89, 0x41, 0x15, 0xd8, 
    0x87, 0x41, 0x0b, 0xe0, 0x87, 0xf3, 0x41, 0x11, 0xc0, 0x87, 0x41, 0x0b, 
    0x6f, 0x87, 0x42, 0xfa, 0x87, 0x86, 0x22, 0x83, 0xb1, 0x00, 0x99, 0x84, 
    0x20, 0x83, 0x00, 0x85, 0x20, 0x83, 0x00, 0xa1, 0x20, 0x83, 0x00, 0x85, 
    0x20, 0x83, 0x00, 0xa1, 0x20, 0x83, 0x00, 0x85, 0x20, 0x83, 0x00, 0x85, 
    0x20, 0x83, 0x00, 0x93, 0x41, 0x17, 0xa0, 0x88, 0x41, 0x0d, 0xaf, 0x88, 
    0x41, 0x17, 0xa0, 0x88, 0x41, 0x0d, 0xaf, 0x88, 0x41, 0x17, 0xa0, 0x88, 
    0xa1, 0x20, 0x83, 0x00, 0x85, 0x20, 0x83, 0x00, 0x85, 0x41, 0x21, 0x99, 
    0x88, 0x41, 0x17, 0xa0, 0x88, 0x41, 0x0d, 0xaf, 0x88, 0x41, 0x17, 0xa0, 
    0x88, 0x41, 0x0d, 0xaf, 0x88, 0x41, 0x17, 0xa0, 0x88, 0x41, 0x0d, 0xaf, 
    0x88, 0x41, 0x17, 0xa0, 0x88, 0x41, 0x0d, 0xaf, 0x88, 0x41, 0x17, 0xa0, 
    0x88, 0x41, 0x0d, 0xaf, 0x88, 0x41, 0x17, 0xa0, 0x88, 0xa1, 0x20, 0x83, 
    0x00, 0x85, 0x20, 0x83, 0x00, 0xff, 0xe5, 0x86, 0x21, 0xb5, 0x41, 0x19, 
    0x9d, 0x88, 0x41, 0x10, 0xa7, 0x88, 0x41, 0x11, 0xa7, 0x88, 0x9b, 0x41, 
    0x17, 0xa0, 0x88, 0x41, 0x10, 0xa7, 0x88, 0x41, 0x10, 0xa7, 0x88, 0xa1, 
    0x20, 0x83, 0x00, 0x85, 0x20, 0x83, 0x00, 0x85, 0x9b, 0x41, 0x17, 0xa0, 
    0x88, 0x41, 0x10, 0xa7, 0x88, 0x41, 0x10, 0xa7, 0x88, 0x41, 0x0a, 0x2f, 
    0x89, 0x41, 0x17, 0xa0, 0x88, 0xa1, 0x20, 0x83, 0x00, 0x85, 0x20, 0x83, 
    0x00, 0xf5, 0x20, 0x83, 0x00, 0x85, 0x20, 0x83, 0x00, 0x85, 0x8a, 0x21, 
    0xa9, 0x00, 0xa5, 0xe1, 0x84, 0x20, 0x83, 0x00, 0xff, 0xff, 0xd5, 0xe1, 
    0x20, 0x83, 0x00, 0xff, 0xff, 0xd5, 0x86, 0x22, 0x83, 0x7f, 0x41, 0x1f, 
    0x9c, 0x88, 0x41, 0x17, 0xa0, 0x88, 0x41, 0x0d, 0xaf, 0x88, 0x41, 0x17, 
    0xa0, 0x88, 0x41, 0x0d, 0xaf, 0x88, 0x41, 0x17, 0xa0, 0x88, 0x41, 0x0d, 
    0xaf, 0x88, 0x41, 0x17, 0xa0, 0x88, 0x41, 0x0d, 0xaf, 0x88, 0x41, 0x17, 
    0xa0, 0x88, 0x41, 0x0d, 0xaf, 0x88, 0x41, 0x17, 0xa0, 0x88, 0x41, 0x0c, 
    0x09, 0x89, 0x41, 0x19, 0x9d, 0x88, 0x41, 0x10, 0xa7, 0x88, 0x41, 0x11, 
    0xa7, 0x88, 0x9b, 0x41, 0x17, 0xa0, 0x88, 0x41, 0x10, 0xa7, 0x88, 0x41, 
    0x10, 0xa7, 0x88, 0x41, 0x0a, 0x2f, 0x89, 0x41, 0x17, 0xa0, 0x88, 0x41, 
    0x10, 0xa7, 0x88, 0x41, 0x10, 0xa7, 0x88, 0x41, 0x0a, 0x2f, 0x89, 0x41, 
    0x17, 0xa0, 0x88, 0x41, 0x23, 0x4d, 0x89, 0x42, 0x72, 0x89, 0x01, 0x81, 
    0x00, 0x95, 0x02, 0x81, 0x00, 0x87, 0x02, 0x81, 0x00, 0x87, 0x03, 0x8b, 
    0x00, 0x8b, 0x02, 0x81, 0x00, 0x87, 0x04, 0x89, 0x00, 0x41, 0x0e, 0xe2, 
    0x89, 0x05, 0x89, 0x00, 0x02, 0x81, 0x00, 0x87, 0x04, 0x89, 0x00, 0x41, 
    0x0e, 0xe2, 0x89, 0x02, 0x81, 0x00, 0x87, 0x02, 0x81, 0x00, 0x87, 0x04, 
    0x89, 0x00, 0x01, 0x81, 0x00, 0x95, 0x05, 0x89, 0x00, 0x02, 0x81, 0x00, 
    0x87, 0x03, 0x8b, 0x05, 0x89, 0x00, 0x02, 0x81, 0x00, 0x87, 0x02, 0x81, 
    0x00, 0x87, 0x41, 0x0e, 0xe2, 0x89, 0x41, 0x23, 0x0b, 0x8a, 0x41, 0x0e, 
    0xe2, 0x89, 0x41, 0x23, 0x0b, 0x8a, 0x41, 0x17, 0xe2, 0x89, 0x41, 0x0e, 
    0xe2, 0x89, 0x41, 0x0a, 0xfd, 0x89, 0x41, 0x0d, 0xe2, 0x89, 0x8d, 0x00, 
    0x89, 0x41, 0x1f, 0x0f, 0x8a, 0x41, 0x0d, 0xe2, 0x89, 0x8d, 0x00, 0x89, 
    0x41, 0x1f, 0x0f, 0x8a, 0x41, 0x0d, 0xe2, 0x89, 0x8d, 0x00, 0x89, 0x41, 
    0x1f, 0x0f, 0x8a, 0x41, 0x17, 0xe2, 0x89, 0x41, 0x18, 0x16, 0x8a, 0x41, 
    0x0d, 0xe2, 0x89, 0x8d, 0x00, 0x89, 0x41, 0x1f, 0x0f, 0x8a, 0x41, 0x0d, 
    0xe2, 0x89, 0x8d, 0x00, 0x89, 0x41, 0x1f, 0x0f, 0x8a, 0x01, 0x81, 0x00, 
    0x95, 0x03, 0x8d, 0x00, 0x89, 0x01, 0x81, 0x00, 0x95, 0x03, 0x8b, 0x03, 
    0x8d, 0x00, 0xb3, 0x03, 0x8b, 0x03, 0x8b, 0x03, 0x8b, 0x03, 0x8b, 0x41, 
    0x17, 0xe2, 0x89, 0x41, 0x0e, 0xe2, 0x89, 0x41, 0x0a, 0xfd, 0x89, 0x41, 
    0x0e, 0xe2, 0x89, 0x41, 0x23, 0x0b, 0x8a, 0x41, 0x0e, 0xe2, 0x89, 0x41, 
    0x23, 0x0b, 0x8a, 0x41, 0x0e, 0xe2, 0x89, 0x41, 0x23, 0x0b, 0x8a, 0x41, 
    0x0e, 0xe2, 0x89, 0x41, 0x23, 0x0b, 0x8a, 0x41, 0x0e, 0xe2, 0x89, 0x41, 
    0x23, 0x0b, 0x8a, 0x41, 0x0e, 0xe2, 0x89, 0x41, 0x23, 0x0b, 0x8a, 0x01, 
    0x81, 0x00, 0x95, 0x03, 0x8d, 0x00, 0x89, 0x03, 0x8b, 0x03, 0x8b, 0x41, 
    0x0e, 0x95, 0x8a, 0x01, 0x81, 0x00, 0xb1, 0x03, 0x8d, 0x00, 0x97, 0x01, 
    0x81, 0x00, 0x87, 0x01, 0x81, 0x00, 0x95, 0x01, 0x81, 0x00, 0x95, 0x03, 
    0x8d, 0x00, 0xa5, 0x41, 0x0e, 0xe2, 0x89, 0x02, 0x81, 0x00, 0x87, 0x02, 
    0x81, 0x00, 0x87, 0x02, 0x81, 0x00, 0x87, 0x01, 0x41, 0x0a, 0x0c, 0x8a, 
    0x8d, 0x03, 0x8b, 0x05, 0x89, 0x00, 0x02, 0x81, 0x00, 0x87, 0x05, 0x89, 
    0x00, 0x41, 0x18, 0xef, 0x8a, 0x41, 0x0e, 0xe2, 0x89, 0x41, 0x0d, 0x0b, 
    0x8b, 0x41, 0x0a, 0x0c, 0x8a, 0x8d, 0x03, 0x8b, 0x03, 0x8b, 0x03, 0x8b, 
    0x03, 0x8b, 0x41, 0x17, 0xe2, 0x89, 0x41, 0x0e, 0xe2, 0x89, 0x41, 0x0a, 
    0xfd, 0x89, 0x41, 0x0d, 0xe2, 0x89, 0x8d, 0x00, 0x89, 0x41, 0x1f, 0x0f, 
    0x8a, 0x41, 0x0d, 0xe2, 0x89, 0x8d, 0x00, 0x89, 0x41, 0x1f, 0x0f, 0x8a, 
    0x41, 0x0d, 0xe2, 0x89, 0x8d, 0x00, 0x89, 0x41, 0x1f, 0x0f, 0x8a, 0x41, 
    0x17, 0xe2, 0x89, 0x41, 0x18, 0x16, 0x8a, 0x41, 0x0d, 0xe2, 0x89, 0x8d, 
    0x00, 0x89, 0x41, 0x1f, 0x0f, 0x8a, 0x41, 0x0d, 0xe2, 0x89, 0x8d, 0x00, 
    0x89, 0x41, 0x1f, 0x0f, 0x8a, 0x41, 0x1a, 0x89, 0x8a, 0x41, 0x17, 0xe2, 
    0x89, 0x41, 0x0e, 0xe2, 0x89, 0x41, 0x0a, 0xfd, 0x89, 0x41, 0x0e, 0xe2, 
    0x89, 0x41, 0x23, 0x0b, 0x8a, 0x41, 0x0e, 0xe2, 0x89, 0x41, 0x23, 0x0b, 
    0x8a, 0x41, 0x0e, 0xe2, 0x89, 0x41, 0x23, 0x0b, 0x8a, 0x41, 0x0e, 0xe2, 
    0x89, 0x41, 0x23, 0x0b, 0x8a, 0x41, 0x0e, 0xe2, 0x89, 0x41, 0x23, 0x0b, 
    0x8a, 0x41, 0x0e, 0xe2, 0x89, 0x41, 0x23, 0x0b, 0x8a, 0x41, 0x0c, 0xdf, 
    0x8a, 0x41, 0x0e, 0x95, 0x8a, 0x41, 0x18, 0xef, 0x8a, 0x41, 0x0e, 0xe2, 
    0x89, 0x41, 0x0d, 0x0b, 0x8b, 0x41, 0x0a, 0x0c, 0x8a, 0x41, 0x0d, 0x1c, 
    0x8b, 0x41, 0x18, 0xef, 0x8a, 0x41, 0x0e, 0xe2, 0x89, 0x41, 0x0d, 0x0b, 
    0x8b, 0x41, 0x0a, 0x0c, 0x8a, 0x8d, 0x03, 0x8b, 0x03, 0x8b, 0x03, 0x8b, 
    0x03, 0x8b, 0x42, 0x42, 0x8b, 0x0f, 0x00, 0x00, 0x0b, 0x0f, 0x00, 0x03, 
    0x22, 0x0f, 0x00, 0x12, 0x1c, 0x0f, 0x00, 0x1e, 0x0c, 0x0f, 0x00, 0x21, 
    0x1c, 0x0d, 0x00, 0x2a, 0x2d, 0x0d, 0x00, 0x4a, 0x60, 0x0d, 0x00, 0x2a, 
    0x0b, 0x0e, 0x00, 0x2a, 0x60, 0xff, 0xaa, 0xaa, 0xaa, 0xaa, 0xaf, 0xf0, 
    0x9e, 0x9f, 0xf0, 0x06, 0xae, 0x7c, 0x1b, 0x98, 0xf0, 0x00, 0x03, 0x00, 
    0x0c, 0x01, 0xce, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x1f, 
    0xff, 0xfb, 0xf3, 0xcf, 0xf3, 0xef, 0xde, 0xb8, 0xf9, 0xa6, 0xa2, 0x24, 
    0x39, 0x81, 0x43, 0x80, 0x86, 0x19, 0x02, 0x38, 0x51, 0x87, 0x86, 0x15, 
    0x39, 0x8c, 0xa6, 0x55, 0x4d, 0x16, 0x8c, 0xa3, 0x45, 0x45, 0x90, 0xca, 
    0x56, 0x22, 0x52, 0x70, 0xd6, 0x53, 0x16, 0xd3, 0x54, 0xd7, 0x4b, 0x37, 
    0x4b, 0x5b, 0x6b, 0x56, 0xce, 0xb5, 0x5b, 0x5d, 0x59, 0xb6, 0xd5, 0xab, 
    0xd6, 0xb5, 0xd7, 0x6b, 0x6d, 0xad, 0xae, 0xb6, 0xd6, 0xb5, 0xb5, 0xaa, 
    0xd6, 0xaa, 0xac, 0xaa, 0xa9, 0x54, 0xa9, 0x94, 0xa4, 0xa9, 0x4a, 0x4a, 
    0x8a, 0x92, 0x54, 0xa5, 0x49, 0x29, 0x4a, 0x54, 0xa5, 0x29, 0x52, 0xa5, 
    0x4a, 0xa5, 0x54, 0xa9, 0x54, 0xaa, 0x95, 0x2a, 0xaa, 0x94, 0x95, 0x2a, 
    0xaa, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 
    0x55, 0x55, 0x55, 0x55, 0x55, 0x29, 0x25, 0xa5, 0xda, 0xee, 0x7d, 0xdb, 
    0x2a, 0xa5, 0xca, 0x52, 0x25, 0x22, 0x22, 0x49, 0x6d, 0xbb, 0xdd, 0xb6, 
    0xad, 0x6d, 0xad, 0x2a, 0x42, 0x80, 0x90, 0x54, 0xab, 0x6d, 0xad, 0xed, 
    0x76, 0xdb, 0xb6, 0x2a, 0x49, 0x22, 0x91, 0x24, 0x52, 0xaa, 0xac, 0xed, 
    0xdd, 0xdd, 0xad, 0x55, 0x95, 0x92, 0x44, 0x88, 0x24, 0x55, 0xad, 0xad, 
    0xad, 0xb5, 0xd6, 0xda, 0xb6, 0xaa, 0x24, 0x22, 0x92, 0x52, 0x55, 0x55, 
    0xb5, 0xb6, 0xdd, 0x6d, 0xab, 0x49, 0x22, 0x49, 0x4a, 0x55, 0xaa, 0xaa, 
    0xb4, 0xb6, 0x6e, 0xdb, 0xaa, 0xa6, 0xaa, 0x54, 0x49, 0x12, 0x91, 0x54, 
    0xd5, 0xb6, 0xdb, 0xb6, 0xb5, 0x5a, 0x55, 0xa5, 0x24, 0x49, 0x92, 0x52, 
    0x55, 0xb5, 0xaa, 0xb5, 0xb6, 0x6d, 0x6b, 0x55, 0xa5, 0x24, 0x49, 0x52, 
    0x52, 0x55, 0xd5, 0xb6, 0x6d, 0x6b, 0x55, 0x65, 0xaa, 0xaa, 0x52, 0x4a, 
    0x4a, 0xa9, 0xaa, 0x56, 0xab, 0xd5, 0xd6, 0x5a, 0xab, 0xaa, 0x24, 0x91, 
    0xa4, 0x54, 0xab, 0xb5, 0x6a, 0xb5, 0x56, 0x6b, 0xb5, 0x4a, 0x29, 0xa5, 
    0x52, 0x29, 0x55, 0x4a, 0x55, 0x6b, 0xdb, 0x76, 0x6b, 0x55, 0x2a, 0x25, 
    0x25, 0x25, 0x95, 0x2a, 0xab, 0xd6, 0x5a, 0xad, 0x55, 0xab, 0x56, 0x55, 
    0x55, 0x4a, 0x52, 0x52, 0xaa, 0xb2, 0x6a, 0x6d, 0x6d, 0x6b, 0xb5, 0x54, 
    0x29, 0xa5, 0x52, 0xaa, 0x4a, 0x6b, 0x5a, 0xb5, 0xaa, 0xb5, 0x56, 0xaa, 
    0xd5, 0x55, 0x56, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0x95, 0x55, 
    0x55, 0xb5, 0x56, 0xaa, 0xd5, 0x55, 0x56, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 
    0xaa, 0xaa, 0x95, 0x55, 0x55, 0xfe, 0xff, 0xde, 0xff, 0xe3, 0xef, 0x8c, 
    0x44, 0x40, 0x31, 0x37, 0x09, 0x80, 0x18, 0x60, 0x00, 0xc0, 0x20, 0x2f, 
    0x3a, 0xa3, 0xee, 0x3c, 0xef, 0x77, 0x57, 0xef, 0x9f, 0xaf, 0xff, 0xde, 
    0xbf, 0x9e, 0xc9, 0x24, 0x41, 0x09, 0x09, 0x09, 0x29, 0x00, 0x00, 0x04, 
    0x00, 0x18, 0x74, 0x9b, 0xbf, 0x76, 0xeb, 0xcf, 0x7b, 0xdf, 0xfd, 0xf3, 
    0xe7, 0xc6, 0xf3, 0x81, 0xe2, 0x68, 0x31, 0x1c, 0x31, 0x88, 0x6a, 0xc0, 
    0x8f, 0x06, 0x0a, 0x61, 0xc6, 0x38, 0xd4, 0xc5, 0xc7, 0x2f, 0x1d, 0x63, 
    0xa7, 0xd0, 0xfc, 0x63, 0xcf, 0xb8, 0xf1, 0xb1, 0xcf, 0xf1, 0xdc, 0x76, 
    0x33, 0xee, 0x30, 0xe2, 0xd0, 0x08, 0x38, 0x84, 0x01, 0x42, 0x0e, 0x07, 
    0x0c, 0x1d, 0x1e, 0x7d, 0x9e, 0x7f, 0xe7, 0xef, 0x3f, 0x7f, 0x3f, 0x87, 
    0xdc, 0x63, 0x18, 0x73, 0x0c, 0x23, 0x18, 0x88, 0x53, 0x11, 0x06, 0x0d, 
    0x62, 0x03, 0x3c, 0x33, 0x24, 0xe7, 0x7c, 0xae, 0xf3, 0xe7, 0x1f, 0xf8, 
    0xf0, 0xf3, 0xcf, 0x71, 0xae, 0x1c, 0x8e, 0xcf, 0x08, 0xce, 0x0c, 0xcc, 
    0x18, 0x31, 0xc2, 0x40, 0xe1, 0x8e, 0x18, 0x2c, 0xd1, 0x91, 0xed, 0x9f, 
    0x0f, 0xcf, 0xa7, 0x3c, 0xff, 0x93, 0xc7, 0xcc, 0xf5, 0x38, 0x39, 0xb8, 
    0x19, 0x47, 0x88, 0x46, 0xd6, 0x04, 0x76, 0x31, 0x85, 0x1c, 0x58, 0x63, 
    0x9c, 0x61, 0x79, 0x8c, 0xf3, 0x98, 0xf7, 0x78, 0x27, 0xc7, 0x9f, 0x64, 
    0x77, 0x39, 0x0f, 0x8c, 0xf5, 0x86, 0x2f, 0x91, 0x8c, 0x71, 0xc7, 0x21, 
    0xe1, 0xc4, 0x19, 0x8c, 0x41, 0x66, 0x07, 0xa6, 0x2e, 0x8a, 0xe7, 0x43, 
    0xcf, 0x9e, 0x1f, 0x9e, 0x71, 0xf3, 0xe3, 0xb7, 0x18, 0xfc, 0x63, 0xc3, 
    0x18, 0xc3, 0xb0, 0xe0, 0xe3, 0x0f, 0x08, 0xe0, 0x71, 0x8c, 0xc1, 0x8f, 
    0x19, 0xc3, 0x39, 0x39, 0x99, 0xf1, 0xbc, 0x74, 0x76, 0xe2, 0xf3, 0xc7, 
    0x97, 0x19, 0xe3, 0x63, 0x39, 0x87, 0xc6, 0x37, 0x0f, 0x09, 0xc6, 0x71, 
    0x23, 0x88, 0xe0, 0xa9, 0x83, 0x8b, 0x19, 0x87, 0x66, 0x71, 0x8c, 0xef, 
    0x0d, 0xda, 0xf4, 0x73, 0xee, 0x1c, 0xdb, 0x8e, 0x76, 0x1a, 0x39, 0x0e, 
    0x71, 0xc6, 0x2e, 0x31, 0xc3, 0x9c, 0x59, 0x07, 0xa1, 0xc0, 0xf1, 0x19, 
    0x9c, 0x58, 0xce, 0x2e, 0x2d, 0x6e, 0x3c, 0xe3, 0x39, 0xe3, 0x8e, 0xb8, 
    0xe7, 0x1c, 0xe3, 0xa7, 0x0d, 0x8e, 0x47, 0x94, 0xb7, 0x05, 0xc2, 0x39, 
    0xe6, 0x26, 0x36, 0x19, 0x8c, 0x74, 0x98, 0xcd, 0x47, 0x38, 0x59, 0xe8, 
    0xd3, 0x53, 0xb8, 0xf2, 0x6e, 0xac, 0xb3, 0xb4, 0xe5, 0x71, 0x27, 0x8e, 
    0x8c, 0x8f, 0x1a, 0x3c, 0x8a, 0x73, 0x8e, 0x1c, 0x6b, 0x29, 0x8e, 0x1c, 
    0x2c, 0x68, 0xd9, 0x31, 0xe3, 0x55, 0x47, 0x39, 0xab, 0x2d, 0x97, 0x38, 
    0xd3, 0xb1, 0xce, 0x38, 0xf1, 0xd4, 0xb3, 0x4e, 0xa9, 0x62, 0xad, 0x54, 
    0xb2, 0x38, 0xc9, 0xa8, 0xac, 0xa9, 0x18, 0xe6, 0x51, 0x9a, 0x73, 0x29, 
    0x9e, 0x39, 0x9a, 0xd5, 0xa7, 0x96, 0xd3, 0x65, 0x9d, 0x58, 0xba, 0x61, 
    0xda, 0x46, 0xaa, 0x3c, 0x52, 0xb1, 0xa5, 0x66, 0x63, 0x8d, 0x31, 0xe0, 
    0xe5, 0x95, 0x2a, 0xcc, 0xb2, 0x9c, 0xac, 0xab, 0x39, 0x66, 0xa7, 0x4b, 
    0x99, 0x5a, 0x6a, 0xcc, 0xab, 0x8b, 0x55, 0xa6, 0xa6, 0xaa, 0x3c, 0x5a, 
    0x39, 0x38, 0xe2, 0xa4, 0xb9, 0x30, 0xb9, 0x54, 0xaa, 0x95, 0xaa, 0x65, 
    0x5a, 0x9c, 0xc7, 0x59, 0xb2, 0xae, 0x6b, 0x34, 0xdc, 0xb2, 0xd4, 0xb1, 
    0xd4, 0x63, 0xc5, 0x34, 0x6a, 0x65, 0x69, 0x65, 0x4c, 0xe3, 0x35, 0x4c, 
    0x71, 0xa5, 0x54, 0xd6, 0x33, 0x4d, 0x71, 0x92, 0xea, 0x9a, 0xab, 0x56, 
    0x55, 0x5a, 0xca, 0x6a, 0xce, 0x33, 0x3a, 0x3c, 0x65, 0x55, 0x55, 0x55, 
    0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 
    0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 
    0x55, 0x55, 0x55, 0x55, 0x55, 0x7e, 0xdb, 0x4a, 0x49, 0x04, 0x02, 0x40, 
    0x88, 0x88, 0x48, 0xaa, 0x5a, 0x6b, 0xad, 0xa9, 0xd5, 0xf6, 0xfe, 0xfe, 
    0xdd, 0x6d, 0xad, 0x2a, 0x49, 0x42, 0x44, 0x08, 0x40, 0x20, 0xa9, 0x54, 
    0xad, 0xb5, 0x75, 0xf7, 0xee, 0xba, 0xd6, 0xb6, 0xb6, 0xdb, 0x5d, 0xab, 
    0x94, 0x08, 0x10, 0x00, 0x10, 0x12, 0x29, 0x55, 0xdb, 0xee, 0xee, 0xde, 
    0xbb, 0xfb, 0xee, 0xb6, 0xaa, 0x24, 0x91, 0x44, 0x91, 0x48, 0x92, 0x24, 
    0x25, 0x29, 0x55, 0x56, 0x6b, 0xb5, 0x6d, 0xbb, 0xbb, 0x77, 0x77, 0xeb, 
    0xaa, 0x4a, 0x22, 0x04, 0x21, 0x22, 0x51, 0x52, 0xaa, 0xb4, 0xaa, 0xd5, 
    0x5a, 0xdb, 0xb6, 0xdb, 0x76, 0xdb, 0xb6, 0x56, 0x55, 0x95, 0x94, 0x88, 
    0x10, 0x08, 0x42, 0x52, 0x6a, 0x6d, 0xdb, 0xee, 0xb6, 0xdd, 0xd6, 0xd6, 
    0x5a, 0x55, 0x4d, 0x4b, 0x55, 0x4a, 0x92, 0x24, 0x49, 0x4a, 0x4a, 0x4a, 
    0xa9, 0xaa, 0xd6, 0xb6, 0xed, 0xee, 0xee, 0xb6, 0x6d, 0x55, 0x95, 0x24, 
    0x12, 0x91, 0x24, 0x49, 0x29, 0xa5, 0xaa, 0xaa, 0x56, 0xab, 0x56, 0x6b, 
    0xdb, 0xb6, 0xdd, 0xda, 0xd6, 0xaa, 0x4a, 0x49, 0x44, 0x22, 0x22, 0x91, 
    0x52, 0xa9, 0x56, 0xab, 0x6d, 0xdd, 0xb6, 0xad, 0xd5, 0xaa, 0xaa, 0x5a, 
    0xb5, 0xaa, 0xaa, 0x4a, 0x4a, 0x92, 0x48, 0x22, 0x29, 0xa5, 0x6a, 0xb5, 
    0x6d, 0x5b, 0xbb, 0x6d, 0xdb, 0xb6, 0x56, 0x55, 0x8a, 0x24, 0x49, 0x52, 
    0x4a, 0xa9, 0x54, 0xa9, 0xaa, 0xaa, 0xaa, 0x6a, 0xb5, 0xb6, 0x6d, 0xdb, 
    0xb6, 0x56, 0xad, 0xaa, 0x94, 0x92, 0x44, 0x22, 0x49, 0x4a, 0x55, 0x55, 
    0xad, 0x5a, 0xab, 0xb5, 0xd6, 0x5a, 0xad, 0x56, 0xb5, 0x5a, 0xb5, 0xaa, 
    0x52, 0x29, 0x29, 0x49, 0x24, 0x49, 0xa9, 0xaa, 0x5a, 0x6d, 0xdb, 0xda, 
    0xb6, 0xd6, 0x5a, 0xd5, 0xaa, 0x54, 0xa9, 0x94, 0x52, 0x4a, 0xa9, 0x54, 
    0xaa, 0x54, 0x55, 0x55, 0x55, 0xb5, 0x5a, 0x5b, 0xdb, 0xda, 0xd6, 0x5a, 
    0x95, 0x2a, 0x29, 0x49, 0x49, 0x29, 0xa5, 0x52, 0x55, 0x55, 0xd5, 0xaa, 
    0x55, 0xab, 0xd5, 0x5a, 0xb5, 0x56, 0xab, 0x5a, 0x55, 0x95, 0x2a, 0x25, 
    0x49, 0x52, 0x52, 0xaa, 0xaa, 0xaa, 0xd5, 0x5a, 0x5b, 0x6b, 0xad, 0x56, 
    0xb5, 0x2a, 0x55, 0x55, 0x55, 0xa9, 0x52, 0x29, 0x95, 0x4a, 0xa9, 0x4a, 
    0x55, 0x55, 0xab, 0xd6, 0x5a, 0x6b, 0xad, 0xd6, 0x6a, 0x55, 0x95, 0x4a, 
    0x29, 0xa5, 0x54, 0x2a, 0x55, 0xa5, 0xaa, 0xaa, 0xaa, 0x55, 0xd5, 0xaa, 
    0xd5, 0x5a, 0x6b, 0xb5, 0xaa, 0xaa, 0xaa, 0x54, 0x4a, 0x29, 0xa5, 0x94, 
    0x4a, 0x55, 0xd5, 0xaa, 0xd5, 0x6a, 0xb5, 0x6a, 0x55, 0xab, 0xaa, 0x6a, 
    0x55, 0x55, 0x55, 0xaa, 0x54, 0x4a, 0xa5, 0x54, 0xa9, 0x52, 0xb5, 0xaa, 
    0xd5, 0x5a, 0xad, 0xd5, 0x6a, 0x55, 0x55, 0x55, 0x95, 0x2a, 0x55, 0xaa, 
    0x54, 0xa9, 0x2a, 0x55, 0x55, 0x55, 0x55, 0x55, 0xad, 0x5a, 0xad, 0xb5, 
    0x5a, 0xd5, 0xaa, 0x4a, 0x95, 0x4a, 0xa9, 0x94, 0x2a, 0x55, 0x55, 0x55, 
    0x55, 0xd5, 0xaa, 0x56, 0xb5, 0xaa, 0x56, 0x55, 0xab, 0xaa, 0xaa, 0x56, 
    0xaa, 0x4a, 0xa5, 0x52, 0x2a, 0x95, 0xaa, 0xaa, 0x5a, 0xb5, 0x5a, 0xad, 
    0x5a, 0xb5, 0xaa, 0xaa, 0xaa, 0xaa, 0x4a, 0x55, 0xa5, 0x55, 0xd5, 0xaa, 
    0xd5, 0x6a, 0xd5, 0xaa, 0xaa, 0xaa, 0x4a, 0x49, 0x4a, 0xaa, 0xaa, 0xb6, 
    0xd6, 0x5a, 0xab, 0x56, 0x55, 0xa5, 0x52, 0x4a, 0x29, 0x55, 0x55, 0xd5, 
    0xaa, 0xaa, 0xb5, 0xd6, 0x56, 0xab, 0x52, 0x49, 0x49, 0x29, 0x95, 0xaa, 
    0x6a, 0xb5, 0xd6, 0x56, 0xab, 0xaa, 0xaa, 0x4a, 0x95, 0xaa, 0x94, 0x2a, 
    0x55, 0x55, 0xad, 0xd6, 0x6a, 0xad, 0x5a, 0x95, 0x2a, 0xa5, 0xa4, 0x2a, 
    0x55, 0xad, 0xaa, 0xd5, 0xaa, 0x55, 0xab, 0xaa, 0x54, 0xa5, 0x2a, 0x55, 
    0xa9, 0x52, 0x55, 0xad, 0xb5, 0x56, 0xab, 0xaa, 0xaa, 0x95, 0xaa, 0xaa, 
    0xea, 0x69, 0x23, 0x80, 0x82, 0xa7, 0xff, 0x7f, 0xff, 0x0f, 0x00, 0x00, 
    0x00, 0x60, 0xff, 0xff, 0x3f, 0x00, 0x00, 0x80, 0xff, 0xff, 0xff, 0x01, 
    0x00, 0xf0, 0xff, 0x1f, 0x78, 0x01, 0x89, 0xfc, 0x0f, 0x00, 0xe0, 0xff, 
    0xff, 0x03, 0x00, 0x00, 0xff, 0xff, 0x7f, 0x00, 0x00, 0xff, 0x2e, 0x76, 
    0x22, 0xa3, 0x02, 0xfe, 0xfe, 0x46, 0x0e, 0x20, 0x68, 0xff, 0xff, 0x01, 
    0x80, 0xaa, 0xbd, 0xed, 0x48, 0x40, 0xd5, 0xff, 0x56, 0x15, 0x02, 0x34, 
    0xdd, 0x9b, 0x6b, 0xae, 0x20, 0xa4, 0xda, 0xbb, 0x5a, 0x05, 0x29, 0xed, 
    0x6b, 0x91, 0x90, 0x36, 0xd9, 0x5d, 0xdb, 0x24, 0x52, 0x54, 0x9b, 0x37, 
    0x4d, 0x96, 0xa4, 0x56, 0x6d, 0x8d, 0x14, 0x6a, 0xdb, 0xaf, 0x45, 0x90, 
    0xca, 0x74, 0x55, 0x4d, 0x6d, 0xa5, 0xaa, 0x89, 0x56, 0xd3, 0x56, 0xa5, 
    0xaa, 0xb4, 0xac, 0x59, 0x33, 0x54, 0x54, 0xeb, 0xb5, 0xaa, 0x16, 0x45, 
    0x49, 0xda, 0xda, 0x55, 0x55, 0xa5, 0xaa, 0x54, 0xa5, 0xaa, 0x5a, 0x5b, 
    0xab, 0x4a, 0x52, 0x69, 0x55, 0x55, 0xab, 0xd4, 0x56, 0x95, 0x8a, 0x54, 
    0xad, 0xdb, 0xaa, 0x52, 0x4a, 0x55, 0x55, 0xd3, 0xaa, 0x54, 0x56, 0x5b, 
    0x55, 0x95, 0xa4, 0x55, 0xd6, 0xaa, 0x96, 0xaa, 0x54, 0x55, 0xcd, 0xaa, 
    0xaa, 0xaa, 0xaa, 0x54, 0x55, 0xab, 0x95, 0x2a, 0x55, 0x55, 0xd5, 0xaa, 
    0xaa, 0x56, 0xaa, 0x54, 0x55, 0xad, 0x5a, 0xa5, 0xaa, 0x52, 0x55, 0xad, 
    0xd2, 0x2a, 0xab, 0xaa, 0x2a, 0x55, 0x55, 0xad, 0xaa, 0x4a, 0x55, 0xab, 
    0x6a, 0xa9, 0x52, 0x55, 0x55, 0xab, 0x96, 0xaa, 0x4a, 0x55, 0xb5, 0xaa, 
    0xaa, 0x54, 0x55, 0xd5, 0x6a, 0xaa, 0x2a, 0x55, 0xd5, 0xaa, 0xaa, 0xaa, 
    0xaa, 0xaa, 0x2a, 0xd3, 0x54, 0xd5, 0xaa, 0xaa, 0xaa, 0xaa, 0x52, 0x55, 
    0x55, 0xad, 0xaa, 0xaa, 0xaa, 0x52, 0x55, 0x55, 0xab, 0x55, 0xa9, 0x4a, 
    0x55, 0xd5, 0xaa, 0xaa, 0x4a, 0x55, 0xad, 0xaa, 0x2a, 0x55, 0x55, 0xad, 
    0x5a, 0xaa, 0x4a, 0x55, 0xd5, 0xaa, 0xaa, 0x54, 0xd5, 0xaa, 0xaa, 0xaa, 
    0x54, 0x55, 0xab, 0xaa, 0x2a, 0x4d, 0xad, 0xaa, 0x2a, 0x55, 0xad, 0xaa, 
    0xaa, 0xaa, 0x32, 0x55, 0xad, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0x2a, 
    0x55, 0xd5, 0xaa, 0xaa, 0xaa, 0x54, 0x55, 0xad, 0xaa, 0xaa, 0x52, 0x55, 
    0xb5, 0xaa, 0xaa, 0x52, 0x55, 0xad, 0xaa, 0xaa, 0xca, 0xb2, 0xaa, 0xaa, 
    0xaa, 0x4a, 0xab, 0xaa, 0xaa, 0x52, 0xb5, 0xaa, 0xaa, 0x2a, 0x55, 0xd5, 
    0xaa, 0xaa, 0xaa, 0x52, 0x55, 0xab, 0xaa, 0xaa, 0x2a, 0x55, 0xad, 0xaa, 
    0xaa
};

const size_t track_count = 1;

static void add_attr(dw_rom *rom, int track)
{
    struct music_attr attr_choice[] = {
        { .title =   "          Stage 1           ",
          .game =    "          Mad City          ",
          .artist =  "          The Man           ",
          .arrange = "          Some guy          "}
    };
    const struct music_attr *attr = &attr_choice[track];
    uint8_t *ptr = &rom->expansion[CREDIT_MUSIC_ATTRIBUTION+3];
    if (attr->title) {
        ptr = ppatch(ptr, 14, (const uint8_t*)"  SONG TITLE  ")+3;
        ptr = ppatch(ptr, 28, (const uint8_t*)attr->title)+3;
        ptr += 31;
    }
    if (attr->game) {
        ptr = ppatch(ptr, 14, (const uint8_t*)"     GAME     ")+3;
        ptr = ppatch(ptr, 28, (const uint8_t*)attr->game)+3;
    }
    if (attr->artist) {
        ptr = ppatch(ptr, 14, (const uint8_t*)" COMPOSED BY  ")+3;
        ptr = ppatch(ptr, 28, (const uint8_t*)attr->artist)+3;
    }
    if (attr->arrange) {
        ptr = ppatch(ptr, 14, (const uint8_t*)"  ARRANGED BY  ")+3;
        ptr = ppatch(ptr, 28, (const uint8_t*)attr->arrange)+3;
    }
}

static void add_dpcm(dw_rom *rom, int track)
{
    const uint8_t *dmc_bayou_start = &music_bytes[3077];

    struct music_data dmc_choice[] = {
        { .start = dmc_bayou_start, .size = 0x740 }
    };
    struct music_data *dmc = &dmc_choice[track];
    if (dmc->start)
        ppatch(&rom->expansion[0x8000], dmc->size, dmc->start);
}

void add_music(dw_rom *rom, int track)
{
    const uint8_t *music_bayou_start = &music_bytes[0];

    struct music_data music_choice[] = {
        { .start = music_bayou_start, .size = 0xc05 }
    };

    struct music_data *music = &music_choice[track];
    ppatch(&rom->expansion[0x4000], music->size, music->start);
    add_dpcm(rom, track);
    add_attr(rom, track);
}

