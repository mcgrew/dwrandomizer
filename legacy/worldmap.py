#!/bin/env python3

import random
import argparse
import pathfinding
import struct
import ips

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

# some border tile values
BORDER = {
  GRASS: 0,
  DESERT: 1,
  HILL: 2,
#  BRICK: 4,
  SWAMP: 6,
  TREES: 11,
  WATER: 15,
  BLOCK: 16
#  ROOF: 21
}

MAPS = { 2: "Charlock", 3: "Hauksness", 4: "Tantegel", 6: "Charlock", 7: "Kol", 
         8: "Brecconary", 9: "Garinham", 10: "Cantlin", 11: "Rimuldar",
         12: "Tantegel Basement", 13:"Northern Shrine", 14: "Southern Shrine",
         21: "Swamp Cave", 22: "Mountain Cave", 24: "Garin's Grave", 
         28: "Erdrick's Cave" }

class WorldMap:
  encoded_size = 0x8f6
  map_width = 120
  map_height = 120
  min_walkable = 6000 # minimum accessible land area
  cave_warps = (1, 5, 8, 12, 13, 17, 19, 7)
  town_warps = (0, 2, 3, 9, 10, 11)
  tantegel_warp = 4
  charlock_warp = 6
  love_calc_slice = slice(0xdf67, 0xdf4a, -28)
  return_point_addr = (0xdb11, 0xdb15, 0xdb1d)
  axe_knight_slice = slice(0xcd64, 0xcd76, 6)
  green_dragon_slice = slice(0xcd7b, 0xcd8d, 6)
  golem_slice = slice(0xcd98, 0xcdaa, 6)
  rainbow_drop_slice = slice(0xde9e, 0xdeab, 6)
  rainbow_bridge_slice = slice(0x2c4e, 0x2c5b, 6)
  tiles = ("grass", "desert", "hill", "mountain", "water", "block", 
           "trees", "swamp", "town", "cave",  "castle", "bridge", "stairs")
  border_addresses = {3:0x3d, 4:0x42, 7:0x51, 8:0x56, 9:0x5b, 10:0x60, 11:0x65}


  def __init__(self, rom_data=None): 
    self.rom_data = rom_data
    self.grid = None
    self.warps_from = []
    self.warps_to   = []
    self.return_point = None
    self.generated = False
    self.return_point = None
    self.axe_knight = None
    self.green_dragon = None
    self.golem = None
    self.chests = None
    self.revert()
    self.error = None

  def generate(self):
    """
    Potential alternate implementation for map generation.

    rtype: array
    return: The newly generated map
    """
    self.error = None
    self.grid = []
    for i in range(self.map_height):
      self.grid.append([WATER]*self.map_width)
    tiles = [GRASS, GRASS, GRASS, SWAMP, DESERT, DESERT, HILL, MOUNTAIN,
             TREES, TREES, TREES, WATER, WATER, WATER, WATER]
    for i in range(12):
      random.shuffle(tiles)
      for tile in tiles:
        points = []
        size = round((self.map_width * self.map_height) / 30)
        size = random.randint(round(size/4), size)
        if tile == MOUNTAIN:
          size >>= 1
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

    # smooth out the map a bit for better compression
    for i in range(len(self.grid)):
      row = self.grid[i]
      for j in range(self.map_width-2):
        if (row[j] != row[j+1] and row[j] == row[j+2]):
          row[j+1] = row[j]
        # add in some bridges
        if (row[j+1] == WATER and WATER not in (row[j], row[j+2]) and
             MOUNTAIN not in (row[j], row[j+2]) and 
             (i < 1 or self.grid[i-1][j+1] in IMPASSABLE) and
             (i >= self.map_height-1 or self.grid[i+1][j+1] in IMPASSABLE)):
          row[j+1] = BRIDGE
        if (j < self.map_width-3 and row[j+1] == WATER and row[j+1] == WATER 
             and WATER not in (row[j], row[j+3]) 
             and MOUNTAIN not in (row[j], row[j+3])):
          row[j+1],row[j+2] = row[j],row[j+3]

    #remove errantly placed bridges 
    for i in range(len(self.grid)-1):
      for j in range(1, len(self.grid[i])):
        if self.grid[i][j] == BRIDGE and self.grid[i+1][j] not in IMPASSABLE:
          self.grid[i][j] = self.grid[i][j-1]

    try:
      self.place_landmarks()
    except SanityError as e:
      self.error = e
      return False

    self.encode()
    if len(self.encoded) > 2534:
      self.error = SanityError("Compressed map is too large (%d bytes)" 
              % len(self.encoded))
      return False
    self.generated = True
    self.update_warps()
    return True

  def set_border_tile(self, index, x, y):
    """
    Converts a map tile into a border tile for towns
    :Parameters:
      index : int
        The map index of the town or castle
      x : int
        The x coordinate where the town will be placed
      y : int
        The y coordinate where the town will be placed
    """
    border_tile = BORDER.get(self.tile_at(x, y), 0)
    if index in self.border_addresses:
      self.patch.add_record(self.border_addresses[index], (border_tile,))
    else:
      print("Error in border tile setting: invalid index %d specified" % index)
      

  def place_landmarks(self):
    """
    Places landmarks on the map (castles, towns and caves)
    """
    self.warps_from[self.tantegel_warp] = None
    self.warps_from[self.charlock_warp] = None
    for i in range(6):
      self.warps_from[self.town_warps[i]] = None
    for i in range(8):
      # clear the caves on the overworld
      if self.warps_from[self.cave_warps[i]][0] == 1:
        self.warps_from[self.cave_warps[i]] = None

    # place tantegel castle
    x, y = self.random_land()
    tantegel = (x, y)
    self.add_warp(1, x, y, CASTLE)

    grid = MapGrid(self.grid)

    if self.plot_size(grid, tantegel) < self.min_walkable:
      raise SanityError("Accessible land area is too small")

    # place Charlock
    while (self.closer_than(5, x-3, y, *tantegel) or
        self.tile_at( x, y) in (TOWN, CASTLE, CAVE, STAIRS)):
      x, y = self.accessible_land(grid, tantegel, 6, 118, 3, 116)
    charlock = (x-3, y)
    self.place_charlock(x-3, y)

    # check again, just in case.
    if self.plot_size(grid, tantegel) < self.min_walkable:
      raise SanityError("Accessible land area is too small")

    if not self.is_accessible(grid, tantegel, (x, y)):
      raise SanityError("Charlock is obstructed")

    for i in range(6):
      x, y = charlock
      while (self.closer_than(5, x, y, *charlock) or
          self.grid[y][x] in (TOWN, CASTLE, CAVE, STAIRS)):
        x, y = self.accessible_land(grid, tantegel)
      self.add_warp(1, x, y, TOWN)

    for i in range(6):
      x, y = charlock
      while (self.closer_than(5, x, y, *charlock) or
          self.grid[y][x] in (TOWN, CASTLE, CAVE, STAIRS)):
        x, y = self.accessible_land(grid, tantegel)
      self.add_warp(1, x, y, CAVE)

  def closer_than (self, distance, x1, y1, x2, y2):
    """
    Determines whether x1,y1 is closer than distance vertically and
    horizontally to x2,y2.

    :Parameters:
      x1 : int
        The x coordinate of the first point.
      y1 : int
        The y coordinate of the first point.
      x2 : int
        The x coordinate of the second point.
      y2 : int
        The y coordinate of the secont point.
      distance : int
        The distance for the calculation.

    rtype: bool
    return: A boolean indicating whether the 2 points are closer than distance.
    """
    return (abs(x1 - x2) < distance and abs(y1 - y2) < distance)
    
  def tile_at(self, x, y):
    """
    Returns the tile type at the given x,y coordinate.
    
    :Parameters:
      x : int
        The x coordinate of the tile.
      y : int
        The y coordinae of the tile.

    rtype: int
    return: The tile type at the given coordinates.
    """
    return self.grid[y][x]

  def set_tile(self, x, y, tile):
    """
    Sets the type of the given tile

    :Parameters:
      x : int
        The x coordinate of the tile
      y : int
        The y coordinate of the tile
      tile : int
        The new tile type.
    """
    self.grid[y][x] = tile

  def place_charlock(self, x, y):
    """
    Places Charlock Castle on the map.
    :Parameters:
      x : int
        The x coordinate of Charlock
      y : int
        The y coordinate of Charlock
    """
    self.add_warp(1, x, y, CASTLE)
    self.set_tile(x, y, CASTLE) #charlock
    for i in range(-3, 4):
      for j in range(-3, 4):
        if abs(i) >= 3 or abs(j) >= 3:
          if not (i > 0 and j == 0):
            self.set_tile(x+i, y+j, BLOCK)
        elif abs(i) >= 2 or abs(j) >= 2:
          self.set_tile(x+i, y+j, WATER)
        elif not (i==0 and j==0):
          self.set_tile(x+i, y+j, SWAMP)
    self.rainbow_bridge = [1, x+2, y]

  def accessible_land(self, grid, from_, minx=1, maxx=118, miny=1, maxy=118):
    """
    Returns a random land tile that is accessible from the given tile
    :Parameters:
      grid : MapGrid
        A MapGrid object created from the world map grid
      fromx : int
        The x coordinate the tile must be accessible from
      fromy : int
        The y coordinate the tile must be accessible from
      minx : int
        The minimum x coordinate (default 1)
      maxx : int
        The maximum x coordinate (default 118)
      miny : int
        The minimum x coordinate (default 1)
      maxy : int
        The maximum y coordinate (default 118)


    rtype: tuple
    return: An x and y coordinate on the map.
    """
    came_from = {}
    x = y = 0
    while not (x, y) in came_from.keys():
      x, y = random.randint(minx, maxx), random.randint(miny, maxy)
      came_from, cost_so_far = pathfinding.a_star_search(grid, from_, (x, y))
    return x, y
    
  def is_accessible(self, grid, from_, to):
    """
    Determines if a particular tile is accessible from another
    :Parameters:
      grid : MapGrid
        A MapGrid object created from the world map grid
      from : tuple(int)
        x and y coordinates of the starting point.

    rtype: bool
    return: Whether or not the player is able to walk between the 2 coordinates.
    """
    came_from, cost_so_far = pathfinding.a_star_search(grid, from_, to)
    return to in came_from.keys()

  def plot_size(self, grid, point):
    """
    Determines if a particular tile is accessible from another
    :Parameters:
      grid : MapGrid
        A MapGrid object created from the world map grid
      from_ : tuple(int)
        x and y coordinates of the starting point.
      to : tuple(int)
        x and y coordinates of the ending point.

    rtype: bool
    return: Whether or not the player is able to walk between the 2 coordinates.
    """
    came_from, cost_so_far = \
      pathfinding.a_star_search(grid, point, (self.map_width, self.map_height))
    return len(came_from.keys())

  def random_land(self, minx=1, maxx=118, miny=1, maxy=118):
    """
    Returns a random land tile.
    :Parameters:
      minx : int
        The minimum x coordinate (default 1)
      maxx : int
        The maximum x coordinate (default 118)
      miny : int
        The minimum x coordinate (default 1)
      maxy : int
        The maximum y coordinate (default 118)


    rtype: tuple
    return: An x and y coordinate on the map.
    """
    x, y = random.randint(minx, maxx), random.randint(miny, maxy)
    while self.grid[y][x] not in (GRASS, DESERT, HILL, TREES, SWAMP):
      x, y = random.randint(minx, maxx), random.randint(miny, maxy)
    return x, y

  def add_warp(self, m, x, y, type_):
    """
    Adds a warp item to the map (castle, town, or cave)
    :Parameters:
      m : int
        The map to add the warp to (always 1)
      x : int
        The x coordinate on the map
      y : int
        The y coordinate on the map
      type_ : int
        The type of warp to add
    """
    if type_ == CASTLE:
      if not self.warps_from[self.tantegel_warp]:
        self.set_border_tile(4, x, y)
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
          self.set_border_tile(self.warps_to[self.town_warps[i]][0], x, y)
          self.warps_from[self.town_warps[i]] = [m, x, y]
          break
    elif type_ == CAVE:
      for i in range(0,8):
        if self.warps_from[self.cave_warps[i]] is None:
          self.warps_from[self.cave_warps[i]] = [m, x, y]
          break
    else:
      return
    self.set_tile(x, y, type_)
    
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
    pointers = []
    for row in self.grid:
      pointers.append(len(map_data) + 0x9d5d)
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

    map_data, pointers = self.optimize(bytearray(map_data), pointers)
    # create a byte array from the pointers, suitable for ROM insertion.
    pointer_data = bytearray()
    for i in range(len(pointers)):
      pointer_data.append(pointers[i] & 0xff)
      pointer_data.append(pointers[i] >> 8)

    self.encoded = map_data + pointer_data
    self.add_patch(0x1d6d, self.encoded)
    return self.encoded

  def optimize(self, encoded_map, pointers):
    """
    Optimizes the map by removing unneeded bytes. Not yet implemented.

    :Parameters:
      encoded_map : bytearray
        The map encoded for use in the ROM. This will remove bytes that are 
        unneccesary. 
      pointers : array(int)
        The pointer addresses to the beginning of each tile row.

    rtype: bytearray
    return: The map optimized for use in the ROM.
    """
    # remove redundant bytes at the end of each row.
    print("Optimizing compressed map... ", end="")
    saved = 0
    for i in range(1, len(pointers)):
      offset = pointers[i] - 0x9d5d
      if (((encoded_map[offset-1] & 0xf0) == (encoded_map[offset] & 0xf0)) and 
         ((encoded_map[offset-1] & 0xf) <= (encoded_map[offset] & 0xf))):
        saved += 1
        encoded_map = encoded_map[:offset-1] + encoded_map[offset:]
        for j in range(i, len(pointers)):
          pointers[j] -= 1
    print("(Saved %d bytes)" % saved)

    #extend the map data to the right length.
    encoded_map += bytearray([0xff] * (self.encoded_size - len(encoded_map)))
    return encoded_map, pointers

  def decode(self, map_data):
    """
    Decodes a map from the orignal ROM format

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
    for i in range(len(pointers)):
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


  def revert(self):
    """
    Creates a map from existing rom data.

    :Parameters:
      rom_data : bytearray
        The ROM data to be parsed
    """
    self.encoded = self.rom_data[0x1d6d:0x2753]
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

    self.rainbow_bridge = self.rom_data[self.rainbow_bridge_slice]
    self.patch = ips.Patch()

  def read_warps(self):
    """
    Reads existing warps from the rom data.
    """
    self.warps_from = []
    self.warps_to = []
    start = 0xf3d8
    for i in range(51):
      self.warps_from.append(self.rom_data[start:start+3])
      start += 3
    for i in range(51):
      self.warps_to.append(self.rom_data[start:start+3])
      start += 3
    
  def shuffle_warps(self):
    """
    Shuffles the map warps (towns and caves).
    """
    cave_end = 8 if self.generated else 7 
    caves = [self.warps_from[x] for x in self.cave_warps[:cave_end]]
    towns = [self.warps_from[x] for x in self.town_warps]
    random.shuffle(caves)
    random.shuffle(towns)
    while not self.generated and (tuple(caves[1]) == (1, 108, 109) or
           (tuple(towns[4]) == (1, 102, 72) and caves[1][0] in (4, 9)) or
           (tuple(towns[0]) == (1, 102, 72) and caves[1][0] == 9)):
      random.shuffle(caves)
    # save the shuffling
    for i in range(cave_end):
      self.warps_from[self.cave_warps[i]] = caves[i]
    if not self.generated: # don't shuffle towns if this is a generated map.
      for i in range(6):
        self.warps_from[self.town_warps[i]] = towns[i]
    self.update_warps()

  def update_warps(self):
    # update with the new warps
    warp_data = []
    for i in range(20):
      warp_data += self.warps_from[i]
    self.add_patch(0xf3d8, warp_data)
#    warp_data = []
#    for i in range(20):
#      warp_data += self.warps_to[i]
#    self.add_patch(0xf471, warp_data)

    # update gwaelin's love calculation code
    self.add_patch(self.love_calc_slice, self.warps_from[self.tantegel_warp][1:3])

    #update the return point
    for i in range(3):
      self.add_patch(self.return_point_addr[i], self.return_point[i])

    # update the rainbow bridge location and trigger
    if self.rainbow_bridge:
      rainbow = self.rainbow_bridge[:]
      self.add_patch(self.rainbow_bridge_slice, rainbow)
      rainbow[1] += 1
      self.add_patch(self.rainbow_drop_slice, rainbow)


  def commit(self):
    """
    Deprecated. Left in for compatibility.
    """
    pass
    # update the fixed encounters
    # these aren't changed here yet, so let's comment them out for now
#    if self.axe_knight:
#      self.add_patch(self.axe_knight_slice, self.axe_knight)
#    if self.green_dragon:
#      self.add_patch(self.green_dragon_slice, self.green_dragon)
#    if self.golem:
#      self.add_patch(self.golem_slice, self.golem)

  def add_patch(self, addr, values):
    if not isinstance(addr, slice):
      self.patch.add_record(addr, values)
    elif addr.step == 1:
      self.patch.add_record(addr.start, values)
    else:
      for i in range(len(values)):
        self.patch.add_record(addr.start + i * addr.step, values[i])
    
    
  def to_html(self, output="-"):
    """
    Outputs the map to HTML format for viewing.

    :Parameters:
      map_grid : array
        An array of integers indicating the tiles of the map.

    """
    html =("<html><head><style type='text/css' rel='stylesheet'>"
           "td { width: 16px; height: 16px; } "
           ".grass { background: no-repeat center center url(data:image/gif;base64,R0lGODlhEAAQAIQWAAAAAIwQAOAAXABc/0xgfAB8/wCsAP88AOBcEHx8fHyQrDy8//98XLCcgFzcUP+gQLy8vMDAwLz/HP/cfLz/vP/grP///////////////////////////////////////yH+EUNyZWF0ZWQgd2l0aCBHSU1QACwAAAAAEAAQAAAFJKAkisZoliNqSurqvm6bkq8M33OM72fd67wgyzcECm+24nEUAgA7); } "
           ".desert { background: no-repeat center center url(data:image/gif;base64,R0lGODlhEAAQAIQWAAAAAIwQAOAAXABc/0xgfAB8/wCsAP88AOBcEHx8fHyQrDy8//98XLCcgFzcUP+gQLy8vMDAwLz/HP/cfLz/vP/grP///////////////////////////////////////yH+EUNyZWF0ZWQgd2l0aCBHSU1QACwAAAAAEAAQAAAFJuA0IWIpkmZ6qmOKuuYbs3Rdyveq4vDe/zVea+bLFW1IYk8oNP5CADs=); } "
           ".hill { background: no-repeat center center url(data:image/gif;base64,R0lGODlhEAAQAKEDAAAAAJjoAP/YoP///yH+EUNyZWF0ZWQgd2l0aCBHSU1QACwAAAAAEAAQAAACO4yPB7gZ7aJkoDID4ZtLGK8pgeB920UiaTYCXmkuruB+9LjS+kPePV+buSxC4rBXITUqsuTQImGGJo4CADs=); } "
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
    y=0
    for row in self.grid:
      html += "<tr>"
      x=0
      for tile in row:
        title = ''
        tile_name = self.tiles[tile]
        if tile_name in ('castle', 'town', 'cave'):
          try:
            title = 'title="%s"' % MAPS[self.warps_to[
                self.warps_from.index(struct.pack('BBB', 1, x, y))][0]]
          except ValueError:
            pass
        html += ("<td class='%s' %s></td>" % (self.tiles[tile], title))
        x += 1
      html += "</tr>"
      y += 1 
    html += "</table></body></html>"

    if output == "-":
      sys.stdout.write(html)
    elif output:
      with open(output, 'w') as f:
        f.write(html)
    return html

class MapGrid(pathfinding.SquareGrid):

  def __init__(self, grid):
    super(MapGrid, self).__init__(len(grid[0]), len(grid))
    self.grid = grid

  def passable(self, id):
    """
    Whether or not the given tile is passable by walking.
    """
    x, y = id
    return self.grid[y][x] not in IMPASSABLE

  def cost(self, *args):
    """
    Returns the cost to move from one space to the next
    """
    return 1

class SanityError(Exception):
  """
  An error to be thrown when the new map fails a sanity check.
  """
  def __init__(self, message):
    super(SanityError, self).__init__(message)


if __name__ == "__main__":
  # Run this code if this module is called instead of imported
  parser = argparse.ArgumentParser(prog="DWRandomizer",
      description="A randomizer for Dragon Warrior for NES")
  parser.add_argument("--test", action="store_true",
      help="Generate and export a test map.")
  parser.add_argument("-o", "--output", default="map.html",
      help="The output file. Defaults to 'map.html'")
           
  parser.add_argument("filename", help="The rom file to use for input")
  args = parser.parse_args()

  
#  world_map.from_rom(args.filename)
  f = open(args.filename, 'rb')
  rom_data = f.read()
  f.close()
  world_map = WorldMap(rom_data)
  if args.test:
    while not world_map.generate():
      print("Error: " + str(world_map.error) + ", retrying...")
      world_map.revert()
  world_map.to_html(args.output)


