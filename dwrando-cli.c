
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "dwr.h"
#include "dwr_types.h"
#include "map.h"

int main(int argc, char **argv)
{
    uint64_t seed;
    char flags[256];

    strncpy(flags, "CDGIMPRTWZf", 256);

    if (argc > 1) {
        sscanf(argv[1], "%Lu", &seed);
    } else {
        srand(time(NULL));
        seed = ((uint64_t)rand() << 32) | ((uint64_t)rand() & 0xffffffffL);
    }
    printf("Randomizing using seed: %Lu\n", seed);

    dwr_randomize( "/home/mcgrew/Dropbox/Dragon Warrior (U) (PRG0) [!].nes", 
            seed, flags, "/tmp/");

    return 0;
}

