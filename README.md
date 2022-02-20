# Dragon Warrior Randomizer #

This application is a randomizer for Dragon Warrior for NES. This randomizer
will work on either version of the ROM.

If you enjoy Dragon Warrior Randomizer, come join our
[discord community](https://discord.gg/SmHeCkD)!

## The randomizer has the following features, and more: ##

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

### Web Version ###

The easiest way to use the randomizer is via your web browser. Simply go to
[https://dwrandomizer.com/release](https://dwrandomizer.com/release). This should
work on all major browsers.

### Desktop Installation ###

#### Windows #####

Simply click on the executable in your file manager. On Windows, you may want to
place this somewhere easy to find, such as on the Desktop.

#### Linux and Mac OSX #####

On Linux the application is available as a debian package, snap image, or AppImage.
For ArchLinux the package is available via the AUR. For Mac OS, there is currently
no package provided as I don't have a way to build one, so you may have to do so
yourself or use the [web-based version](https://dwrandomizer.com/release)

### Usage ###

Once the application is running, simply select your Dragon Warrior ROM file by
clicking the `ROM File` box, and choose an output directory. The `Seed` box will be pre-
filled with a random seed number. You can generate a new one with the `Random Seed`
button, or choose your own by entering it in the box. The `Flags` box is where
you enter flags for the options you would like to use, or simply choose them with
the check boxes below. If you're not sure, click `Presets` to get a list of standard
presets. Now click `Randomize`, and you should be prompted by your browser to 
select a directory do save the file. If you do not get such a dialog, check your
`Downloads` folder for the file. Simply run this using your favorite emulator
or flash cart.

## Compiling from source ##

If you'd prefer to compile your own version from source code, here are some
instructions.

### Web-based or Electron version ###

To build this application, you will first need Emscripten.

* Instructions for installing Emscripten can be found [here](https://emscripten.org/docs/getting_started/downloads.html)

#### Emscripten #####

Install emscripten from https://emscripten.org/docs/getting_started/downloads.html

Instructions for MacOS and linux. These *may* also work on Windows.

```bash
mkdir build
cd build
cmake -DCMAKE_TOOLCHAIN_FILE=<path to emscripten>/cmake/Modules/Platform/Emscripten.cmake ..
make dwrandomizer-cli
```

You'll then have the necessary files in the `release` directory, which
can be served via a local web server. For example:

```sh
cd release/
python -m http.server
```

Then open a web browser to http://localhost:8000

##### Electron #####

First, complete the emscripten build instructions and copy the files from `build/release`
into the `electron` directory (or copy all of these files into a new directory).
Change to that directory.

Some Linux distributions contain a standalone electron in the package manager.
There may also be other ways to install this on other platforms.
In this case, you can simply run electron and pass in the electron directory.

```sh
electron .
```

Otherwise, To build the electron version, you will need node.js, yarn, and electron-builder.

* Instructions for installing node.js can be found [here](https://nodejs.org/en/download/)

After that is complete, install yarn and electron-builder:

```sh
npm install yarn
yarn add electron-builder
```

Next run electron-builder to build the application for the appropriate operating
system:

```sh
node_modules/.bin/electron-builder --windows --linux --macos
```

On Windows, this should result in a executable in the `dist` directory. Copy it to
somewhere appropriate.

On Linux and Macos, the `dist` directory should contain a package you can install.
The linux build currently builds a debian package, a snap image and an AppImage.
Choose the one you want to install.

#### Command Line #####

To build the command line application, you should only need `cmake` installed.

```sh
mkdir build
cd build
cmake .. && make
```

This should result in a command line executable present in the build directory.
The command line application is very barebones, but running without arguments will
give some help. There is currently no way to get the appropriate flagset argument
via this version, so you will have to obtain that elsewhere, such as the
[web-based version](https://dwrandomizer.com/release).

## FAQ ##

#### What is the "checksum" when generating a ROM? ####

The program outputs a checksum at the bottom of the screen when generating a
new ROM. The checksum can be used to verify that 2 instances of the game are
the same, i.e. the input ROM is the same and the same seed and flags were used.
This is mostly useful for verifying ROMs in races, where normally all players
play the same randomized game.
