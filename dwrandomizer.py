#!/usr/bin/env python3

import argparse
import random
import sys
import hashlib

VERSION = "0.9.4"
#sha1sums of various roms
prg0sums = ['6a50ce57097332393e0e8751924fd56456ef083c', #Dragon Warrior (U) (PRG0) [!].nes
            '66330df6fe3e3c85adb8183721e5f88c149e52eb', #Dragon Warrior (U) (PRG0) [b1].nes
            '49974889619f1d8c39b6c20fa208c62a0a73ecce', #Dragon Warrior (U) (PRG0) [b1][o1].nes
            'd98b8a3fc93bb2f1f5016326556b68998dd5f85d', #Dragon Warrior (U) (PRG0) [b2].nes
            'e81a693efe322be9584c97b55c6d7ae38ae44a66', #Dragon Warrior (U) (PRG0) [o1].nes
            '6e1a52b7b3a13494536bbab7248690861665001a', #Dragon Warrior (U) (PRG0) [o2].nes
            '3077d5bd5c5c3744398b122d5ee1bba7055c8d45'] #Dragon Warrior (U) (PRG0) [o3].nes
prg1sums = ['1ecc63aaac50a9612eaa8b69143858c3e48dd0ae'] #Dragon Warrior (U) (PRG1) [!].nes

class Rom:
  # Slices for various data. Offsets include iNES header.
  chest_content_slice = slice(0x5de0, 0x5e59, 4)
  enemy_patterns_slice = slice(0x5e5e, 0x60db, 16)
  enemy_ss_resist_slice = slice(0x5e5f, 0x60db, 16)
  enemy_stats_slice = slice(0x5e5b, 0x60db)
  warps_slice = slice(0xf3d8, 0xf50b)
  zones_slice = slice(0xf55f, 0xf5c3)
  mp_req_slice = slice(0x1d63, 0x1d6d) #mp requirements for each spell
  player_stats_slice = slice(0x60dd, 0x6191)
  weapon_shop_inv_slice = slice(0x19a1, 0x19cc)

  token_slice = slice(0xe11e, 0xe12b, 6)
  flute_slice = slice(0xe15d, 0xe16a, 6)
  armor_slice = slice(0xe173, 0xe180, 6)

  def __init__(self, filename):
    input_file = open(filename, 'rb')
    self.rom_data = bytearray(input_file.read())
    input_file.close()

  def sha1(self):
    """
    Returns a sha1 checksum of the rom data
    """
    return hashlib.sha1(self.rom_data).hexdigest()

  def verify_checksum(self):
    digest = self.sha1()
    if digest in prg0sums:
      return 0
    elif digest in prg1sums:
      return 1
    return False

  def shuffle_chests(self):
    """
    Shuffles the contents of all chests in the game. Checks are used to ensure no
    quest items are located in Charlock chests.
    """
    contents = [int(x) for x in self.rom_data[self.chest_content_slice]]
    for i in range(len(contents)):
      # change all gold (and erdrick's tablet) to large gold stash
      if (contents[i] >= 18 and contents[i] <= 20):
        contents[i] = 21
      # 50/50 chance to have erdrick's token in a chest
      if contents[i] == 23:
        if random.randint(0,1):
          self.rom_data[self.token_slice.start] = 0 # remove token from swamp
          contents[i] = 10 # put it in a chest
        else:
          contents[i] = 21

    random.shuffle(contents)

    # make sure required quest items aren't in Charlock
    charlock_chests = contents[11:17] + [contents[24]]
    charlock_chest_indices = (11, 12, 13, 14, 15, 16, 24)
    quest_items = (10, 13, 15, 16)
    for item in quest_items:
      if (item in charlock_chests):
        for i in charlock_chest_indices:
          if contents[i] == item:
            # this could probably be cleaner...
            j = self.non_charlock_chest()
            while contents[j] in quest_items:
              j = self.non_charlock_chest()
            contents[j], contents[i] = contents[i], contents[j]

    # make sure there's a key to in the throne room
    if not (3 in contents[4:7]):
      for i in range(len(contents)):
        if contents[i] == 3:
          j = random.randint(4, 6)
          # if key is in charlock and chest[j] contains a quest item, try again
          while (i in charlock_chest_indices and (contents[j] in quest_items)):
            j = random.randint(4, 6)
          contents[j], contents[i] = contents[i], contents[j]
          break

        # make sure staff of rain guy doesn't have the stones (potentially unwinnable)
#      if (contents[19] == 15):
#        j = self.non_charlock_chest()
#        #                  don't remove the key from the throne room
#        while (j == 19 or (j in (4, 5, 6) and contents[j] == 3)):
#          j = self.non_charlock_chest()
#        contents[19],contents[j] = contents[j],contents[19]

      self.rom_data[self.chest_content_slice] = bytearray(contents)

  def non_charlock_chest(self):
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

  def randomize_attack_patterns(self):
    """
    Randomizes attack patterns of enemies (whether or not they use spells/fire 
    breath).
    """
    new_patterns = list() # attack patterns
    new_ss_resist = self.rom_data[self.enemy_ss_resist_slice] 
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
    self.rom_data[self.enemy_patterns_slice] = bytearray(new_patterns)
    self.rom_data[self.enemy_ss_resist_slice] = bytearray(new_ss_resist)

  def shuffle_towns(self):
    """
    Shuffles the locations of towns on the map.
    """
    warp_data = self.rom_data[self.warps_slice]
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
    self.rom_data[self.warps_slice] = warp_data

  def randomize_zones(self):
    """
    Randomizes which enemies are present in each zone.
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
    self.rom_data[self.zones_slice] = new_zones

  def randomize_shops(self):
    """
    Randomizes the items available in each weapon shop
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
    new_shop_bytes = []
    for shop in new_shop_inv:
      shop.sort()
      new_shop_bytes += shop + [0xfd]

    self.rom_data[self.weapon_shop_inv_slice] = new_shop_bytes

  def shuffle_searchables(self):
    """
    Shuffles the 3 searchable items in the game. (E.Armor, F.Flute, E.Token)
    """
    searchables = [self.rom_data[self.token_slice],
                   self.rom_data[self.flute_slice],
                   self.rom_data[self.armor_slice]]
    random.shuffle(searchables)
    self.rom_data[self.token_slice] = searchables[0]
    self.rom_data[self.flute_slice] = searchables[1]
    self.rom_data[self.armor_slice] = searchables[2]

  def randomize_growth(self):
    player_stat_data = self.rom_data[self.player_stats_slice]
    
    player_str = list(player_stat_data[0:180:6])
    player_agi = list(player_stat_data[1:180:6])
    player_hp  = list(player_stat_data[2:180:6])
    player_mp  = list(player_stat_data[3:180:6])

    for i in range(len(player_str)):
      player_str[i] = round(player_str[i] * random.uniform(0.8, 1.2))
    for i in range(len(player_agi)):
      player_agi[i] = round(player_agi[i] * random.uniform(0.8, 1.2))
    for i in range(len(player_hp)):
      player_hp[i] = round(player_hp[i] * random.uniform(0.8, 1.2))
    for i in range(len(player_mp)):
      player_mp[i] = round(player_mp[i] * random.uniform(0.8, 1.2))

    player_str.sort()
    player_agi.sort()
    player_hp.sort()
    player_mp.sort()

    player_stat_data[0:180:6] = player_str
    player_stat_data[1:180:6] = player_agi
    player_stat_data[2:180:6] = player_hp
    player_stat_data[3:180:6] = player_mp
    self.rom_data[self.player_stats_slice] = player_stat_data 

  def update_drops(self):
    """
    Raises enemy XP and gold drops to those of the remakes.
    """
    remake_xp = [  1,  2,  3,  4,  8, 12, 16, 14, 15, 18, 12, 25, 28, 
                  31, 40, 42,255, 47, 52, 58, 58, 64, 70, 72,255,  6, 
                  78, 83, 90,120,135,105,120,130,180,155,172,255,  0,  0]
    # These are +1 for proper values in the ROM
    remake_gold=[  3,  5,  7,  9, 17, 21, 26, 22, 20, 31, 21, 43, 51, 
                  49, 61, 63,  7, 76, 81, 96,111,106,111,121, 11,255,
                 151,136,149,186,161,170,186,166,151,149,153,144,  0,  0]
    enemy_stats = self.rom_data[self.enemy_stats_slice]
    enemy_stats[6::16] = bytearray(remake_xp)
    enemy_stats[7::16] = bytearray(remake_gold)
    self.rom_data[self.enemy_stats_slice] = enemy_stats 

  def update_mp_reqs(self):
    """
    Lowers the MP requirements of spells to that of the remake
    """
    #magic required for each spell, in order
    remake_mp = [3, 2, 2, 2, 2, 6, 8, 2, 8, 5]
    self.rom_data[self.mp_req_slice] = remake_mp

  def update_enemy_hp(self):
    """
    Updates HP of enemies to that of the remake, where possible.
    """
    #dragonlord hp should be 204 and 350, but we want him beatable at lv 18
    enemy_stats = self.rom_data[self.enemy_stats_slice]
    remake_hp = [  2,  3,  5,  7, 12, 13, 13, 22, 26, 43, 16, 24, 28, 
                  18, 33, 39,  3, 48, 37, 35, 44, 37, 40, 40,153,110, 
                  47, 48, 38, 70, 72, 74, 65, 67, 98,135, 99,106,100,165]
    # randomize Dragonlord's second form HP somewhat 
    remake_hp[-1] -= random.randint(0, 15) # 150 - 165
    enemy_stats[2::16] = bytearray(remake_hp)
    self.rom_data[self.enemy_stats_slice] = enemy_stats 

  def fix_fighters_ring(self):
    """
    Adds functionality for the fighter's ring (+2 to attack)
    """
    ring_patch1 = (0x20, 0x54, 0xff, 0xea)
    ring_patch2 = (0x85, 0xcd, 0xa5, 0xcf, 0x29, 0x20, 0xf0, 0x07, 0xa5, 
                   0xcc, 0x18, 0x69, 0x02, 0x85, 0xcc, 0xa5, 0xcf, 0x60)
    self.rom_data[0xf10c:0xf110] = bytearray(ring_patch1)
    self.rom_data[0xff64:0xff76] = bytearray(ring_patch2)

  def move_repel(self):
    """
    Moves the repel spell to level 8
    """
    player_stats = self.rom_data[self.player_stats_slice]
    for i in range(47, 90, 6):
      player_stats[i] |= 0x80
    self.rom_data[self.player_stats_slice] = player_stats

  def buff_heal(self):
    self.rom_data[0xdbce] = 15

  def fix_northern_shrine( self):
    """
    Removes the 2 blocks from around the shrine guardian so you can walk around 
    him.
    """
    self.rom_data[0xd77] = self.rom_data[0xd81] = 0x66

  def write(self, output_filename):
    outputfile = open(output_filename, 'wb')
    outputfile.write(self.rom_data)
    outputfile.close()

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
  parser.add_argument("-i","--searchitems", action="store_false", 
      help="Do not randomize the locations of searchable items (Fairy Flute, "
           "Erdrick's Armor, Erdrick's Token).")
  parser.add_argument("-g","--growth", action="store_false", 
      help="Do not randomize player stat growth.")
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

def randomize(args):

  if not args.seed:
    args.seed = random.randint(0, sys.maxsize)
  print("Randomizing %s using random seed %d..." % (args.filename, args.seed))
  random.seed(args.seed)
  flags = ""
  prg = "PRG_"

  rom = Rom(args.filename)

  if args.force:
    print("Verifying checksum...")
    result = rom.verify_checksum()
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
  rom.fix_fighters_ring()

  print("Buffing HEAL slightly...")
  rom.buff_heal()

  print("Fixing Northern Shrine...")
  rom.fix_northern_shrine()

  if args.chests:
    print("Shuffling chest contents...")
    flags += "c"
    rom.shuffle_chests()

  if args.towns:
    print("Shuffling town locations...")
    flags += "t"
    rom.shuffle_towns()

  if args.enemies:
    print("Randomizing enemy zones...")
    flags += "e"
    rom.randomize_zones()

  if args.patterns:
    print("Randomizing enemy attack patterns...")
    flags += "p"
    rom.randomize_attack_patterns()

  if args.shops:
    print("Randomizing weapon shops...")
    flags += "w"
    rom.randomize_shops()

  if args.searchitems:
    print("Shuffling searchable item locations...")
    flags += "i"
    rom.shuffle_searchables()

  if args.growth:
    print("Randomizing player stat growth...")
    flags += "g"
    rom.randomize_growth()

  if args.remake:
    print("Increasing XP/Gold drops to remake levels...")
    flags += "r"
    rom.update_drops()
    print("Setting enemy HP to approximate remake levels...")
    rom.update_enemy_hp()
    print("Lowering MP requirements to remake levels...")
    flags += "m"
    rom.update_mp_reqs()

  if args.repel:
    print("Moving REPEL to level 8...")
    flags += "l"
    rom.move_repel()

  output_filename = "DWRandomizer.%d.%s.%s.nes" % (args.seed, flags, prg)
  print("Writing output file %s..." % output_filename)
  rom.write(output_filename)
  print ("New ROM Checksum: %s" % rom.sha1())

if __name__ == "__main__":
  main()

