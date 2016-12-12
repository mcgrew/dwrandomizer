# 1.3-rc1 - Lots of balance changes and improvements #
###### Pre-release - Released Dec 11, 2016 ######

 * Graphical interface! DWRandomizer now has a GUI for a better user experience.
 * There is now a Windows installer for DWRandomizer - it will now be added to the start menu.
 * The ROM output directory can now be specified in the GUI or from the command line via the `--output-dir` option
 * Fixed the Cantlin NPC dialog when the overworld item is more than 100 steps from the castle.
 * Added back the silver harp.
 > * The NPC in the Northern Shrine will now always have the Staff of Rain.
 > * Enemies summoned by the harp will now be a random set of 8 different enemies.
 * Buffed some of the weaker spells.
 > * `HEAL` now has a range of 18-25 HP.
 > * `HURT` now has a range of 9-16 HP.
 * Menu wrap-around.
 > * Various menus in the game will now have the cursor wrap from top to bottom *(optional)*.
 * The Death Necklace will now always appear in it's chest the first time. You can still only get one.
 * Death Necklace now has an *optional* function other than cursing you. It can give a +10 ATK at the expense of 25% of your HP (you are still cursed).
 * Some enemy balance changes:
 > * Reduced Droll HP from 35 to 20
 > * Reduced Poltergeist HP from 26 to 23
 > * Reduced Specter HP from 48 to 33
 > * Reduced Goldman HP from 110 to 35
 * Some changes to zone randomization:
 > * Zones 1 & 2 will now have Wolf and below only.
 > * Tantegel will have 2 of each of these zones surrounding it
 >> * Zone 1 will be to the east and west
 >> * Zone 2 will be to the north and south
 > * All other zones will be random (using a new algorithm)
 >> * The new algorithm ensures each zone will appear on the overworld no more than 5 times.



# 1.2.1 - Some bug fixes #
###### Released Aug 6, 2016 ######

 * Fixed an issue where Garin's Grave and Tantegel Basement weren't randomized
 * Moved "speed hacks" option to a top level question.



# 1.2 - Speed hacks & maps are now more random! # 
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



# 1.0-rc1 - 1.0 Release Candidate 1! Randomly Generated Map & More! # 
###### Pre-release - Released Jan 12, 2016 ######

Generated Overworld!

 * A new overworld map can be generated with each seed. This is enabled by default
 * The enemy for forced encounters are now randomized in ultra mode. Also, on generated maps, the golem encounter is moved into Charlock's Throne Room (kind of a miniboss guarding Dragonlord).

 No more command line needed!

 * When the randomizer is run without options (or drag & drop), you will be prompted for options, removing the need to run from the command line to customize options.

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
 * REPEL will now be learned at level 8.
 * HEAL will now have a range of 10-25 instead of 10-17.
 * Shortened the output filename for better readability on Everdrive.
 * Removed the possibility of Axe Knight having strong fire breath
 * Several tweaks to avoid a few potentially unwinnable situations.
 * Dragonlord HP tweaks:
 > * Dragonlord form 1 will now have 100HP
 > * Dragonlord form 2 will be random between 150-165HP (Always the same on the same seed)
 * Fixed some issues with pattern randomization
 * Fixed stopspell resistance
 > * Enemies with newly obtained magic now have lower stopspell resistance



# 0.9.1 - - Bugfix for initial beta #
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
