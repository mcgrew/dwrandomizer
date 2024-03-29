
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "dwr.h"
#include "dwr_types.h"
#include "map.h"
#include "sprites.h"


static void print_usage(const char *command, char *default_flags)
{
    size_t i;

    printf("Usage %s <rom_file> [flags [seed [sprite]]] <output_dir>\n", command);
    printf("\n");
    printf("A randomizer for Dragon Warrior for NES\n");
    printf("\n");
    printf("\n");
    printf("If flags are not specified, the flags \"%s\" will be used\n",
            default_flags);
    printf("If the seed is not specified, a random seed will be chosen\n");
    printf("\n");
    printf("Sprite should be one of: ");
    for(i=0; i < sprite_count(); i++) {
        if (i) {
            printf(", %s", dwr_sprite_names[i]);
        } else {
            printf("%s", dwr_sprite_names[i]);
        }
    }
    printf("\n");
}

int main(int argc, char **argv)
{
    uint64_t seed;
    const char *sprite;
    char *flags;
    char default_flags[] = DEFAULT_FLAGS;
    char *input_file, *output_dir;

    printf("DWRandomizer version %s\n", VERSION);
    if (argc < 3) {
        print_usage(argv[0], default_flags);
        exit(0);
    }

    input_file = argv[1];
    output_dir = argv[argc-1];
    if (argc >= 4) {
        flags = argv[2];
    } else {
        flags = default_flags;
    }
    if (argc < 5 || sscanf(argv[3], "%"PRIu64, &seed) != 1) {
        srand(time(NULL));
        seed = ((uint64_t)rand() << 32) | ((uint64_t)rand() & 0xffffffffL);
    }
    if (argc > 5) {
        sprite = argv[4];
    } else {
        sprite = "Random";
    }

    printf("Randomizing using seed: %"PRIu64" with flags %s\n", seed, flags);

    dwr_randomize(input_file, seed, flags, sprite, output_dir);

    return 0;
}

