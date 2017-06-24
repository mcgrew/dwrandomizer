
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "dwr.h"
#include "dwr_types.h"
#include "map.h"

int main(int argc, char **argv)
{
    uint64_t seed;

    if (argc > 1) {
        sscanf(argv[1], "%Lu", &seed);
    } else {
        srand(time(NULL));
        seed = ((uint64_t)rand() << 32) | ((uint64_t)rand() & 0xffffffffL);
    }
    printf("Randomizing using seed: %u\n", seed);

    dwr_randomize( "/home/mcgrew/Dropbox/Dragon Warrior (U) (PRG0) [!].nes", 
            seed, "CDGIMPRTWZf", "/tmp/");

    return 0;
}

