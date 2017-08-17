
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "dwr.h"
#include "dwr_types.h"
#include "map.h"

static void print_usage(const char *command, char *default_flags)
{
    printf("Usage %s <rom_file> [flags [seed]] <output_dir>\n", command);
    printf("\n");
    printf("A randomizer for Dragon Warrior for NES\n");
    printf("\n");
    printf("Flags:\n");
    printf("    C  Randomize chest and searchable item contents.\n");
    printf("    D  Enable Death Necklace functionality (+10 ATK -25%% HP)\n");
    printf("    V  Set XP requirements for each level to 75%% of normal.\n");
    printf("    F  Set XP requirements for each level to 50%% of normal.\n");
    printf("    G  Enable randomization of player stat growth.\n");
    printf("    K  Shuffle the game music. (beta)\n");
    printf("    M  Randomize the level spells are learned.\n");
    printf("    R  Enable menu wrap-around\n");
    printf("    P  Enable randomization of enemy attack patterns.\n");
    printf("    Q  Disables most of the game music (beta). This doesn't\n");
    printf("       work for battle music yet, and someother tunes, such\n");
    printf("       as the flute, death music, victory music, etc., still\n");
    printf("       play.\n");
    printf("    W  Randomize weapon shops.\n");
    printf("    X  Chaos mode.\n");
    printf("    Z  Randomize enemy zones.\n");
    printf("    h  Apply game speed hacks\n");
    printf("    b  Make a larger portion of the terrain swamp\n");
    printf("    k  Don't require keys to open doors\n");
    printf("    o  Open Charlock. No Rainbow Drop required.\n");
    printf("    s  Shortened Charlock\n");
    printf("    t  Enable faster text\n");
    printf("\n");
    printf("If flags are not specified, the flags \"%s\" will be used\n",
            default_flags);
    printf("If the seed is not specified, a random seed will be chosen\n");
}

int main(int argc, char **argv)
{
    uint64_t seed;
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
    if (argc <= 5 || sscanf(argv[argc-2], "%"PRIu64, &seed) != 1) {
        srand(time(NULL));
        seed = ((uint64_t)rand() << 32) | ((uint64_t)rand() & 0xffffffffL);
    }
    printf("Randomizing using seed: %"PRIu64" with flags %s\n", seed, flags);

    dwr_randomize(input_file, seed, flags, output_dir);

    return 0;
}

