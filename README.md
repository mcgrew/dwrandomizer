This application is a randomizer for Dragon Warrior 1 for NES. This randomizer
will work on either PRG0 or PRG1 ROMs.

## The randomizer has the following features:

### Randomizing chest contents

Chest contents throughout the game are shuffled. Checks are used to ensure that
quest items (Staff of Rain, Silver Harp, Stones of Sunlight) are not located in 
Charlock castle. Erdrick's Tablet is replaced with something more useful.

### Randomizing enemy attack patterns

This will give some enemies spells they did not originally have. This means you
could have a Red Slime with SLEEP or a Wizard with Fire Breath!

### Randomizing enemy zones

The locations of enemies will be changed. The randomizer ensures that you will
not encounter powerful enemies too early, so you won't see a Red Dragon hanging
around Tantegel, but you could see a Ghost.

### Boosting XP/Gold drops

Enemies will now give you more XP and Gold, more inline with the remake versions
of the game. XP and gold drops are limited to 255 due to memory limitations.

### Shuffling town locations

Towns are randomized, so you could end up with Kol where Cantlin should be, or
Hauksness right next to Tantegel!

### Shuffling caves

Cave locations are randomized so that the cave entrance can lead to an
unexpected dungeon. Charlock dungeon is left intact, as is the swamp cave
entrance on the Rimuldar side to ensure you can get there.

### Lowering MP requirements

MP requirements for spells are adjusted to be the same as those in the remakes.

### Randomized Weapon Shops

Weapon shop inventory will now be random. Every shop will have 5-6 random items.

### Shuffled "Searched for" items

The Fairy Flute, Erdrick's Armor, and Erdrick's Token will now be shuffled, so
for example the token could be in Kol instead of the flute, and the armor in the
swamp. In addition, Erdrick's Token could now be in a random chest instead of on
the ground.

### Randomized Player Attributes

Your character's Strength, Agility, HP and MP can now vary up to 20% from the 
original version.

### Randomized Spell Learning

Spells learning is now randomized. Spells will still be learned within 2 levels
of the original. For instance, in Dragon Warrior, RADIANT is normally learned at
level 9. It will now be learned somewhere between level 7 and level 11.

### Buffed HEAL

The HEAL spell will now have a range from 10-25 instead of 10-17.

### Earlier REPEL

REPEL will now be learned at level 8 instead of 15.

### Outside
OUTSIDE doesn't always send you to where you'd expect it to, though it is 
predictable. It's not a bug, it's a feature. Kappa

## Usage:

Remember that if you are running this using Python instead of the exe, Python
2.x will not produce an identical file as Python 3.x. Even though this will run
on both Python 2.7 and 3.x, The random number generator is different between the
2 versions. **Remember this if you are using this for a race.** The exe is built
with Python 3.x, so it is recommended that you to use the 3.x version of Python.

### Windows

If you have a ROM from the GoodNES set, you can just drag and drop the ROM file
onto the exe. This will generate a new ROM in the same directory as the exe. If
This doesn't work, you will need to run the application from a command prompt.
The command would is:
  
  dwrandomizer C:\path\to\dragon_warrior_rom.nes

See below for other command line options.

### Linux & Mac OS

Run the python script from the command line: 

  python dwrandomizer.py /path/to/dragon_warrior_rom.nes

See below for other command line options.

## Command line arguments:

#### positional arguments:
    filename              The rom file to use for input

#### optional arguments:
    -h, --help            show this help message and exit
    -r, --remake          Do not set enemy HP, XP/Gold drops and MP use up to
                          that of the remake. This will make grind times much
                          longer.
    -c, --chests          Do not randomize chest contents.
    -f, --force           Skip checksums and force randomization. This may
                          produce an invalid ROM if the incorrect file is used.
    -l, --repel           Do not move repel to level 8.
    -p, --patterns        Do not randomize enemy attack patterns.
    -s SEED, --seed SEED  Specify a seed to be used for randomization.
    -t, --towns           Do not randomize towns.
    -w, --shops           Do not randomize weapon shops.
    -z, --zones           Do not randomize enemy zones.

## Special Thanks

Special thanks to Ryan8bit for his 
[Formula Guide](http://www.gamefaqs.com/nes/563408-dragon-warrior/faqs/61640), 
as this may not have been possible without it.  Thanks to Zombero on Twitch 
for providing the Fighter's Ring patch and Zurkei on Twitch for helping me get 
the Window binary working.


###  -- Created by mcgrew
