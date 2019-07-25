# Dragon Warrior Randomizer #

This application is a randomizer for Dragon Warrior for NES. This randomizer
will work on either version of the ROM.

If you enjoy Dragon Warrior Randomizer, come join our
[discord community](https://discord.gg/auYUFJ)!

## The randomizer has the following features: ##

#### Random overworld map generation ####

A new overworld map is generated with each seed. Checks are used to make
sure all towns and caves are accessible. On generated maps, the Rainbow Drop
is used just outside Charlock to create a bridge over the moat.

#### Randomizing chest contents ####

Chest contents throughout the game are randomized. Checks are used to ensure that
quest items (Staff of Rain, Silver Harp, Stones of Sunlight) are not located in 
Charlock castle. Erdrick's Tablet is removed.

#### Randomizing enemy attack patterns ####

This will give some enemies spells they did not originally have. This means you
could have a Red Slime with SLEEP, a Wizard with Fire Breath, or any other 
combination!

#### Randomizing enemy zones ####

The locations of enemies will be changed. Randomizer ensures that enemies near
Tantegel Castle are not too tough to defeat at low levels, but beyond that,
anything goes.

#### Random enemy stats, XP, and gold ####

Enemy statisics (strength, agility, HP) can be randomized, as can the amount of
gold and experience you receive from them.

#### Boosting XP/gold drops ####

Enemies will now give you more XP and Gold, in line with the SNES remake of the
game. XP and gold drops are limited to 255 due to game engine limitations.

#### Lowering MP requirements ####

MP requirements for spells are adjusted to be the same as those in the remake.

#### Randomized weapon shops ####

Weapon shop inventory will now be random. Every shop will have 5-6 random items.

#### Random XP requirements ####

Experience requirements to obtain a new level can be randomized.

#### Random weapon prices ####

Weapon prices can be randomized. This does not affect their capability.

#### Shuffled "searched for" items ####

The Fairy Flute, Erdrick's Armor, and Erdrick's Token will now be shuffled, so
for example the token could be in Kol instead of the flute, and the armor on the
overworld. Erdrick's Token or the Flute could also be in a random chest instead
of on the ground. In addition, the item's location on the overworld is random. The
NPC in Cantlin can tell you where it is.

#### Randomized player attributes ####

Your character's Strength, Agility, HP and MP are randomly generated.

#### Randomized spell learning ####

Spells learning is now randomized. Any spell can be learned anywhere from level 1 to
level 16.

#### Buffed HEAL and HURT ####

The HEAL spell has a range from 17-25. The HURT spell has a range from 9-16.

#### Faster leveling ####

The number of experience points required for level ups can be adjusted with the
fast leveling and very fast leveling options. These require 75% and 50% of the 
normal requirements for the game, respectively.

#### Cursed Princess mode ####

Get Princess Gwaelin to take a cursed belt from your inventory when you bring
her to the king to win the game. The game can also still be completed in the
normal manner.

#### Three's Company mode ####

Bring Princess Gwaelin to the Dragonlord and agree to join him in ruling the
world to win in this mode. The game can also still be completed in the normal
manner.

#### Alternate player sprites ####

Over 60 alternate sprites to choose from for the main character.

#### Other changes ####
The Death Necklace is always available in a chest. Putting it on gives you 
+10 attack power at the cost of 25% of your max HP and being cursed. The
Fighter's Ring works (+2 ATK). Other options include faster text, open Charlock
(no Rainbow Drop required) shortened Charlock dungeon, disabling magic keys, 
or more swamp on the overworld.

## Running the randomizer ##

##### Windows #####

Simply click on the executable in your file manager. On Windows, you may want to
place this somewhere easy to find, such as on the Desktop.

##### Mac OSX #####

On Mac OSX, DWRandomizer will require the installation of Qt5. I recommend
installing this via [homebrew](https://brew.sh):
```
brew install qt5
```

The *.app* folder contained in the zip file may be placed inside the
Applications folder for easy access from Launchpad.

##### Linux #####

Qt5 is also required on Linux. This should be available in your package manager.
If you'd prefer not to install this, there is a command line version also
available in the download which does not require Qt.

### Usage ###

Once the application is running, simply select your Dragon Warrior ROM file from
The `ROM File` box, and choose an output directory. The `Seed` box will be pre-
filled with a random seed number. You can generate a new one with the `Random`
button, or choose your own by entering it in the box. The `Flags` box is where
you enter flags for the options you would like to use, or simply choose them with
the check boxes below. If you're not sure, click `Defaults` for the options most
players use. Next, choose your player sprite. `Loto` is the default original 
sprite. There are other sprites to choose from, including sprites from other
famous games. Now click `Randomize`, and you should get a message saying your
ROM has been created. It will be in the directory you specified. Simply run this
using your favorite emulator or flash cart.

##### Options #####

* Shuffle Chests & Search Items: Self-explanatory. Changes what are in chests and search locations.
* Random Weapon Shops: Randomizes what items are available in all weapon shops.
* Random Growth: Randomize the stat changes you receive on each level up.
* Random Spell Learning: Randomizes the level at which you learn each spell.
* Random Enemy Attacks: Enemy spells and abilities are randomized.
  * Dragonlord form 2 may also have HEAL or SLEEP.
* Random Enemy Zones: Enemy locations are random.
* Random Enemy XP & Gold: Randomize the amount of XP and Gold received from enemies
* Random Weapon Prices: Weapon & armor prices are randomized.
* Random XP Requirements: Experience requirements for each level are randomized.
* Enable Menu Wrapping: Enable wrapping of the cursor from top to bottom in menus.
* Enable Death Necklace: Adds additional functionality to the Death Necklace. +10 ATK and -25% HP.
* Shuffle Music: Shuffles which tunes you hear in each location. Battle music is unchanged.
* Disable Music: Disables most game music. Battle music is not disabled, and music plays after an inn stay. These are known issues.
* Big Swamp: Makes approximately 75% of the overworld contain swamp tiles. Ouch!
* Fast Text: Speeds up all text in dialogs.
* Speed Hacks: Speeds up other aspects of the game (spells, fairy flute, death music, etc.)
* Open Charlock: No need to go around collecting items to make a bridge. Just walk right into Charlock!
* Short Charlock: Dragonlord skimped on the dungeon so you go straight to his throne room on entering Charlock.
* Don't Require Magic Keys: Doors will open without a key. The key shop owners have gone on strike.
* Cursed Princess: Make the princess take a cursed belt to win.
* Three's Company: Bring the princess to the Dragonlord and join him to win.

## Compiling from source ##
If you'd prefer to compile your own version from source code, here are some
instructions.

##### Windows #####

The easiest way I've found *so far* to build dwrandomizer on Windows is using
Ubuntu in Windows Subsystem for Linux on Windows 10 along with the MXE
cross-compiler. Setting up this environment is rather involved, but I'll try
to cover it briefly.

First, set up [Windows Subsystem for Linux](https://docs.microsoft.com/en-us/windows/wsl/install-win10)

Next, download [mxe](https://github.com/mxe/mxe) into your Linux filesystem.
Install the needed dependencies and build it with qt5 support. This may take a
while to compile. 

```
apt update
apt -y install automake autopoint bison flex gperf intltool libgdk-pixbuf2.0-dev libtool-bin lzip make p7zip-full ruby unzip
make qt5
```

After this completes, navigate to the `dwrandomizer` source directory in your
terminal and run the following (replacing `/path/to/mxe` with the path to the
mxe directory you just created):

```
mkdir build
cd build
/path/to/mxe/usr/x86_64-w64-mingw32.static/qt5/bin/qmake ../dwrandomizer.pro
make
```

This will compile dwrandomizer and the new executable should be in the release
directory.

##### Mac OSX #####

First, install Qt5 using homebrew.
```
brew install qt5
```
Now, download the randomizer source code and change to that directory.
```
mkdir build
cd build
/usr/local/Cellar/qt5/5.9.1/bin/qmake ../dwrandomizer.pro
make
```
The Qt version number may be different. This will create an app folder that you
can run from finder, or from the command line simply run
```
dwrandomizer.app/Contents/MacOS/dwrandomizer
```

##### Linux #####

First, install Qt5 using your package manager. Download the randomizer source
code, then change to the directory containing the source code.

```
mkdir build
cd build
qmake ../dwrandomizer.pro
make
```

Some compiling will happen, and that's it! you should have a working binary in 
the current directory.

## FAQ ##

#### What is the "checksum" when generating a ROM? ####

The program outputs a checksum when generating a new ROM. The checksum
can be used to verify that 2 instances of the game are the same, e.g. the
input ROM is the same and the same seed and flags were used. This is 
mostly useful for verifying ROMs in races, where normally all players play the 
same randomized game.
