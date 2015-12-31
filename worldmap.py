#!/bin/env python3

import random
import argparse
import pathfinding

########################################################
# Tiles:
#  0 - Grass
#  1 - Desert
#  2 - Hills
#  3 - Mountains
#  4 - Water
#  5 - Block
#  6 - Trees
#  7 - Swamp
#  8 - Town
#  9 - Cave
#  A - Castle
#  B - Bridge
#  C - Stairs Down
#  D - Unused?
#  E - Unused? 
#  F - Unused? 
#########################################################

GRASS   =0
DESERT  =1
HILL    =2
MOUNTAIN=3
WATER   =4
BLOCK   =5
TREES   =6
SWAMP   =7
TOWN    =8
CAVE    =9
CASTLE  =10
BRIDGE  =11
STAIRS  =12
IMPASSABLE = (WATER, MOUNTAIN, BLOCK)



class WorldMap:
  encoded_size = 0x8f6
  map_width = 120
  map_height = 120
  cave_warps = (1, 5, 8, 12, 13, 17, 19, 7)
  town_warps = (0, 2, 3, 9, 10, 11)
  tantegel_warp = 4
  charlock_warp = 6
  return_point_addr = (0xdb11, 0xdb15, 0xdb1d)
  axe_knight_slice = slice(0xcd64, 0xcd76, 6)
  green_dragon_slice = slice(0xcd7b, 0xcd8d, 6)
  golem_slice = slice(0xcd98, 0xcdaa, 6)
  tiles = ("grass", "desert", "hill", "mountain", "water", "block", 
           "trees", "swamp", "town", "cave",  "castle", "bridge", "stairs")

  def __init__(self): 
    self.grid = None
    self.rom_data = None
    self.warps_from = []
    self.warps_to   = []
    self.return_point = None
    self.generated = False
    self.return_point = None
    self.axe_knight = None
    self.green_dragon = None
    self.golem = None
    self.chests = None

  def generate(self):
    """
    Potential alternate implementation for map generation.

    rtype: array
    return: The newly generated map
    """
    self.grid = []
    for i in range(self.map_height):
      self.grid.append([WATER]*self.map_width)
    tiles = [GRASS, GRASS, SWAMP, DESERT, DESERT, HILL, HILL, MOUNTAIN,
             TREES, TREES, WATER, WATER, WATER, WATER]
    random.shuffle(tiles)
    for tile in tiles:
      for i in range(16):
        points = []
        size = round((self.map_width * self.map_height) / 20)
        size = random.randint(round(size/4), size)
        points.append((random.randint(0,self.map_width-1), 
                      random.randint(0,self.map_height-1)))
        while size > 0:
          directions = random.randint(0,15)
          new_points = []
          for point in points:
            self.grid[point[0]][point[1]] = tile
            if (directions & 8 and point[0] > 0 
                and self.grid[point[0]-1][point[1]] != tile): #up
              new_points.append((point[0]-1, point[1]))
            if (directions & 4 and point[0] < self.map_height-1
                and self.grid[point[0]+1][point[1]] != tile): #down
              new_points.append((point[0]+1, point[1]))
            if (directions & 2 and point[1] > 0
                and self.grid[point[0]][point[1]-1] != tile): #left
              new_points.append((point[0], point[1]-1))
            if (directions & 1 and point[1] < self.map_width-1
                and self.grid[point[0]][point[1]+1] != tile): #right
              new_points.append((point[0], point[1]+1))
            size -= 1
          if (len(new_points)):
            points = new_points

    # smooth out the map a bit so it compresses better
    for i in range(len(self.grid)):
      row = self.grid[i]
      for j in range(self.map_width-2):
        if (row[j] != row[j+1] and row[j] == row[j+2]):
          row[j+1] = row[j]
        # add in some bridges
        if (row[j+1] == WATER and WATER not in (row[j], row[j+2]) 
             and MOUNTAIN not in (row[j], row[j+2])
             and (i < 1 or self.grid[i-1][j+1] in IMPASSABLE)
             and (i >= self.map_width-1 or self.grid[i+1][j+1] in IMPASSABLE)):
          row[j+1] = BRIDGE
        if (j < self.map_width-3 and row[j+1] == WATER and row[j+1] == WATER 
             and WATER not in (row[j], row[j+3]) 
             and MOUNTAIN not in (row[j], row[j+3])):
          row[j+1] = row[j+2] = row[j]
    self.place_landmarks()
    self.encode()
    self.generated = True
    return self.grid

  def place_landmarks(self):
    self.warps_from[self.tantegel_warp] = None
    self.warps_from[self.charlock_warp] = None
    for i in range(6):
      self.warps_from[self.town_warps[i]] = None
    for i in range(6):
      self.warps_from[self.cave_warps[i]] = None

    # keep tantegel in regular zone 0 for now
    x, y = self.random_land(30, 58, 30, 44)
    tantegel = (x, y)
    self.add_warp(1, x, y, CASTLE)
    self.grid[y][x] = CASTLE #tantegel

    grid = MapGrid(self.grid)
    
    # place Charlock
    x, y = self.accessible_land(grid, *tantegel, 6, 118, 3, 116)
    charlock = (x-3, y)
    self.place_charlock(x-3, y)

    # make sure there's a town near tantegel
    x, y = charlock
    while (abs(x - charlock[0]) < 4 and abs(y - charlock[1]) < 4):
      x, y = self.accessible_land(grid, *tantegel, x-10, x+10, y-10, y+10)
    self.add_warp(1, x, y, TOWN)
    self.grid[y][x] = TOWN

    for i in range(5):
      x, y = charlock
      while (abs(x - charlock[0]) < 4 and abs(y - charlock[1]) < 4):
        x, y = self.accessible_land(grid, *tantegel)
      self.add_warp(1, x, y, TOWN)
      self.grid[y][x] = TOWN

    for i in range(6):
      x, y = charlock
      while (abs(x - charlock[0]) < 4 and abs(y - charlock[1]) < 4):
        x, y = self.accessible_land(grid, *tantegel)
      self.add_warp(1, x, y, CAVE)
      self.grid[y][x] = CAVE

  def place_charlock(self, x, y):
    self.add_warp(1, x, y, CASTLE)
    self.grid[y][x] = CASTLE #charlock
    for i in range(-3, 4):
      for j in range(-3, 4):
        if abs(i) >= 3 or abs(j) >= 3:
          if not (i > 0 and j == 0):
            self.grid[y+j][x+i] = BLOCK
        elif abs(i) >= 2 or abs(j) >= 2:
          self.grid[y+j][x+i] = WATER
        elif not (i==0 and j==0):
          self.grid[y+j][x+i] = SWAMP
    self.grid[y][x+2] = BRIDGE #temporary

  def accessible_land(self, grid, fromx, fromy, minx=1, maxx=118, miny=1, maxy=118):
    came_from = {}
    x = y = 0
    while not (x, y) in came_from.keys():
#      x, y = self.random_land(minx, maxx, miny, maxy)
      x, y = random.randint(minx, maxx), random.randint(miny, maxy)
      came_from, cost_so_far = pathfinding.a_star_search(
                                     grid, (fromx, fromy), (x, y))
    return x, y
    

  def random_land(self, minx=1, maxx=118, miny=1, maxy=118):
    x, y = random.randint(minx, maxx), random.randint(miny, maxy)
    while self.grid[y][x] not in (GRASS, DESERT, HILL, TREES, SWAMP):
      x, y = random.randint(minx, maxx), random.randint(miny, maxy)
    return x, y

  def add_warp(self, m, x, y, type_):
    if type_ == CASTLE:
      if not self.warps_from[self.tantegel_warp]:
        self.warps_from[self.tantegel_warp] = [m, x, y]
        # update the return point
        if self.grid[y+1][x] not in IMPASSABLE:
          self.return_point = [m, x, y+1]
        elif self.grid[y-1][x] not in IMPASSABLE:
          self.return_point = [m, x, y-1]
        elif self.grid[y][x-1] not in IMPASSABLE:
          self.return_point = [m, x-1, y]
        elif self.grid[y][x+1] not in IMPASSABLE:
          self.return_point = [m, x+1, y]
      else:
        self.warps_from[self.charlock_warp] = [m, x, y]
    elif type_ == TOWN:
      for i in range(0,6):
        if self.warps_from[self.town_warps[i]] is None:
          self.warps_from[self.town_warps[i]] = [m, x, y]
          break
    elif type_ == CAVE:
      for i in range(0,6):
        if self.warps_from[self.cave_warps[i]] is None:
          self.warps_from[self.cave_warps[i]] = [m, x, y]
          break
    
  def encode(self):
    """
    Encodes the map into the proper format for insertion into the ROM.

    :Parameters:
      map_grid : array
        An array of integers indicating the tiles of the map.

    rtype: bytearray
    return: The RLE data for the ROM 
    """
    map_data = []
    pointer_data = []
    for row in self.grid:
      pointer_addr = len(map_data) + 0x9d5d
      pointer_data.append(pointer_addr & 0xff)
      pointer_data.append(pointer_addr >> 8)
      last_tile = None
      count = 0
      for tile in row:
        if tile == last_tile:
          count += 1
        elif not last_tile is None:
          map_data.append(last_tile << 4 | count)
          last_tile = tile
          count = 0
        if last_tile is None:
          last_tile = tile
        if count == 15:
          map_data.append(tile << 4 | count)
          last_tile = None
          count = 0
      if not last_tile is None:
        map_data.append(tile << 4 | count)
        last_tile = None
        count = 0
          
    self.encoded = self.optimize(bytearray(map_data), bytearray(pointer_data))
    return self.encoded


  def decode(self, map_data):
    """
    Decodes a map from the orignal format from the ROM

    :Parameters:
      map_data : bytearray
        The RLE raw data from the ROM
      pointer_data : bytearray
        The pointers to the beginning of each row in the ROM.

    rtype: 
    return: 
    """
    pointer_data = map_data[-240:]
    map_data = map_data[:-240]
    pointers = []
    for i in range(0, 240, 2):
      # pointer data -> value - 0x8000 + 16
      pointer = pointer_data[i+1] << 8 | pointer_data[i]
      pointer -= 0x9d5d
      pointers.append(pointer)

    colcount = 0
    rowcount = 0
    currentline = ""
    self.grid = []
    for i in range(len(pointers)-1):
      row = map_data[pointers[i]:]
      colcount = 0
      decoded_row = []
      for byte in row:
        tile = byte >> 4
        count = byte & 0xf
        for j in range(count+1):
          if colcount >= self.map_width: break
          decoded_row.append(tile)
          colcount += 1
        if colcount >= self.map_width: break
      self.grid.append(decoded_row)
    return self.grid


  def optimize(self, encoded_map, pointers):
    """
    Optimizes the map by removing unneeded bytes. Not yet implemented.

    :Parameters:
      encoded_map : bytearray
        The map encoded for use in the ROM. This will remove bytes that are 
        unneccesary. 
      pointers : bytearray
        The encoded pointers to the beginning of each tile row.

    rtype: bytearray
    return: The map optimized for use in the ROM.
    """
    #extend the map data to the right length.
    encoded_map += bytearray([0xff] * (self.encoded_size - len(encoded_map)))
    return encoded_map + pointers

  def from_rom(self, rom_file):
    f = open(rom_file, 'rb')
    self.from_rom_data(f.read()) 
    f.close()


  def from_rom_data(self, rom_data):
    """
    Creates a map from existing rom data.

    :Parameters:
      rom_data : bytearray
        The ROM data to be parsed
    """
    self.rom_data = rom_data
    self.encoded = rom_data[0x1d6d:0x2753]
    self.decode(self.encoded)
    self.read_warps()

    # read the current return point
    self.return_point = []
    for i in range(3):
      self.return_point.append(self.rom_data[self.return_point_addr[i]])

    # read the fixed encounter data
    self.axe_knight = self.rom_data[self.axe_knight_slice]
    self.green_dragon = self.rom_data[self.green_dragon_slice]
    self.golem = self.rom_data[self.golem_slice]

  def read_warps(self):
    self.warps_from = []
    self.warps_to = []
    start = 0xf3db
    for i in range(51):
      self.warps_from.append(self.rom_data[start:start+3])
      start += 3
    for i in range(51):
      self.warps_to.append(self.rom_data[start:start+3])
      start += 3
    
  def shuffle_warps(self):
    cave_end = 8 if self.generated else 7 
    caves = []
    towns = []
    for i in range(cave_end):
      caves.append(self.warps_from[self.cave_warps[i]])
    random.shuffle(caves)
    for i in range(6):
      towns.append(self.warps_from[self.town_warps[i]])
    random.shuffle(towns)
    while not self.generated and (caves[3][0] == 21 or
          (towns[3][0] == 11 and 21 in (caves[5][0], caves[6][0])) or
          (towns[3][0] ==  9 and caves[6][0] == 21)):
      random.shuffle(caves)

    # save the shuffling
    for i in range(cave_end):
      self.warps_from[self.cave_warps[i]] = caves[i]
    for i in range(6):
      self.warps_from[self.town_warps[i]] = towns[i]

  def commit(self):
    if len(self.encoded) > 2534:
      print("Unable to commit changes, map is too large.")
      return
    self.rom_data[0x1d6d:0x2753] = self.encoded
    # update with the new warps
    warp_start = 0xf3db
    for i in range(51):
      self.rom_data[warp_start:warp_start+3] = self.warps_from[i]
      warp_start += 3
    for i in range(51):
      self.rom_data[warp_start:warp_start+3] = self.warps_to[i]
      warp_start += 3

    #update the return point
    for i in range(3):
      self.rom_data[self.return_point_addr[i]] = self.return_point[i]

    # update the fixed encounters
    if self.axe_knight:
      self.rom_data[self.axe_knight_slice] = self.axe_knight
    if self.green_dragon:
      self.rom_data[self.green_dragon_slice] = self.green_dragon
    if self.golem:
      self.rom_data[self.golem_slice] = self.golem
    
  def to_html(self):
    """
    Outputs a map to HTML format for viewing.

    :Parameters:
      map_grid : array
        An array of integers indicating the tiles of the map.

    """
    html =("<html><head><style type='text/css' rel='stylesheet'>"
           "td { width: 16px; height: 16px; } "
           ".grass { background: no-repeat center center url(data:image/gif;base64,R0lGODlhEAAQAIQWAAAAAIwQAOAAXABc/0xgfAB8/wCsAP88AOBcEHx8fHyQrDy8//98XLCcgFzcUP+gQLy8vMDAwLz/HP/cfLz/vP/grP///////////////////////////////////////yH+EUNyZWF0ZWQgd2l0aCBHSU1QACwAAAAAEAAQAAAFJKAkisZoliNqSurqvm6bkq8M33OM72fd67wgyzcECm+24nEUAgA7); } "
           ".desert { background: no-repeat center center url(data:image/gif;base64,R0lGODlhEAAQAIQWAAAAAIwQAOAAXABc/0xgfAB8/wCsAP88AOBcEHx8fHyQrDy8//98XLCcgFzcUP+gQLy8vMDAwLz/HP/cfLz/vP/grP///////////////////////////////////////yH+EUNyZWF0ZWQgd2l0aCBHSU1QACwAAAAAEAAQAAAFJuA0IWIpkmZ6qmOKuuYbs3Rdyveq4vDe/zVea+bLFW1IYk8oNP5CADs=); } "
           ".hill { background: no-repeat center center url(data:image/gif;base64,R0lGODlhEAAQAIQWAAAAAIwQAOAAXABc/0xgfAB8/wCsAP88AOBcEHx8fHyQrDy8//98XLCcgFzcUP+gQLy8vMDAwLz/HP/cfLz/vP/grP///////////////////////////////////////yH+EUNyZWF0ZWQgd2l0aCBHSU1QACwAAAAAEAAQAAAFSKAjisA4liZqruyITEjavu/EqkgOT3YL5LTdTVfjvVLAHc/Y+/1gNWLMGX3+aLkrTDvRIq5YLfgr9XazgGs6DVyj1id4a74KAQA7); } "
           ".mountain { background: no-repeat center center url(data:image/gif;base64,R0lGODlhEAAQAIQWAAAAAIwQAOAAXABc/0xgfAB8/wCsAP88AOBcEHx8fHyQrDy8//98XLCcgFzcUP+gQLy8vMDAwLz/HP/cfLz/vP/grP///////////////////////////////////////yH+EUNyZWF0ZWQgd2l0aCBHSU1QACwAAAAAEAAQAAAFUKAjikAyjsCZAgCFmudLmaUbk8ns1DS587scTFWbFYc4ilGoQ/ESz+YvR2lZhS+qVUmlsXK1VhVb0grBwlK4laCOq+V32+ytztJQG4nF7+tDADs=); } "
           ".water { background: no-repeat center center url(data:image/gif;base64,R0lGODlhEAAQAIQWAAAAAIwQAOAAXABc/0xgfAB8/wCsAP88AOBcEHx8fHyQrDy8//98XLCcgFzcUP+gQLy8vMDAwLz/HP/cfLz/vP/grP///////////////////////////////////////ywAAAAAEAAQAAAFKGAhjmRplpZ1mmqKuqtauPB6yiJu5/Pu/6ParqUD4lLFG5E1DPaAvxAAOw==); } "
           ".block { background: no-repeat center center url(data:image/gif;base64,R0lGODlhEAAQAKEDAAAAAH9/f7y8vP///yH+EUNyZWF0ZWQgd2l0aCBHSU1QACwAAAAAEAAQAAACNgQighm41pKSVAVQM7taH9ZJHBYyX6QhB2mlXymy8LWU9EQ5uVzLFsahsGiNw8UIOa6OxaWhAAA7); } "
           ".trees { background: no-repeat center center url(data:image/gif;base64,R0lGODlhEAAQAIQWAAAAAIwQAOAAXABc/0xgfAB8/wCsAP88AOBcEHx8fHyQrDy8//98XLCcgFzcUP+gQLy8vMDAwLz/HP/cfLz/vP/grP///////////////////////////////////////yH+EUNyZWF0ZWQgd2l0aCBHSU1QACwAAAAAEAAQAAAFTKAkAoZoSuSJGuxJtmMLv9LM3vFdvi/L64ZezYcjAoNHowxZJPWIT6XhIaU6dYDrDYB6UB/c7jcsVgG8puwZPMp6w+41N/5uq9PqcAgAOw==); } "
           ".swamp { background: no-repeat center center url(data:image/gif;base64,R0lGODlhEAAQAKEDAAAAAEioEJjoAP///yH+EUNyZWF0ZWQgd2l0aCBHSU1QACwAAAAAEAAQAAACLIQlpoAascyLlKXYZtS14XstnihBm1lxDmpRo1pWn5vAgK2MOUi78Sm7sWIFADs=); } "
           ".town { background: no-repeat center center url(data:image/gif;base64,R0lGODlhEAAQAKEDAAAAAEBg+Ly8vP///yH+EUNyZWF0ZWQgd2l0aCBHSU1QACwAAAAAEAAQAAACOoSPqSl7bYR0YMq78Nib1TkE4VCVVsN1Eqmi5COkH0BeTSyqkY3mLb9KsYRCF42YMm5EItdAWaRRFgUAOw==); } "
           ".cave { background: no-repeat center center url(data:image/gif;base64,R0lGODlhEAAQAKEEAAAAAH9/f7y8vJjoACH+EUNyZWF0ZWQgd2l0aCBHSU1QACwAAAAAEAAQAAACLZyPqcsjD4UCD9hLpaG4W815mBSKY2l+aCqsZtumFyzPZYDngR3XdASs9GqWAgA7); } "
           ".castle { background: no-repeat center center url(data:image/gif;base64,R0lGODlhEAAQAKEDAAAAAEBg+Ly8vP///yH+EUNyZWF0ZWQgd2l0aCBHSU1QACwAAAAAEAAQAAACQIRvM8is24A6MFU5gRA0yyN4l8aR0IaE0GOooPrEXom2tB1vMCsEgVLSKHyL3o+jExKFQBIyxhwgUTsn9ZqMaAsAOw==); } "
           ".bridge { background: no-repeat center center url(data:image/gif;base64,R0lGODlhEAAQAKEDAAAAAEBg+Ly8vP///yH+EUNyZWF0ZWQgd2l0aCBHSU1QACwAAAAAEAAQAAACPYwDqXmnCAGShskb811rbgGGIAeIZqicqlCu7ki2YjLUdp2g+ADwPs7y9YbC3e9mI95ivSOj4WjyoFTEoAAAOw==); } "
           ".stairs { background: no-repeat center center url(data:image/gif;base64,R0lGODlhEAAQAKEEAAAAAH9/f7y8vJjoACH+EUNyZWF0ZWQgd2l0aCBHSU1QACwAAAAAEAAQAAACPZwHcZdr7WCYIVFqK9i8e+SFmxEIpsgNkymgW1WacRuq7Fzb8s2O6w6kwXjBEkAnmCkFJGKSp8JcpBOXqAAAOw==); } "
           "</style><body><table border='0' cellpadding='0' cellspacing='0' width='%d'>" % (16 * self.map_width))
    for row in self.grid:
      html += "<tr>"
      for tile in row:
        html += ("<td class='%s'></td>" % self.tiles[tile])
      html += "</tr>"
      
    html += "</table></body></html>"
    return html

class MapGrid(pathfinding.SquareGrid):

  def __init__(self, grid):
    pathfinding.SquareGrid.__init__(self, len(grid[0]), len(grid))
    self.grid = grid

  def passable(self, id):
    x, y = id
    return self.grid[y][x] not in IMPASSABLE

  def cost(self, *args):
    return 1

if __name__ == "__main__":

  parser = argparse.ArgumentParser(prog="DWRandomizer",
      description="A randomizer for Dragon Warrior for NES")
  parser.add_argument("--test", action="store_true",
      help="Generate and export a test map.")
           
  parser.add_argument("filename", help="The rom file to use for input")
  args = parser.parse_args()

  world_map = WorldMap()
  world_map.from_rom(args.filename)
  if args.test:
    world_map.generate()
  # 0x8f6 is the maximum size for an encoded map that will fit in the ROM.
    while len(world_map.encoded) > 0x8f6 + 240:
      print("World Map too large (%d/2534 bytes), retrying... " % 
            len(world_map.encoded))
      world_map.generate()
  f = open('map.html', 'w')
  f.write(world_map.to_html())
  f.close()

