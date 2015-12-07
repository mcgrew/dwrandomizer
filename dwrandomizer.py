#!/usr/bin/env python3

import argparse
import random
import sys
import hashlib

VERSION = "0.9.2-2015.12.07"
#Addresses for various data. Offsets include iNES header.
warps_addr = (0xf3d8, 0xf50b)
zones_addr = (0xf55f, 0xf5c3)
chest_addr = (0x5ddd, 0x5e59)
mp_req_addr = (0x1d63, 0x1d6d)
enemy_stats_addr = (0x5e5b, 0x60db)
player_stats_addr = (0x60dd, 0x6190)
weapon_shop_inv_addr = (0x19a1, 0x19cc)

#sha1sums of various roms
prg0sums = ['6a50ce57097332393e0e8751924fd56456ef083c', #Dragon Warrior (U) (PRG0) [!].nes
            '66330df6fe3e3c85adb8183721e5f88c149e52eb', #Dragon Warrior (U) (PRG0) [b1].nes
            '49974889619f1d8c39b6c20fa208c62a0a73ecce', #Dragon Warrior (U) (PRG0) [b1][o1].nes
            'd98b8a3fc93bb2f1f5016326556b68998dd5f85d', #Dragon Warrior (U) (PRG0) [b2].nes
            'e81a693efe322be9584c97b55c6d7ae38ae44a66', #Dragon Warrior (U) (PRG0) [o1].nes
            '6e1a52b7b3a13494536bbab7248690861665001a', #Dragon Warrior (U) (PRG0) [o2].nes
            '3077d5bd5c5c3744398b122d5ee1bba7055c8d45'] #Dragon Warrior (U) (PRG0) [o3].nes
prg1sums = ['1ecc63aaac50a9612eaa8b69143858c3e48dd0ae'] #Dragon Warrior (U) (PRG1) [!].nes

def randomize(args):

  if not args.seed:
    args.seed = random.randint(0, sys.maxsize)
  print("Randomizing %s using random seed %d..." % (args.filename, args.seed))
  random.seed(args.seed)
  flags = ""
  prg = "PRG_"

  input_file = open(args.filename, 'rb')
  rom_data = bytearray(input_file.read())

  if args.force:
    print("Verifying checksum...")
    result = verify_checksum(rom_data)
    if result is False:
      print("Checksum does not match any known ROM, aborting.")
      print("Rerun with -f to force randomization.")
      sys.exit(-1)
    else:
      print("Processing Dragon Warrior PRG%d ROM..." % result)
      prg = "PRG%d" % result
      
  else:
    print("Skipping checksum...")

  print("Fixing functionality of the fighter's ring (+2 atk)...")
  rom_data = fix_fighters_ring(rom_data)
  print("Buffing HEAL slightly...")
  rom_data[0xdbce] = 15

  enemy_stats = rom_data[slice(*enemy_stats_addr)]
  warp_data = rom_data[slice(*warps_addr)]
  chest_data = rom_data[slice(*chest_addr)]
  enemy_zones = rom_data[slice(*zones_addr)]
  mp_reqs = rom_data[slice(*mp_req_addr)]
  weapon_shop_inv = rom_data[slice(*weapon_shop_inv_addr)]
  player_stats = rom_data[slice(*player_stats_addr)]

  if args.chests:
    print("Shuffling chest contents...")
    flags += "c"
    chest_data = shuffle_chests(chest_data)
    rom_data[slice(*chest_addr)] = chest_data

  if args.towns:
    print("Shuffling town locations...")
    flags += "t"
    warp_data = shuffle_towns(warp_data)
    rom_data[slice(*warps_addr)] = warp_data

  if args.enemies:
    print("Randomizing enemy zones...")
    flags += "e"
    enemy_zones = randomize_zones(enemy_zones)
    rom_data[slice(*zones_addr)] = enemy_zones

  if args.patterns:
    print("Randomizing enemy attack patterns...")
    flags += "p"
    enemy_stats = randomize_attack_patterns(enemy_stats)
    rom_data[slice(*enemy_stats_addr)] = enemy_stats

  if args.shops:
    print("Randomizing weapon shops...")
    flags += "w"
    weapon_shop_inv = randomize_shops(weapon_shop_inv)
    rom_data[slice(*weapon_shop_inv_addr)] = weapon_shop_inv

  if args.remake:
    print("Increasing XP/Gold drops to remake levels...")
    flags += "r"
    enemy_stats = update_drops(enemy_stats)
    rom_data[slice(*enemy_stats_addr)] = enemy_stats
    print("Setting enemy HP to approximate remake levels...")
    enemy_stats = update_enemy_hp(enemy_stats)
    rom_data[slice(*enemy_stats_addr)] = enemy_stats
    print("Lowering MP requirements to remake levels...")
    flags += "m"
    mp_reqs = update_mp_reqs(mp_reqs)
    rom_data[slice(*mp_req_addr)] = mp_reqs

  if args.repel:
    print("Moving REPEL to level 8...")
    flags += "w"
    player_stats = move_repel(player_stats)
    rom_data[slice(*player_stats_addr)] = player_stats

  outputfilename = "DWRandomizer.%d.%s.%s.nes" % (args.seed, flags, prg)
  print("Writing output file %s..." % outputfilename)
  outputfile = open(outputfilename, 'wb')
  outputfile.write(rom_data)
  digest = hashlib.sha1(rom_data).hexdigest()
  print ("New ROM Checksum: %s" % digest)

def verify_checksum(rom_data):
  digest = hashlib.sha1(rom_data).hexdigest()
  if digest in prg0sums:
    return 0
  elif digest in prg1sums:
    return 1
  return False

def shuffle_chests(chest_data):
  """
  Shuffles the contents of all chests in the game. Checks are used to ensure no
  quest items are located in Charlock chests.

  :Parameters:

  rtype: 
  return: 
  """
  contents = [int(x) for x in chest_data[3::4]]
  for i in range(len(contents)):
    # change all gold (and erdrick's tablet) to large gold stash
    if (contents[i] >= 18 and contents[i] <= 20):
      contents[i] = 21
    # 50/50 chance to have a chest with gwaelin's love (lol)
    if contents[i] >= 23:
      contents[i] = 11 + (random.randint(0,1) * 10)

  random.shuffle(contents)

  # make sure required quest items aren't in Charlock
  charlock_chests = contents[11:17] + [contents[24]]
  charlock_chest_indices = (11, 12, 13, 14, 15, 16, 24)
  for item in (13, 15, 16):
    if (item in charlock_chests):
      for i in charlock_chest_indices:
        if contents[i] == item:
          # this could probably be cleaner...
          j = non_charlock_chest()
          while contents[j] in (13, 15, 16):
            j = non_charlock_chest()
          contents[j], contents[i] = contents[i], contents[j]

  # make sure staff of rain guy doesn't have the stones (potentially unwinnable)
  if (contents[19] == 15):
    j = non_charlock_chest()
    #                  don't remove the key from the throne room
    while (j == 19 or (j in (4, 5, 6) and contents[j] == 3)):
      j = non_charlock_chest()
    contents[19],contents[j] = contents[j],contents[19]

  # make sure there's a key to get out of the throne room
  if not (3 in contents[4:7]):
    for i in range(len(contents)):
      if contents[i] == 3:
        j = random.randint(4, 6)
        # if key is in charlock and chest[j] contains a quest item, try again
        while (i in charlock_chest_indices and (contents[j] in (13, 15, 16))):
          j = random.randint(4, 6)
        contents[j], contents[i] = contents[i], contents[j]
        break
  
  chest_data[3::4] = bytearray(contents)
  return chest_data

def non_charlock_chest():
  """
  Gives a random index of a chest that is not in Charlock

  rtype: int
  return: A chest index that is not in Charlock.
  """
  chest = random.randint(0, 23)
  # avoid 11-16 and chest 24 (they are in charlock)
  chest = chest + 6 if (chest > 10) else chest
  chest = chest + 1 if (chest > 23) else chest
  return chest

def randomize_attack_patterns(enemy_stats):
  """
  Randomizes attack patterns of enemies (whether or not they use spells/fire 
  breath).

  :Parameters:
    enemy_stats : bytearray

  rtype: bytearray
  return: The new randomized enemy stats.
  """
  new_patterns = list() # attack patterns
  new_ss_resist = enemy_stats[4::16] #stopspell resistance
  for i in range(38):
    new_ss_resist[i] |= 0xf
    if random.randint(0,1): # 50/50 chance
      resist = random.randint(0,round(i/5))
      new_ss_resist[i] &= (0xf0 | resist)
      if i <= 20:
        # heal, sleep, stopspell, hurt
        new_patterns.append((random.randint(0,11) << 4) | random.randint(0,3))
      elif i < 30:
        # healmore, heal, sleep, stopspell, fire breath, hurtmore
        new_patterns.append((random.randint(0,15) << 4) | random.randint(4, 11))
      else:
        # healmore, sleep, stopspell, strong fire breath, fire breath, hurtmore
        #we'll be nice and not give Axe Knight Dragonlord's breath.
        slot2 = random.randint(4, 11) if i == 33 else random.randint(4, 15)
        new_patterns.append((random.choice((0, 1, 3)) << 6) | 
            (random.randint(0, 3) << 4) | slot2)
    else:
      new_patterns.append(0)
  new_patterns.append(87) #Dragonlord form 1
  new_patterns.append(14) #Dragonlord form 2
  enemy_stats[3::16] = bytearray(new_patterns)
  enemy_stats[4::16] = bytearray(new_ss_resist)
  return enemy_stats

def shuffle_towns(warp_data):
  """
  Shuffles the locations of towns on the map.

  :Parameters:
    warp_data : bytearray
      The warp data read from the ROM.

  rtype: bytearray
  return: The shuffled warp_data
  """
  towns = [warp_data[153:156], warp_data[159:162], warp_data[162:165],
           warp_data[180:183], warp_data[183:186], warp_data[186:189]]
  caves = [warp_data[156:159], warp_data[168:171], warp_data[177:180],
           warp_data[189:192], warp_data[192:195], warp_data[204:207],
           warp_data[210:213]]
  random.shuffle(towns)
  random.shuffle(caves)
  # make sure the rimuldar cave isn't inaccessible
  # this could happen if it ends up in southern shrine position or
  # behind a locked door when rimuldar is in it's normal location.
  while (caves[3][0] == 21 or
          (towns[3][0] == 11 and 21 in (caves[5][0], caves[6][0]))):
    random.shuffle(caves)
  warp_data[153:156] = towns[0]
  warp_data[159:162] = towns[1]
  warp_data[162:165] = towns[2]
  warp_data[180:183] = towns[3]
  warp_data[183:186] = towns[4]
  warp_data[186:189] = towns[5]

  warp_data[156:159] = caves[0]
  warp_data[168:171] = caves[1]
  warp_data[177:180] = caves[2]
  warp_data[189:192] = caves[3]
  warp_data[192:195] = caves[4]
  warp_data[204:207] = caves[5]
  warp_data[210:213] = caves[6]
  # randomly swap swamp cave exit with southern shrine
  if (random.randint(0,1)):
		warp_data[174:177],warp_data[189:192] = warp_data[189:192],warp_data[174:177]
  return warp_data

def randomize_zones(zone_info):
  """
  Randomizes which enemies are present in each zone.

  :Parameters:
    zone_info : bytearray
      The enemy zone info from the ROM.

  rtype: bytearray
  return: The newly randomized zone info
  """
  new_zones = list()
  #zone 0
  for j in range(5): 
    new_zones.append(int(random.randint(0, 4)/2))

  # zones 1-13 (overworld)
  for i in range(1, 14):
    for j in range(5):
      enemy = random.randint(max(0, i * 2 - 2), (max(2,round(i*2.5))))
      while enemy == 24: # don't add golem
        enemy = random.randint(i * 2 - 2, round(i*2.5))
      new_zones.append(enemy)

  #zone 14 - garin's grave?
  for j in range(5): 
    new_zones.append(random.randint(7, 17))

  #zone 15 - lower garin's grave
  for j in range(5): 
    new_zones.append(random.randint(15, 23))

  # zone 16-18 - Charlock
  for i in range(16, 19):
    for j in range(5):
      new_zones.append(random.randint(13+i, 37))

  # zone 19 rimuldar tunnel
  for j in range(5):
    new_zones.append(random.randint(3, 11))
  return bytearray(new_zones)

def randomize_shops(weapon_shop_inv):
  """
  Randomizes the items available in each weapon shop
  :Parameters:
    weapon_shop_inv : bytearray
      The weapon shop data from the ROM

  rtype: bytearray
  return: The new weapon shop inventory
  """
  weapons = (0, 1, 2, 3, 4, 5, 7, 8, 9, 10, 11, 12, 14, 15, 16)
  new_shop_inv = []
  # add 5 items to each shop
  for i in range(7):
    this_shop = []
    while(len(this_shop) < 5):
      new_item = random.choice(weapons)
      if not new_item in this_shop:
        this_shop.append(new_item)
    new_shop_inv.append(this_shop)

  # add an extra item to one shop since we have 36 slots.
  six_item_shop = random.randint(0, 6)
  new_item = random.choice(weapons)
  while new_item in new_shop_inv[six_item_shop]:
    new_item = random.choice(weapons)
  new_shop_inv[six_item_shop].append(new_item)
  
  # create the bytearray to insert into the rom. Shops are separated by an 
  # 0xfd byte
  returnvalue = []
  for shop in new_shop_inv:
    shop.sort()
    returnvalue += shop + [0xfd]

  return bytearray(returnvalue)

def update_drops(enemy_stats):
  """
  Raises enemy XP and gold drops to those of the remakes.

  :Parameters:
    enemy_stats : bytearray
      The enemy stats read from the ROM

  rtype: bytearray
  return: The new enemy_stats after buffing drops.
  """
  enemy_stats[6::16] = bytearray(remake_xp)
  enemy_stats[7::16] = bytearray(remake_gold)
  return enemy_stats

def update_mp_reqs(mp_data):
  """
  Lowers the MP requirements of spells to that of the remake

  :Parameters:
    mp_data : bytearray
      The MP data read from the ROM

  rtype: bytearray
  return: The new MP requirement data.
  """
  return bytearray(remake_mp)

def update_enemy_hp(enemy_stats):
  """
  Updates HP of enemies to that of the remake, where possible.

  :Parameters:
    mp_data : bytearray
      The enemy stats read from the ROM

  rtype: bytearray
  return: The new enemy stats.
  """
  # randomize Dragonlord's HP somewhat 
  remake_hp[-1] -= random.randint(0, 15) # 150 - 165
  enemy_stats[2::16] = bytearray(remake_hp)
  return enemy_stats

def fix_fighters_ring(rom_data):
  """
  Adds functionality for the fighter's ring (+2 to attack)
  :Parameters:
    rom_data : bytearray
      The entire ROM data

  rtype: bytearray
  return: The patched ROM data
  """
  rom_data[0xf10c:0xf110] = bytearray(ring_patch[0])
  rom_data[0xff64:0xff76] = bytearray(ring_patch[1])
  return rom_data

def move_repel(player_stats):
  """
  Moves the repel spell to level 8

  :Parameters:
    player_stats : bytearray
      The player statistics

  rtype: bytearray
  return: The player stats with repel moved.
  """
  for i in range(47, 90, 6):
    player_stats[i] |= 0x80
  return player_stats

def main():
  
  parser = argparse.ArgumentParser(prog="DWRandomizer",
      description="A randomizer for Dragon Warrior for NES")
  parser.add_argument("-r","--remake", action="store_false",
      help="Do not set enemy HP, XP/Gold drops and MP use up to that of the "
           "remake. This will make grind times much longer.")
  parser.add_argument("-c","--chests", action="store_false",
      help="Do not randomize chest contents.")
  parser.add_argument("-f","--force", action="store_false", 
      help="Skip checksums and force randomization. This may produce an invalid"
           " ROM if the incorrect file is used.")
  parser.add_argument("-e","--enemies", action="store_false", 
      help="Do not randomize enemy zones.")
  parser.add_argument("-l","--repel", action="store_false", 
      help="Do not move repel to level 8.")
  parser.add_argument("-p","--patterns", action="store_false", 
      help="Do not randomize enemy attack patterns.")
  parser.add_argument("-s","--seed", type=int, 
      help="Specify a seed to be used for randomization.")
  parser.add_argument("-t","--towns", action="store_false", 
      help="Do not randomize towns.")
  parser.add_argument("-w","--shops", action="store_false", 
      help="Do not randomize weapon shops.")
#  parser.add_argument('--version', action='version', version='%(prog) %s'%VERSION)
  parser.add_argument("filename", help="The rom file to use for input")
  args = parser.parse_args()
  randomize(args)

remake_mp = [3, 2, 2, 2, 2, 6, 8, 2, 8, 5]
#dragonlord hp should be 204 and 350, but we want him beatable at lv 18
remake_hp = [  2,  3,  5,  7, 12, 13, 13, 22, 26, 43, 16, 24, 28, 
              18, 33, 39,  3, 48, 37, 35, 44, 37, 40, 40,153,110, 
              47, 48, 38, 70, 72, 74, 65, 67, 98,135, 99,106,100,165]
remake_xp = [  1,  2,  3,  4,  8, 12, 16, 14, 15, 18, 12, 25, 28, 
              31, 40, 42,255, 47, 52, 58, 58, 64, 70, 72,255,  6, 
              78, 83, 90,120,135,105,120,130,180,155,172,255,  0,  0]
# These are +1 for proper values in the ROM
remake_gold=[  3,  5,  7,  9, 17, 21, 26, 22, 20, 31, 21, 43, 51, 
              49, 61, 63,  7, 76, 81, 96,111,106,111,121, 11,255,
             151,136,149,186,161,170,186,166,151,149,153,144,  0,  0]

# this data is to patch in funcionality for the fighter's ring. +2 atk.
ring_patch = ((0x20, 0x54, 0xff, 0xea), #rom address 0xf10c 
              (0x85, 0xcd, 0xa5, 0xcf, 0x29, 0x20, 0xf0, 0x07, 0xa5, 
               0xcc, 0x18, 0x69, 0x02, 0x85, 0xcc, 0xa5, 0xcf, 0x60)) #rom address 0xff64

if __name__ == "__main__":
  main()

