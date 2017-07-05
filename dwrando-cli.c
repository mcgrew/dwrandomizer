
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "dwr.h"
#include "dwr_types.h"
#include "map.h"

static void print_usage(const char *command)
{
    printf("Usage %s <rom_file> [[flags] seed] <output_dir>\n", command);
    printf("\n");
    printf("A randomizer for Dragon Warrior for NES\n");
    printf("\n");
    printf("Flags:\n");
    printf("    C  Randomize chest contents.\n");
    printf("    D  Enable Death Necklace functionality (+10 ATK -25%% HP)\n");
    printf("    f  Set XP requirements for each level to 75%% of normal.\n");
    printf("    F  Set XP requirements for each level to 50%% of normal.\n");
    printf("    G  Enable ultra randomization of player stat growth.\n");
    printf("    H  Apply game speed hacks\n");
    printf("    I  Randomize the locations of searchable items\n");
    printf("       (Fairy Flute, Erdrick's Armor, Erdrick's Token).\n");
    printf("    K  Shuffle the game music. (beta)\n");
    printf("    M  Randomize the level spells are learned.\n");
    printf("    R  Enable menu wrap-around\n");
    printf("    P  Enable ultra randomization of enemy attack patterns.\n");
    printf("    Q  Disables most of the game music (beta). This doesn't\n");
    printf("       work for battle music yet, and someother tunes, such\n");
    printf("       as the flute, death music, victory music, etc., still\n");
    printf("       play.\n");
    printf("    W  Randomize weapon shops.\n");
    printf("    Z  Randomize enemy zones.\n");
    printf("\n");
    printf("If flags are not specified, the flags \"CDGIMPRTWZf\" will be used\n");
    printf("If the seed is not specified, a random seed will be chosen\n");
}

int main(int argc, char **argv)
{
    uint64_t seed;
    char *flags;
    char default_flags[] = "CDGIMPRTWZf";
    char *input_file, *output_dir;

    if (argc < 3) {
        print_usage(argv[0]);
        exit(0);
    }

    input_file = argv[1];
    output_dir = argv[argc-1];
    if (argc > 4) {
        flags = argv[2];
    } else {
        flags = default_flags;
    }
    if (argc > 3) {
        sscanf(argv[argc-2], "%"PRIu64"u", &seed);
    } else {
        srand(time(NULL));
        seed = ((uint64_t)rand() << 32) | ((uint64_t)rand() & 0xffffffffL);
    }
    printf("Randomizing using seed: %"PRIu64"u with flags %s\n", seed, flags);

    dwr_randomize(input_file, seed, flags, output_dir);

    return 0;
}

