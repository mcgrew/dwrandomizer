# 2.1 - More ways to WIN! #
##### Released July 27, 2019 #####

* Cursed Princess mode!
  * In this mode, if you can win the game by getting Gwaelin to take a cursed belt from your inventory when you return her to the king.
* Three's Company mode!
  * In this mode, you can win by bringing the princess to the Dragonlord and accepting his offer to join him.
* Vanilla Map option
  * Allows you to play with the original map instead of a randomized one.
* "Chaos Mode" has been removed and separated into multiple options
* Fixed some issues with disabled/randomized music.
  * Music can now be totally disabled aside from instances where it affects game timing
  * All music is now randomized/disabled, including after inn stays, etc.
* New player sprites:
  * Red Slime
  * Metal Slime
  * Dragonlord
  * Tov the Dog (Dragon Quest IV)
  * Jason (Blaster Master)
  * Bomberman
  * Christine (Arkista's Ring)
  * Kirby
  * Lala (Adventures of Lolo)
  * Lolo (Adventures of Lolo)
  * Pac-Man
  * Ms. Pac-Man
  * Inky
  * Blinky
  * Pinky
  * Clyde
  * Slime (Dragon Quest franchise)
  * Mike (Star Tropics)
  * Waluigi (Mario franchise)
  * Wario (Mario franchise)
  * Doran (Dragon Warrior 4)
  * Yeti (Dragon Warrior 4)
* "Jerk" now takes the token as well as the staff/stones when you receive the rainbow drop
* An option to make metal slimes always run, even when you have low strength.
* "Search" now opens a treasure chest instead of just telling you one is there.
  * It's still faster to use "Take"
* Added an option to change to modern Dragon Quest spell names
* Fixed a bug in Death Necklace max HP calculations
  * This fixes an issue where HP was 1 lower than it should have been with the necklace equipped.
* Some tweaks to map generation which should make them more interesting

# 2.0.6 - Bug fix for potential soft lock #
##### Released Aug 6, 2018 #####

* Fixed an issue where Brecconary being on a different continent from Tantegel Castle could create a soft lock if the swamp cave is inside Tantegel.
  * Brecconary will now always be on the same continent as Tantegel if the swamp cave is in Tantegel.
* Fixed the Fairy Water text for female characters (his -> her).

# 2.0.5 - Bug fix for the new sprites #
##### Released Jul 31, 2018 #####

* This fixes an issue with some of the new sprites where some randomizer changes made their way into the sprite code.
* Affected sprites:
  * Black Belt
  * Black Mage
  * Black Wizard
  * Master

# 2.0.4 - Bug Fixes and New Sprites #
##### Released Jul 29, 2018 #####

* Some tweaks to Chaos Mode
  * Raised Dragonlord form 1's minimum HP to 81
  * Removed abilities from the "wet noodle" monster
  * Restricted the amount of XP given by the "wet noodle" (16-64XP)
  * Fixed the REPEL table so that Chaos Mode enemies will be repelled based on their new stats
* Fixed a bug with forced encounters where running did not always push you back a space
* Fixed a bug where the player could be placed in the water by the return spell under certain conditions
* Added a bunch of new sprite options

# 2.0.3 - Fixed another swamp cave issue #
##### Released Feb 3, 2018 #####

* Fixed an issue where when Tantegel and Garinham were connected by the swamp cave, they would end up on the same continent, creating a map that was not able to be completed.

# 2.0.2 - Bug fixes and tweaks to Chaos Mode #
##### Released Jan 20, 2018 #####

* Fixed an error in the player stat generation
  * A bug was causing player stats to be higher than intended
* Added a slight HP boost to lower levels in Big Swamp Mode
* Changes to (attempt to) ensure that at least one enemy in early zones is beatable at level 1. The lowest strength enemy will have no more than 5HP
* Changed the run chance calculations in chaos mode
  * Running chance in chaos mode is now based on map index rather than enemy index, since enemy index is fairly meainingless in this mode. Charlock will be the hardest place to run, followed by other dungeons, followed by Hauksness and Charlock (top floor), and easiest in the overworld. This should help with progression to level 2 as well.
* Fixed a problem with bridge placement
  * Bridges shouldn't be placed where walkable tiles are located to the north or south of them
* Fixed a problem where Rimuldar could be placed on the wrong continent if the swamp cave was located in Garinham
* Fixed a UI crash in the title screen generation
* Made the random seeds in the UI more random
  * The random seeds in the UI were not so random on Windows.
  * Changed the UI such that a new random seed is chosen after generating a ROM.


# 2.0.1 - Bugfix #
###### Released Oct 10, 2017 ######

* Fixed a read overrun in death_necklace()
  * This was causing a rom difference and checksum mismatch.
  * Bug did not affect gameplay.


# 2.0 - Complete rewrite, split continents and more #
###### Released Oct 7, 2017 ######

* From scratch rewrite using C, C++ and the Qt toolkit.
* Split continents can now occur, causing the swamp cave to be required
* Some NPCs have been moved or removed
   * The roving guard in the throne room has been sacked
   * The old man in the Garinam item shop has been moved
   * The weapon shop in Cantlin now contains 4 counter tiles - the owner can no longer hide!
* Chaos mode
    * Randomizes all enemy stats including Dragonlord form 1.
    * Randomizes experience requirements for levels.
    * Dragonlord form 2 will have between 100 and 230 HP.
    * Dragonlord form 2 could have `SLEEP` or `HEAL`, but also is more susceptible to `STOPSPELL`.
* More speed options
    * Fast Text and Speed Hacks are now separate options
    * Open Charlock - no Rainbow Drop required.
    * Short Charlock - No need for torches because there's no dungeon, just straight to the Dragonlord's floor.
    * Added an option to not require keys for opening doors.
* Big Swamp option - most of the overworld will be filled with swamp.
* Added player sprite options. The player sprite can be changed when generating a ROM.
* Fixed an issue where the player could obtain multiple flutes/tokens if they were in a chest.
    * This would clutter the inventory since they are unable to be discarded.

# 1.3-rc1 - Lots of balance changes and improvements #
###### Pre-release - Released Dec 11, 2016 ######

* Graphical interface! DWRandomizer now has a GUI for a better user experience.
* There is now a Windows installer for DWRandomizer - it will now be added to the start menu.
* The ROM output directory can now be specified in the GUI or from the command line via the `--output-dir` option
* Fixed the Cantlin NPC dialog when the overworld item is more than 100 steps from the castle.
* Added back the silver harp.
   * The NPC in the Northern Shrine will now always have the Staff of Rain.
   * Enemies summoned by the harp will now be a random set of 8 different enemies.
* Buffed some of the weaker spells.
  * `HEAL` now has a range of 18-25 HP.
  * `HURT` now has a range of 9-16 HP.
* Menu wrap-around.
  * Various menus in the game will now have the cursor wrap from top to bottom *(optional)*.
* The Death Necklace will now always appear in it's chest the first time. You can still only get one.
* Death Necklace now has an *optional* function other than cursing you. It can give a +10 ATK at the expense of 25% of your HP (you are still cursed).
* Some enemy balance changes:
  * Reduced Droll HP from 35 to 20
  * Reduced Poltergeist HP from 26 to 23
  * Reduced Specter HP from 48 to 33
  * Reduced Goldman HP from 110 to 35
* Some changes to zone randomization:
  * Zones 1 and 2 will now have Wolf and below only.
  * Tantegel will have 2 of each of these zones surrounding it
    * Zone 1 will be to the east and west
    * Zone 2 will be to the north and south
  * All other zones will be random (using a new algorithm)
    * The new algorithm ensures each zone will appear on the overworld no more than 5 times.



# 1.2.1 - Some bug fixes #
###### Released Aug 6, 2016 ######

* Fixed an issue where Garin's Grave and Tantegel Basement weren't randomized
* Moved "speed hacks" option to a top level question.



# 1.2 - Speed hacks and maps are now more random! #
###### Released Aug 5, 2016 ######

* A new option --speed-hacks (-H) has been added to speed up text and some other aspects of the game.
* Tantegel Castle is no longer confined to its original zone from the vanilla game. Zone 0 will be moved to accomodate.
* A few incorrect monster stats have been fixed:
* Droll HP has been reduced from 43 to 35 (this is what the GBC/SNES release has it as)
* Werewolves will now give 95 XP and 155G instead of 120/185. This also matches values from the GBC/SNES release.
* Gold drops will now always be the maximum amount instead of a random percentage. This is also in line with the re-releases.
* Fixed a bug where Charlock castle could be obstructed, thus making the game impossible to finish.
* Fixed a bug where the program would occasionally get stuck when generating the map.



# 1.1.2 - Growth refinement #
###### Released Jun 25, 2016 ######

Some refinement to growth curves

* Minor changes to growth curves - buffed hp/agi, nerfed str/mp
* Also removed the IPS generation question from prompts (few will want this)
* Specifying --ips on the command line will still generate this file



# 1.1.1 - Fixed a crash #
###### Released Jun 7, 2016 ######

* Fixed an issue where the program crashed when formatting the title screen with short seed numbers.



# 1.1 - Game length improvements #
###### Released Jun 6, 2016 ######

* Added `--fast-leveling` (`-f`) and `--very-fast-leveling` (`-F`) options.
* The XP requirements for each level will be 75% and 50% of normal, respectively.
* Refined the player growth curves so that the average level to finish the game is level 15.
* Raised the encounter rate in zone 0 to the same as other zones
* Added randomizer version, flags, and seed number to the title screen
* Fixed xp/gold drops for Drakeema
* These values have been wrong since the first version - thanks to @gameboy9 for catching this



# 1.0 - We're stable boyz! 1.0!! #
###### Released Apr 26 ######

* Added an IPS checksum and IPS file generation.
* This version has no gameplay differences from 1.0-rc2



# 1.0-rc2 - Minor bug fixes and tweaks #
###### Pre-release - Released Feb 29, 2016 ######

What's New?

* Added a new exit from the Tantegel throne room. Now you can die even faster!
* Moved the maximum spell level in ultra to 16 from 20. Too many grindfests resulted from learning HEALMORE at 20.
* Fixed a bug in normal rando where `HEALMORE` was being moved to level 8 instead of `REPEL`.
* Removed the Silver Harp and replaced it with a key.
* Changed Erdrick's Token chest to contain a key instead of gold if the token is on the ground.
* Fixed a bug where towns were occasionally being placed on top of other towns/castles.



# 1.0-rc1 - 1.0 Release Candidate 1! Randomly Generated Map and More! #
###### Pre-release - Released Jan 12, 2016 ######

Generated Overworld!

* A new overworld map can be generated with each seed. This is enabled by default
* The enemy for forced encounters are now randomized in ultra mode. Also, on generated maps, the golem encounter is moved into Charlock's Throne Room (kind of a miniboss guarding Dragonlord).

No more command line needed!

* When the randomizer is run without options (or drag and drop), you will be prompted for options, removing the need to run from the command line to customize options.

Other stuff...

* Random enemies in Charlock are now limited to Werewolf and above for ultra mode (it was too easy).
* The token location (or whatever replaces it) is now randomized. An NPC in Cantlin will tell you where it is.
* Fixed a grammatical error which has irritated me since 1989.



# 0.9.6 - Ultra Rando Mode Engage! #
###### Pre-release - Released Dec 28, 2015 ######

New Ultra Rando Mode!

* New options for a "no holds barred" approach to player growth, enemy spells,
    enemy zones, and spell learning. This makes them (almost) completely random.
* All spells will be learned by level 20, and enemies in zones 0 and 1 are limited
    to no tougher than scorpions.

This version also includes minor tweaks to zone randomization in normal mode.



# 0.9.5 - Bug Fix! #
###### Pre-release - Released Dec 22, 2015 ######

* Fixed an issue where the game could end up with no Erdrick's Armor or no Fairy Flute.



# 0.9.4 - More randomization! #
###### Pre-release - Released Dec 22, 2015 ######

* Added randomization of player HP, MP, strength, and agility (+/- 20%).
* Added randomization of spell learning levels (+/- 2).
* Added shuffling of Erdrick's Armor, Fairy Flute, and Erdrick's Token locations.
* Patched Northern Shrine to be accessible without the harp.



# 0.9.3 - Cave shuffle fix and Kol shop fix #
###### Pre-release - Released Dec 7, 2015 ######

* The weapon shop in Kol is now randomized.
* Fixed an issue with the swamp cave south exit being moved improperly.



# 0.9.2 - More bug fixes, tweaks, and random shops! #
###### Pre-release - Released Dec 6, 2015 ######

* Weaon shops are now randomized! (Except for Kol, which will be fixed next release)
* `REPEL` will now be learned at level 8.
* `HEAL` will now have a range of 10-25 instead of 10-17.
* Shortened the output filename for better readability on Everdrive.
* Removed the possibility of Axe Knight having strong fire breath
* Several tweaks to avoid a few potentially unwinnable situations.
* Dragonlord HP tweaks:
  * Dragonlord form 1 will now have 100HP
  * Dragonlord form 2 will be random between 150-165HP (Always the same on the same seed)
* Fixed some issues with pattern randomization
* Fixed stopspell resistance
  * Enemies with newly obtained magic now have lower stopspell resistance



# 0.9.1 - Bugfix for initial beta #
###### Pre-release - Released Dec 2, 2015 ######

* Added fix for fighter's ring. It now adds +2 to attack. Special thanks to Zombero for supplying the ips patch for this.
* Tweaks to cave shuffle to mitigate a potential unwinnable situation.
* Lowered Dragonlord's HP to make him beatable at level 18.
* Tweaks to enemy zone randomization.
* Tweaks to town shuffle.
* Tweaks to enemy patterns.
* Fixed rom version detection to work on overdump roms.
* Fixed a bug in chest randomization where large gold chests were not as abundant as intended.
* Some output filename tweaks. This should be more readable on everdrive.
* Added more info to the README



# 0.9.0 - Initial Beta release. #
###### Pre-release - Released Dec 1, 2015 ######

* Initial Beta
