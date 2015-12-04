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

### Outside
Outside doesn't always return you to where you'd expect it to, though it is 
predictable. It's not a bug, it's a feature. Kappa


### Command line arguments:

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
  -e, --enemies         Do not randomize enemy zones.
  -p, --patterns        Do not randomize enemy attack patterns.
  -s SEED, --seed SEED  Specify a seed to be used for randomization.
  -t, --towns           Do not randomize towns.

## Special Thanks

Special thanks to Ryan8bit for his 
[Formula Guide](http://www.gamefaqs.com/nes/563408-dragon-warrior/faqs/61640), 
as this may not have been possible without it.  Thanks to Zombero on Twitch 
for providing the Fighter's Ring patch and Zurkei on Twitch for helping me get 
the Window binary working.


###  -- Created by mcgrew
