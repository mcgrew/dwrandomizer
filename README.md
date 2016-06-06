This application is a randomizer for Dragon Warrior 1 for NES. This randomizer
will work on either version of the ROM.

## The randomizer has the following features:

### Random overworld map generation

A new overworld map can be generated with each seed. Checks are used to make
sure all towns and caves are accessible. On generated maps, the Rainbow Drop
is used just outside Charlock to create a bridge over the moat.

### Ultra rando mode

New options for a "no holds barred" approach to player growth, spell learning, 
enemy spells, and enemy zones. This makes them (almost) completely random.
All spells will be learned by level 16, and enemies in zones 0 and 1 are limited
to no tougher than scorpions. Encounters in Charlock are limited to Green
Green Dragons and tougher. Forced encounters (Green Dragon, Axe Knight,
Golem) enemy types will also be randomized.

### Randomizing chest contents

Chest contents throughout the game are shuffled. Checks are used to ensure that
quest items (Staff of Rain, Silver Harp, Stones of Sunlight) are not located in 
Charlock castle. Erdrick's Tablet and the Silver Harp are replaced with keys.

### Randomizing enemy attack patterns

This will give some enemies spells they did not originally have. This means you
could have a Red Slime with SLEEP or a Wizard with Fire Breath!

### Randomizing enemy zones

The locations of enemies will be changed. In normal mode, the randomizer ensures
that you will not encounter powerful enemies too early, so you won't see a Red 
Dragon hanging around Tantegel, but you could see a Ghost. Ultra mode removes
this restriction, however.

### Boosting XP/Gold drops

Enemies will now give you more XP and Gold, in line with the SNES remake of the
game. XP and gold drops are limited to 255 due to memory limitations.

### Shuffling town locations

Towns are randomized, so you could end up with Kol where Cantlin should be, or
Hauksness right next to Tantegel!
 
### Shuffling caves

Cave locations are randomized so that the cave entrance can lead to an
unexpected dungeon. Charlock dungeon is left intact, as is the swamp cave
entrance on the Rimuldar side to ensure you can get there.

### Lowering MP requirements

MP requirements for spells are adjusted to be the same as those in the remake.

### Randomized Weapon Shops

Weapon shop inventory will now be random. Every shop will have 5-6 random items.

### Shuffled "searched for" items

The Fairy Flute, Erdrick's Armor, and Erdrick's Token will now be shuffled, so
for example the token could be in Kol instead of the flute, and the armor on the
overworld. Erdrick's Token could also be in a random chest instead of on the 
ground. In addition, the item's location on the overworld is randomized. The
NPC in Cantlin can tell you where it is.

### Randomized Player Attributes

Your character's Strength, Agility, HP and MP can now vary up to 20% from the 
original version.

### Randomized Spell Learning

Spells learning is now randomized. In normal mode, spells will still be learned 
within 2 levels of the original. For instance, in Dragon Warrior, RADIANT is 
normally learned at level 9. It will now be learned somewhere between level 7 
and level 11. In ultra mode, any spell can be learned anywhere from level 1 to
level 16.

### Buffed HEAL

The HEAL spell will now have a range from 10-25 instead of 10-17.

### Earlier REPEL

REPEL will have a normal learning level of 8 instead of 15.

### Faster leveling

The number of experience points required for level ups can be adjusted with the
--fast-leveling and --very-fast-leveling command line options, or via the 
prompts.

## Usage:

Remember that if you are running this using Python instead of the Windows
binary, Python 3.x is required. DWRandomizer does not work with Python 2.x.

### Windows

You can just drag and drop the ROM file onto the exe file. This will prompt you
for which options you want enabled, and generate a new ROM in the same directory 
as the executable. If the application doesn't recognize the file, it will 
prompt you to make sure you really want to run it, as it may generate an invalid
file in this case.

You can also run the application from a command prompt. The command is:
  
    dwrandomizer [options] C:\path\to\dragon_warrior_rom.nes

The path can be omitted if you have the ROM in the same folder as the 
executable. See below for other command line options.

### Linux & Mac OS

Run the python script from the command line: 

    python3 dwrandomizer.py [options] /path/to/dragon_warrior_rom.nes

The path can be omitted if you have the ROM in the same folder as the 
script. See below for other command line options.

## FAQ

### What are the "checksums" in the program output?

The program prints out 2 checksums when generating a new ROM. The ROM checksum
can be used to verify that 2 instances of the game are the same, that is the 
input ROM is the same, and the same seed and flags were used. The IPS checksum
is a checksum that only includes changes for the game. This can be used to
verify that the gameplay will be the same between 2 different ROM versions. If
one player is using PRG0 and the other PRG1, the IPS checksum will still be
identical, indicating the randomizer gameplay will still be the same. This is 
mostly useful for verifying ROMs in races, where normally all players play the 
same randomized game.

## Command line arguments:

#### positional arguments:
    filename              The rom file to use for input

#### optional arguments:
    -h, --help            show the help message and exit
    -r, --no-remake       Do not set enemy HP, XP/Gold drops and MP use up to
                          that of the remake. This will make grind times much
                          longer.
    -c, --no-chests      Do not randomize chest contents.
    -d, --defaults        Run the randomizer with the default options.
    -f, --force           Skip checksums and force randomization. This may
                          produce an invalid ROM if the incorrect file is used.
    -i, --no-searchitems  Do not randomize the locations of searchable items
                          (Fairy Flute, Erdrick's Armor, Erdrick's Token).
    --ips                 Also create an IPS patch for the original ROM
    -f, --fast-leveling   Set XP requirements for each level to 75% of normal.
    -F, --very-fast-leveling
                          Set XP requirements for each level to 50% of normal.
    -g, --no-growth       Do not randomize player stat growth.
    -G, --ultra-growth    Enable ultra randomization of player stat growth.
    -m, --no-spells       Do not randomize the level spells are learned.
    -M, --ultra-spells    Enable ultra randomization of the level spells are
                          learned.
    --no-map              Do not generate a new world map.
    -p, --no-patterns     Do not randomize enemy attack patterns.
    -P, --ultra-patterns  Enable ultra randomization of enemy attack patterns.
    -s SEED, --seed SEED  Specify a seed to be used for randomization.
    -t, --no-towns        Do not randomize towns.
    -w, --no-shops        Do not randomize weapon shops.
    -u, -U, --ultra       Enable all 'ultra' options.
    -z, --no-zones        Do not randomize enemy zones.
    -Z, --ultra-zones     Enable ultra randomization of enemy zones.
    -v, --version         show the version number and exit

## Special Thanks

Special thanks to Ryan8bit for his 
[Formula Guide](http://www.gamefaqs.com/nes/563408-dragon-warrior/faqs/61640), 
as this randomizer may not have been possible without it.  Thanks to Zombero on 
Twitch for providing the Fighter's Ring patch and Zurkei on Twitch for helping
me get the Windows binary working.


###  -- Created by mcgrew
