#!/bin/env python3

import random

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



class WorldMap:
  def __init__(self): 
    self.tiles = ("grass", "desert", "hill", "mountain", "water", "block", 
                "trees", "swamp", "town", "cave",  "castle", "bridge", "stairs")
    self.encoded_size = 0x8f6
    self.map_width = 120
    self.map_height = 120
    self.grid = None
    self.data = None
    self.pointers = None
    self.cave_warps = []
    self.town_warps = []
    self.tantegel_warp = None
    self.charlock_warp = None

#  def generate(self, start_alive=0.45, birth_limit=3, death_limit=3, 
#               starvation_limit=12, iterations=100): 
#    """
#    Generates a new map for use in the Dragon Warrior ROM.
#
#    rtype: array
#    return: The newly generated map
#    """
#    self.grid = []
#    for i in range(self.map_height):
#      self.grid.append([])
#      for j in range(self.map_width):
#        if random.random() < start_alive:
#          self.grid[i].append(0)
#        else:
#          self.grid[i].append(4)
#    self.refine(self.iterations, birth_limit, death_limit, starvation_limit, 
#                iterations)
#    return self.grid

#  def refine(self, birth_limit=3, death_limit=3, starvation_limit=12, 
#             iterations=100):
#    """
#    Performs one step in the refinement operation.
#
#    :Parameters:
#
#    rtype: 
#    return: 
#    """
#    for i in range(iterations):
#      new_map = []
#      for i in range(self.map_width):
#        new_map.append([])
#        for j in range(self.map_height):
#          count = 0
#          for k in range(-1, 2):
#            for l in range(-1, 2):
#              cellX,cellY = i+k, j+l
#              if cellX == 0 and cellY == 0:
#                continue
#              if cellX < 0 or cellX >= self.map_width or cellY < 0 or cellY >= self.map_height:
#                continue
#              if self.grid[cellX][cellY] == 0:
#                if 0 in (cellX, cellY): # directly above or below
#                  count += 2
#                else:
#                  count += 1
#          if count > self.starvation_limit:
#            new_map[i].append(4)
#          if count > self.birth_limit:
#            new_map[i].append(0)
#          elif count < self.death_limit:
#            new_map[i].append(4)
#          else:
#            new_map[i].append(self.grid[i][j])
#    self.grid = new_map
#    return self.grid
#
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
             and (i <= 1 or self.grid[i-1][j+1] in (MOUNTAIN, WATER))
             and (i >= self.map_width-1 or self.grid[i+1][j+1] in (MOUNTAIN, WATER))):
          row[j+1] = BRIDGE
        if (j < self.map_width-3 and row[j+1] == WATER and row[j+1] == WATER 
             and WATER not in (row[j], row[j+3]) 
             and MOUNTAIN not in (row[j], row[j+3])):
          row[j+1] = row[j+2] = row[j]
    self.placelandmarks()
    return self.grid

  def placelandmarks(self):
    x, y = self.random_land()
    self.add_warp(1, x, y, CASTLE)
    self.grid[y][x] = CASTLE #tantegel
    x, y = self.random_land(6, 118, 3, 116)
    self.place_charlock(x-3, y)
    for i in range(6):
      x, y = self.random_land()
      self.grid[y][x] = TOWN
    for i in range(6):
      x, y = self.random_land()
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

  def random_land(self, minx=1, maxx=118, miny=1, maxy=118):
    x, y = random.randint(minx, maxx), random.randint(miny, maxy)
    while ((self.grid[y][x-1] in (WATER, MOUNTAIN)) and
           (self.grid[y][x+1] in (WATER, MOUNTAIN)) and
           (self.grid[y-1][x] in (WATER, MOUNTAIN)) and
           (self.grid[y+1][x] in (WATER, MOUNTAIN))):
      x, y = random.randint(minx, maxx), random.randint(miny, maxy)
    return x, y

  def add_warp(self, m, x, y, type_):
    if type_ == CASTLE:
      if not self.charlock_warp:
        self.charlock_warp = (m, x, y)
      else:
        self.tantegel_warp = (m, x, y)
    elif type_ == TOWN:
      self.town_warps.append((m, x, y))
    elif type_ == CAVE:
      self.cave_warps.append((m, x, y))
    

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
          
    return self.optimize(bytearray(map_data), bytearray(pointer_data))


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
    Optimizes the map by removing unneeded bytes

    :Parameters:
      new_encoded_map : bytearray
        The map encoded for use in the ROM. This will remove bytes that are 
        unneccesary. This data should include the map pointers.

    rtype: bytearray
    return: The map optimized for use in the ROM.
    """
    #extend the map data to the right length.
    encoded_map += bytearray([0xff] * (self.encoded_size - len(encoded_map)))
    return encoded_map + pointers

  def from_rom(self, rom_file):
    f = open(rom_file, 'rb')

    f.read(0x1d6d)
    world_map_data = f.read(0x8f6 + 240) # tiles + pointer data
    f.close()
    self.decode(world_map_data)
    
    
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

if __name__ == "__main__":

  world_map = WorldMap()
#  world_map.from_rom("/home/mcgrew/vbox_shared/Dragon Warrior (U) (PRG0) [!].nes")
#  world_map.generate(birth_limit=3, death_limit=4, iterations=100)
  world_map.generate()
  encoded = world_map.encode()
  # 0x8f6 is the maximum size for an encoded map that will fit in the ROM.
  while len(encoded) > 0x8f6 + 240:
    print("World Map too large (%d/2534 bytes), retrying... " % len(encoded))
    world_map.generate()
    encoded = world_map.encode()
  f = open('map.html', 'w')
  f.write(world_map.to_html())
  f.close()


