#!/usr/bin/env python3

import argparse
import random
import sys
import hashlib

warps_addr = (0xf3d8, 0xf50b)
zones_addr = (0xf55f, 0xf5c3)
chest_addr = (0x5ddd, 0x5e59)
mp_req_addr = (0x1d63, 0x1d6d)
enemy_stats_addr = (0x5e5b, 0x60db)
prg0sums = ['6a50ce57097332393e0e8751924fd56456ef083c']
prg1sums = ['1ecc63aaac50a9612eaa8b69143858c3e48dd0ae']

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
    print("Verifying checksums...")
    result = verify_checksum(rom_data)
    if result is False:
      print("Checksum does not match any known ROM, aborting.")
      print("Rerun with -f to force randomization.")
      sys.exit(-1)
    else:
      print("Processing Dragon Warrior PRG%d ROM..." % result)
      prg = "PRG%d" % result
      
  else:
    print("Skipping checksums...")

  enemy_stats = rom_data[slice(*enemy_stats_addr)]
  warp_data = rom_data[slice(*warps_addr)]
  chest_data = rom_data[slice(*chest_addr)]
  enemy_zones = rom_data[slice(*zones_addr)]
  mp_reqs = rom_data[slice(*mp_req_addr)]

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

  outputfilename = "Dragon Warrior.%d.%s.%s.nes" % (args.seed, flats, prg)
  print("Writing output file %s..." % outputfilename)
  outputfile = open(outputfilename, 'wb')
  outputfile.write(rom_data)

def verify_checksum(rom_data):
  digest = hashlib.sha1(rom_data).hexdigest()
  if digest in prg0sum:
    return 0
  elif digest in prg1sum:
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
  for item in (13, 15, 16):
    if (item in charlock_chests):
      for i in (11, 12, 13, 14, 15, 16, 24):
        if contents[i] == item:
          # this could probably be a lot cleaner...
          j = random.randint(0, 23)
          # avoid 11-16 and chest 24 (they are in charlock)
          j = j + 6 if (j > 10) else j
          j = j + 1 if (j > 23) else j
          while contents[j] in (13, 15, 16):
            j = random.randint(0, 23)
            # avoid 11-16 and chest 24 (they are in charlock)
            j = j + 6 if (j > 10) else j
            j = j + 1 if (j > 23) else j
          contents[j], contents[i] = contents[i], contents[j]

  # make sure there's a key to get out of the throne room
  if not (3 in contents[4:7]):
    for i in range(len(contents)):
      if contents[i] == 3:
        j = random.randint(4, 6)
        contents[j], contents[i] = contents[i], contents[j]
        break
  
  chest_data[3::4] = bytearray(contents)
  return chest_data

def randomize_attack_patterns(enemy_stats):
  """
  Randomizes attack patterns of enemies (whether or not they use spells/fire 
  breath).

  :Parameters:
    enemy_stats : bytearray

  rtype: bytearray
  return: The new randomized enemy stats.
  """
  new_patterns = list()
  for i in range(38):
    if random.randint(0,1): # 50/50 chance
      # we'll start simple. No healmore/hurtmore/fire breath unless the enemy 
      # index > 20
      if i <= 20:
        # heal, sleep, stopspell, hurt
        new_patterns.append((random.randint(0,11) << 4) | random.randint(0,3))
      elif i < 30:
        # healmore, heal, sleep, stopspell, fire breath, hurtmore
        new_patterns.append((random.randint(0,15) << 4) | random.randint(4,11))
      else:
        # healmore, sleep, stopspell, strong fire breath, fire breath, hurtmore
        new_patterns.append((random.randint(4,15) << 4) | random.randint(4,15))
    else:
      new_patterns.append(0)
  new_patterns.append(87) #Dragonlord form 1
  new_patterns.append(14) #Dragonlord form 2
  enemy_stats[3::16] = bytearray(new_patterns)
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
  # if rimuldar is in the normal spot...
	if (towns[3][0] == 11):
		#make sure the rimuldar cave doesn't end up in garinham or tantegel
		while (24 in (caves[3][0], caves[5][0], caves[6][0]): 
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
  # zones 0-13
  new_zones = list()
  for i in range(14):
    for j in range(5):
      new_zones.append(random.randint(i * 2, (max(2,round(i*2.5)))))

  i = 9 #zone 14
  for j in range(5):
    new_zones.append(random.randint(18, 23))

  # zone 15-18
  for i in range(15, 19):
    for j in range(5):
      new_zones.append(random.randint(14 + i, 37))

  # zone 19
  for j in range(5):
    new_zones.append(random.randint(4, 11))
  return bytearray(new_zones)

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
  enemy_stats[2::16] = bytearray(remake_hp)
  return enemy_stats

def main():
  parser = argparse.ArgumentParser(prog="DWRandomizer")
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
  parser.add_argument("-p","--patterns", action="store_false", 
      help="Do not randomize enemy attack patterns.")
  parser.add_argument("-s","--seed", type=int, 
      help="Specify a seed to be used for randomization.")
  parser.add_argument("-t","--towns", action="store_false", 
      help="Do not randomize towns.")
  parser.add_argument("filename", help="The rom file to use for input")
  args = parser.parse_args()
  randomize(args)

remake_mp = (3, 2, 2, 2, 2, 6, 8, 2, 8, 5)
remake_hp = (  2,  3,  5,  7, 12, 13, 13, 22, 26, 43, 16, 24, 28, 
              18, 33, 39,  3, 48, 37, 35, 44, 37, 40, 40,153,110, 
              47, 48, 38, 70, 72, 74, 65, 67, 98,135, 99,106,204,255)
remake_xp = (  1,  2,  3,  4,  8, 12, 16, 14, 15, 18, 12, 25, 28, 
              31, 40, 42,255, 47, 52, 58, 58, 64, 70, 72,255,  6, 
              78, 83, 90,120,135,105,120,130,180,155,172,255,  0,  0)
# These are +1 for proper values in the ROM
remake_gold=(  3,  5,  7,  9, 17, 21, 26, 22, 20, 31, 21, 43, 51, 
              49, 61, 63,  7, 76, 81, 96,111,106,111,121, 11,255,
             151,136,149,186,161,170,186,166,151,149,153,144,  0,  0)

if __name__ == "__main__":
  main()

