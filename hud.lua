
debug = {
	hud = true,
  cheats_enabled = true,
  cheats = function()
    player:set_experience(65535)
    player:set_gold(65535)
    memory.writebyte(0xbe, 255) -- best equipment
    memory.writebyte(0xbf, 6)   -- 6 herbs
    memory.writebyte(0xc0, 6)   -- 6 keys
    memory.writebyte(0xc1, 14)  -- rainbow drop
  end 
}

features = {
  herb_store = true, -- buy herbs anywhere
  enemy_hp = true,  -- show enemy hit points
  grind_mode = true,
  repulsive = true, -- no lower level enemy encounters
  autonav = true -- automatic navigation
}

levels = {}

TILE = {
  GRASS = 0,
  DESERT = 1,
  HILLS = 2,
  STAIRS_UP = 3,
  TILES = 4,
  STAIRS_DOWN = 5,
  SWAMP = 6,
  TOWN = 7,
  CAVE = 8,
  CASTLE = 10,
  BRIDGE = 11,
  FOREST = 12,
  CHEST = 13,
  FORCE_FIELD = 14
}

tile_names = {
  "Grass",
  "Desert",
  "Hill",
  "Stairs Up",
  "Brick",
  "Stairs Down",
  "Swamp",
  "Town",
  "Cave",
  "Castle",
  "Bridge",
  "Forest",
  "Chest",
  "Barrier",
}

maps = {
  "Alefgard",
  "Charlock",
  "Hauksness",
  "Tantegel",
  "Tantegel Throne Room",
  "Charlock Throne Room",
  "Kol",
  "Brecconary",
  "Garinham",
  "Cantlin",
  "Rimuldar",
  "Tantegel Basement",
  "Northern Shrine",
  "Southern Shrine",
  "Charlock Cave Lv 1",
  "Charlock Cave Lv 2",
  "Charlock Cave Lv 3",
  "Charlock Cave Lv 4",
  "Charlock Cave Lv 5",
  "Charlock Cave Lv 6",
  "Swamp Cave",
  "Mountain Cave",
  "Mountain Cave Lv 2",
  "Garin's Grave Lv 1",
  "Garin's Grave Lv 2",
  "Garin's Grave Lv 3",
  "Garin's Grave Lv 4",
  "Erdrick's Cave",
  "Erdrick's Cave Lv 2"
}

strings = {
  encounter = {
    "A %s draws near!",
    "Hark! A %s approacheth!",
    "What is that I see? Why it's a %s!",
    "I have no quarrel with you Sir %s, but I must pass.",
    "You got something for me %s? Bring it!"
  },
  enemyrun = {
    "I suppose he didst not likest me much.",
    "Comest back here thee coward!",
    "Oh running away eh? Come back here and fight me! I'll bite your legs off!",
    "Oh comest on! Why did thee even comest over if thee were just to runnest away?",
    "It was in quite a hurry.",
    "That's right! You get on down the road and don't come back!",
    "Fine, just goest. Leavest me liketh everyone else. Well, excepteth that clingy princess.",
    "Wait what just happened? That %s just disappeared!"
  },
  playerrun = {
    "Whew! That was scary!",
    "I have made my escape from the vile %s",
    "I just felt like running. So I did.",
    "You can't catch me son! Don't even try!",
    "It was a fierce beast! With big fangs! What? Didn't you see the bones?",
    "I think I soiled my armor.",
    "Who, me? I wasn't running from it, I just had somewhere to be...that wasn't anywhere near that %s."
  },
  enemydefeat = {
    "I hath done well in defeating the %s.",
    "Oh! You got wrecketh bro!",
    "Thou just stayeth down there chump!",
    "There ain't gonna be no rematch!",
    "Did you see that? There were like 20 of them! No match for me though.",
    "That was easy.",
    "Seriously? That's all you got? Where's the REAL challenger?",
    "You can't touch this.",
    "I attack the %s! It's super effective!",
    "Who has two thumbs and just put thee down? This guy!",
    "Who's the hero? Who is the hero? That's right, I'm the hero."

  },
  lowhp = {
    "Uuungh... I don't feel so well",
    "MEDIC!",
    "I'm kinda losing a lot of blood here...",
    "'Tis but a scratch!",
    "Oh, it's just a flesh wound.",
    "It is getting dark...I am not long for this world! Oh, nevermind, my helmet was on backward.",
    'So what are "hit points" anyway? More is better, right?'
  },
  playerdefeat = {
    "Ouch.",
    "That really hurt, Charlie!",
    "Oy! Gevalt!",
    "I'll be back.",
    "I'm not quite dead yet. Oh, nevermind, yes I am.",
    "Oh great king! A little help here please!",
    "Well that didn't go quite like I envisioned it.",
    "Thou are at fault for this! Thou! Thou! Not I! Thou art a terrible tactician!",
    "They telleth me to goest toward the light..."
  }
}



function say(str) 
  print(string.format("%s\n", str))
end

function battle_message(strings, enemy_type)
--  if enemy_type == nil then
--    say(strings[math.random(#strings)])
--  else
--    local enemy_name = enemy.types[enemy_type]
--    if enemy_name ~= nil then
--      say(strings[math.random(#strings)]:format(enemy_name))
--    end
--  end
end


in_battle = false
pre_battle = false
function battle_mode (battling, reset_enemy_hp)
  if battling ~= nil then
    in_battle = battling
    pre_battle = battling
    player.valid_tile = false
    if not battling then
      if (reset_enemy_hp == true) then
        -- overwrite monster hp to avoid confusion
        enemy:set_hp(0)
      end
    end
  end
  return in_battle
end

Player = {
  level = 0,
  hp = 0,
  mp = 0,
  gold = 0,
  experience = 0,
  change = {
    level = 0,
    hp = 0,
    mp = 0,
    gold = 0,
    experience = 0
  },
  herbs = 0,
  keys = 0,
  map_x = 0,
  map_y = 0,
--   player_x = 0,
--   player_y = 0,
--   current_map = 0,
  tile = 0,
  last_tile = 0,
  valid_tile = false,

  last_command = 0,
  grind_action = 0,
  quiet = false,
  mode = {
    grind = false,
    auto_battle = false,
    fraidy_cat = false,
    explore = false,
    manual = true
  },
  path = nil,
  path_pointer = 1,
  destination = nil,
  destination_commands = nil,
  destination_callback = nil
}

function Player.update (self)

  --level cap
  if features.level_cap ~= nil then
    local exp_cap = self.levels[features.level_cap]
    if self:get_experience() > exp_cap then
      self:set_experience(exp_cap)
    end
  end

  -- read in the values from memory.
  local level = self:get_level()
  local hp = self:get_hp()
  local mp = self:get_mp()
  local gold = self:get_gold()
  local experience = self:get_experience()

  -- update the changes.
  self.change.level = level - self.level
  self.change.hp = hp - self.hp
  self.change.mp = mp - self.mp
  self.change.gold = gold - self.gold
  self.change.experience = experience - self.experience

  -- update the object variables.
  self.level = level
  self.hp = hp
  self.mp = mp
  self.gold = gold
  self.experience = experience

  self.herbs = self.get_herbs()
  self.keys = self:get_keys()

  self.last_tile = self.tile
  self.tile = self:get_tile()
  local map_x = self:get_x()
  local map_y = self.get_y()
  if (map_x ~= self.map_x or map_y ~= self.map_y) then
    if in_battle then
      battle_mode(false, true)
    end
    self.valid_tile = true
  end
  if (self.current_map ~= self.get_map()) then
    self.valid_tile = false
  end
  self.map_x = map_x
  self.map_y = map_y
--   self.player_x = memory.readbyte(0x3a)
--   self.player_y = memory.readbyte(0x3b)
   self.current_map = self:get_map()

  if hp == 0 and self.change.hp ~= 0 and player:get_map() > 0 then
    battle_message(strings.playerdefeat, player:get_tile()+1)
  end

  -- update grind mode if needed
  if features.grind_mode and not player.mode.grind and 
    emu.framecount() - player.last_command > 36000 then
    player:set_mode("grind")
  end

end

function Player.get_tile(self)
  return memory.readbyte(0xe0)
end

function Player.get_x(self)
  return memory.readbyte(0x8e)
end

function Player.get_y(self)
  return memory.readbyte(0x8f)
end

function Player.get_herbs(self)
  return memory.readbyte(0xc0)
end

function Player.get_keys(self)
  return memory.readbyte(0xbf)
end

function Player.get_level(self)
  return memory.readbyte(0xc7)
end

function Player.next_level(self)
  if self:get_level() == features.level_cap then
    return 0
  end
  for i=1,#self.levels do
    if self.levels[i] > self:get_experience() then
      return self.levels[i] - self:get_experience()
    end
  end
  return 0
end

function Player.get_hp(self)
  return memory.readbyte(0xc5)
end

function Player.max_hp(self)
  return memory.readbyte(0xca)
end

function Player.get_mp(self)
  return memory.readbyte(0xc6)
end

function Player.add_hp (self, amount)
  return self:set_hp(self.hp + amount)
end

function Player.set_hp (self, amount)
  if (amount > 255 or amount < 0) then
    return false
  end
  self.hp = amount
  memory.writebyte(0xc5, amount)
  return true
end

function Player.max_mp(self)
  return memory.readbyte(0xcb)
end

function Player.get_map(self)
  return memory.readbyte(0x45)
end

function Player.add_mp (self, amount)
  return self:set_mp(self.mp + amount)
end

function Player.set_mp (self, amount)
  if (amount > 255 or amount < 0) then
    return false
  end
  self.mp = amount
  memory.writebyte(0xc6, amount)
  return true
end

function Player.get_gold(self)
  return memory.readbyte(0xbd) * 256 + memory.readbyte(0xbc)
end

function Player.add_gold (self, amount)
  return self:set_gold(self.gold + amount)
end

function Player.set_gold (self, amount)
  if (amount > 65535 or amount < 0) then
    return false
  end
  self.gold = amount
  memory.writebyte(0xbd, amount / 256)
  memory.writebyte(0xbc, amount % 256)
  return true
end

function Player.get_experience(self)
  return memory.readbyte(0xbb) * 256 + memory.readbyte(0xba)
end

function Player.add_experience (self, amount)
  return self:set_experience(self.experience + amount)
end

function Player.set_experience (self, amount)
  if (amount > 65535 or amount < 0) then
    return false
  end
  self.experience = amount
  memory.writebyte(0xbb, self.experience / 256)
  memory.writebyte(0xba, self.experience % 256)
end

function Player.add_herb (self)
  self.herbs = memory.readbyte(0xc0)
  if self.herbs >= 6 then
    return false
  end
  memory.writebyte(0xc0, self.herbs + 1)
  return true
end

function Player.cancel(self, c)
  if c == nil then
    c = 3
  end
  for j=1,c do
    pressb()
  end
end

function Player.check_cursor(self, x, y)
  return memory.readbyte(0xd8) == x and memory.readbyte(0xd9) == y
end

function Player.down(self, c)
  if c == nil or c < 1 then 
    c = 1
  end
  local input = {}
  input.down = true
  for j=1,c do
    local starty = memory.readbyte(0x8f)
    local startcursor = memory.readbyte(0xd9)
    for i=1,45 do
      joypad.set(1, input)
      emu.frameadvance()
      if memory.readbyte(0x8f) > starty or memory.readbyte(0xd9) > startcursor then
        break
      end
    end
    -- we were unable to move, give up
    if memory.readbyte(0x8f) == starty and memory.readbyte(0xd9) == startcursor then
      return false
    end
  end
  return true
end

function Player.up(self, c)
  if c == nil or c < 1 then 
    c = 1
  end
  local input = {}
  input.up = true
  for j=1,c do
    local starty = memory.readbyte(0x8f)
    local startcursor = memory.readbyte(0xd9)
    for i=1,45 do
      joypad.set(1, input)
      emu.frameadvance()
      if memory.readbyte(0x8f) < starty or memory.readbyte(0xd9) < startcursor then
        break
      end
    end
    -- we were unable to move, give up
    if memory.readbyte(0x8f) == starty and memory.readbyte(0xd9) == startcursor then
      return false
    end
  end
end

function Player.left(self, c)
  if c == nil or c < 1 then 
    c = 1
  end
  local input = {}
  input.left = true
  for j=1,c do
    local startx = memory.readbyte(0x8e)
    local startcursor = memory.readbyte(0xd8)
    for i=1,45 do
      joypad.set(1, input)
      emu.frameadvance()
      if memory.readbyte(0x8e) < startx or memory.readbyte(0xd8) < startcursor then
        break
      end
    end
    -- we were unable to move, give up
    if memory.readbyte(0x8e) == startx and memory.readbyte(0xd8) == startcursor then
      return false
    end
  end
end

function Player.right(self, c)
  if c == nil or c < 1 then 
    c = 1
  end
  local input = {}
  input.right = true
  for j=1,c do
    local startx = memory.readbyte(0x8e)
    local startcursor = memory.readbyte(0xd8)
    for i=1,45 do
      joypad.set(1, input)
      emu.frameadvance()
      if memory.readbyte(0x8e) > startx or memory.readbyte(0xd8) > startcursor then
        break
      end
    end
    -- we were unable to move, give up
    if memory.readbyte(0x8e) == startx and memory.readbyte(0xd8) == startcursor then
      return false
    end
  end
end

function Player.talk(self) 
  self:cancel()
  pressa(2)
  wait(30)
  if not self:check_cursor(0,0) then
    self:cancel()
    return false
  end
  pressa(2)
  return true
end

function Player.status(self) 
  self:cancel()
  pressa(2)
  wait(30)
  self:down(1)
  wait(6)
  if not self:check_cursor(0,1) then
    self:cancel()
    return false
  end
  pressa(2)
  return true
end


function Player.stairs(self) 
  self:cancel()
  pressa(2)
  wait(30)
   self:down(2)
  wait(6)
  if not self:check_cursor(0,2) then
    self:cancel()
    return false
  end
  pressa(2)
  return true
end


function Player.search(self) 
  self:cancel()
  pressa(2)
  wait(30)
  self:down(3)
  wait(6)
  if not self:check_cursor(0,3) then
    self:cancel()
    return false
  end
  pressa(2)
  return true
end


function Player.spell(self, c) 
  self:cancel()
  if not in_battle then
    pressa(2)
  end
  wait(30)
  self:right()
  wait(6)
  if not self:check_cursor(1,0) then
    self:cancel()
    return false
  end
  pressa(2)
  wait(30)
  if c ~= nil then
    for j=1,c-1 do
      self:down()
    end
    if not self:check_cursor(0,c-1) then
      self:cancel()
      return false
    end
    wait(6)
    pressa(2)
  end
  return true
end


function Player.item(self, c) 
  self:cancel()
  if not in_battle then
    pressa(2)
  end
  wait(30)
  self:down()
  self:right()
  wait(6)
  if not self:check_cursor(1,1) then
    self:cancel()
    return false
  end
  pressa(2)
  wait(30)
  if c ~= nil then
    for j=1,c-1 do
      self:down()
    end
    if not self:check_cursor(0,c-1) then
      self:cancel()
      return false
    end
    wait(6)
    pressa(2)
  end
  return true
end


function Player.door(self) 
  self:cancel(5)
  pressa(2)
  wait(30)
  self:down(2)
  self:right()
  wait(6)
  if not self:check_cursor(1,2) then
    self:cancel()
    return false
  end
  pressa(2)
  return true
end


function Player.take(self) 
  self:cancel()
  pressa(2)
  wait(30)
  self:down(3)
  self:right()
  wait(6)
  if not self:check_cursor(1,3) then
    self:cancel()
    return false
  end
  pressa(2)
  return true
end

function Player.fight(self) 
  self:cancel()
  if in_battle then
    wait(30)
    pressa(2)
    return true
  end
  return false
end

function Player.run(self) 
  self:cancel()
  if in_battle then
    wait(30)
    self:down()
    wait(6)
    if not self:check_cursor(0,1) then
      self:cancel()
      return false
    end
    pressa(2)
    return true
  end
  return false
end

function Player.heal(self)
  if (AND(memory.readbyte(0xce), 0x1) > 0) then
    if self.mp < 4 then
      say("I do not have enough magic to cast heal")
      return false
    end
    self:spell(1)
  else
    say("I do not yet have the heal spell")
    return false
  end 
  return true
end

function Player.hurt(self)
  if (AND(memory.readbyte(0xce), 0x2) > 0) then
    if not in_battle then
      say("Hurt is a battle spell. I am not fighting anything!")
      return false
    end
    self:spell(2)
  else
    say("I do not yet have the hurt spell")
    return false
  end 
  return true
end

function Player.sleep(self)
  if (AND(memory.readbyte(0xce), 0x4) > 0) then
    if not in_battle then
      say("Sleep is a battle spell. I am not fighting anything!")
      return false
    end
    self:spell(3)
  else
    say("I do not yet have the sleep spell")
    return false
  end 
  return true
end

function Player.radiant(self)
  if (AND(memory.readbyte(0xce), 0x8) > 0) then
    self:spell(4)
  else
    say("I do not yet have the radiant spell")
    return false
  end 
  return true
end

function Player.stopspell(self)
  if (AND(memory.readbyte(0xce), 0x10) > 0) then
    if not in_battle then
      say("Stopspell is a battle spell. I am not fighting anything!")
      return false
    end
    self:spell(5)
  else
    say("I do not yet have the stopspell spell")
    return false
  end 
  return true
end

function Player.outside(self)
  if (AND(memory.readbyte(0xce), 0x20) > 0) then
    self:spell(6)
  else
    say("I do not yet have the outside spell")
    return false
  end 
  return true
end

function Player.return_(self)
  if (AND(memory.readbyte(0xce), 0x40) > 0) then
    self:spell(7)
  else
    say("I do not yet have the return spell")
    return false
  end 
  return true
end

function Player.repel(self)
  if (AND(memory.readbyte(0xce), 0x80) > 0) then
    self:spell(8)
  else
    say("I do not yet have the repel spell")
    return false
  end 
  return true
end

function Player.healmore(self)
  if (AND(memory.readbyte(0xcf), 0x1) > 0) then
    if self.mp < 10 then
      say("I do not have enough magic to cast healmore")
      return false
    end
    self:spell(9)
  else
    say("I do not yet have the healmore spell")
    return false
  end 
  return true
end

function Player.hurtmore(self)
  if (AND(memory.readbyte(0xcf), 0x2) > 0) then
    if not in_battle then
      say("Hurtmore is a battle spell. I am not fighting anything!")
      return false
    end
    self:spell(10)
  else
    say("I do not yet have the hurtmore spell")
    return false
  end 
  return true
end

function Player.herb (self)
  if self.herbs > 0 then
    self:item(1)
    return true
  end
  if features.herb_store then
    if (self:add_gold(-self.level * self.level)) then 
      local quiet = self.quiet
      self.quiet = false
      say(("I purchased an herb for %dG"):format(self.level * self.level))
      self.quiet = quiet
      self:add_herb()
      self:item(1)
      return true
    end
  end
  if features.herb_store then
    say("I don't have any herbs, and I'm broke fool!")
  else
    say("I don't have any herbs.")
  end
  return false
end

function Player.grind_move(self)
  if self:mode_autonav() then
    return self:follow_path(true)
  end
  if not in_battle then
    self.grind_action = (self.grind_action + 1) % 4
  end
  if self.grind_action == 0 then
    return self:up()
  elseif self.grind_action == 1 then
    return self:left()
  elseif self.grind_action == 2 then
    return self:down()
  else
    return self:right()
  end
end

function Player.grind(self) 
  if not in_battle and self.mode.grind then
    if self:heal_thy_self() then
      wait(120)
    end
    if not self:grind_move() then
      self:cancel() -- maybe we're in a menu?
    end
  end
  if in_battle then
    if self.mode.grind or self.mode.auto_battle then
      if self:heal_thy_self() then
        wait(120)
      end
      self:fight()
      self:cancel() -- in case the enemy runs immediately
      wait(15)
      self:grind_move()
      wait(200)
    end
    if self.mode.fraidy_cat then
      if not self:heal_thy_self() then
        self:run()
      end
      wait(200)
    end
  end
end


function Player.set_mode(self, mode)
  if mode == "autobattle" then
    self.mode.grind = false
    self.mode.auto_battle = true
    self.mode.fraidy_cat = false
    self.mode.explore    = false
  elseif mode == "fraidycat" then
    self.mode.grind = false
    self.mode.auto_battle = false
    self.mode.fraidy_cat = true
    self.mode.explore    = false
  elseif mode == "manual" then
    self.mode.grind = false
    self.mode.auto_battle = false
    self.mode.fraidy_cat = false
    self.mode.explore    = false
    self.destination = nil
    self.path = nil
    self.destination_commands = nil
    self.destination_callback = nil
  elseif features.grind_mode and mode == "grind" then
    self.mode.grind = true
    self.mode.auto_battle = false
    self.mode.fraidy_cat = false
    self.mode.explore    = false
  end
end

function Player.heal_thy_self(self)
  self.quiet = true
  local returnvalue = false
  if self:get_hp() * 3 < self:max_hp() then
    battle_message(strings.lowhp)
    if self:healmore() then
      returnvalue = true
    elseif self:heal() then 
      returnvalue = true
    elseif self:herb() then
      returnvalue = true
    end
  elseif not in_battle and self:get_hp() + 30 < self:max_hp() and (self:get_level() < 17) then
    if self:heal() then
      returnvalue = true
    elseif self:herb() then
      returnvalue = true
    end
  end
  self.quiet = false
  return returnvalue
end

Enemy = {
  hp = 0,
  change = {
    hp = 0
  },
  types = {
    "Slime",  -- 0
    "Red Slime",
    "Drakee",
    "Ghost",
    "Magician",
    "Magidrakee", -- 5
    "Scorpion",
    "Druin",
    "Poltergeist",
    "Droll",
    "Drakeema",  --10
    "Skeleton",
    "Warlock",
    "Metal Scorpion",
    "Wolf",
    "Wraith",  --15
    "Metal Slime",
    "Specter",
    "Wolflord",
    "Druinlord",
    "Drollmagi",  --20
    "Wyvern",
    "Rogue Scorpion",
    "Wraith Knight",
    "Golem",
    "Goldman",  -- 25
    "Knight",
    "Magiwyvern",
    "Demon Knight",
    "Werewolf",
    "Green Dragon",  -- 30
    "Starwyvern",
    "Wizard",
    "Axe Knight",
    "Blue Dragon",
    "Stoneman", --35
    "Armored Knight",
    "Red Dragon",
    "Dragonlord",  --first form
    "Dragonlord"  --second form

  }
}

function Enemy.update (self)
  -- read in the values from memory.
  hp = memory.readbyte(0xe2)
  -- update the changes.
  self.change.hp = hp - self.hp
  -- update the object variables.
  self.hp = hp

  -- update battle status
  if not in_battle and self.change.hp ~= 0 then
    battle_mode(true, false)
  end

  -- hit points wrap below zero, so check for large increases.
  if in_battle and (self.hp == 0 or self.change.hp > 160) then
    battle_mode(false, false)
    battle_message(strings.enemydefeat, player:get_tile()+1)
  end

end

function Enemy.show_hp (self)
  if (in_battle and features.enemy_hp) then 
    gui.drawbox(152, 134, 190, 144, "black")
    gui.text(154, 136, string.format( "HP %3d", self.hp), "white", "black")
  end
end

function Enemy.set_hp(self, hp)
  if (hp > 255 or hp < 0) then
    return false
  end
  self.hp = hp
  memory.writebyte(0xe2, hp)
  return true
end

function Enemy.hud (self)
  enemy_index = memory.readbyte(0xe0)
  enemy_type = enemy.types[enemy_index+1]
  lbound = 186
  if enemy_type ~= nil then
    gui.drawbox(lbound, 64, 260, 144, "black")
    gui.text(lbound, 64, enemy_type, "white", "black")
    gui.text(lbound, 74, string.format( "HP %3d", self.hp), "white", "black")
    gui.text(lbound, 84, 
      string.format("Str %3d", rom.readbyte(0x5e5b + enemy_index * 16))
      , "white", "black")
    gui.text(lbound, 94, 
      string.format("Agi %3d", rom.readbyte(0x5e5c + enemy_index * 16))
      , "white", "black")
    -- show the special attacks
    specialset1 = { "Sleep", "Stopspell", "Heal", "Healmore" }
    specialset2 = { "Hurt", "Hurtmore", "fire", "fire 2" }
    specials = rom.readbyte(0x5e5e + enemy_index * 16)
    ypos = 104
    spectype = specialset1[math.floor(specials / 64) + 1]
    specchance = math.floor(specials / 16 % 4) * 25
    if specchance > 0 then
      gui.text(lbound, ypos, string.format("%s (%2d%%)", spectype, specchance), 
        "white", "black")
      ypos = ypos + 10
    end
    spectype = specialset2[math.floor((specials / 4) % 4) + 1]
    specchance = (specials % 4) * 25
    if specchance > 0 then
      gui.text(lbound, ypos, string.format("%s (%2d%%)", spectype, specchance), 
        "white", "black")
    end
  end
end
-- 
--  Draws any hud elements, such as the enemy hit points
--  and debug info
-- 
function overlay()
  if features.grind_mode and player.mode.grind then
    gui.drawbox(0, 0, 60, 15, "black")
    gui.text(8, 8, "Grind mode", "white", "black")
  end
  if player.mode.auto_battle then
    gui.drawbox(0, 0, 82, 15, "black")
    gui.text(8, 8, "Auto-battle mode", "white", "black")
  end
  if player.mode.fraidy_cat then
    gui.drawbox(0, 0, 82, 15, "black")
    gui.text(8, 8, "Fraidy-cat mode", "white", "black")
  end

  if debug.hud then
    current_map = maps[memory.readbyte(0x45)]
    if current_map ~= nil then
      gui.drawbox(0, 0, 120, 25, "black")
      gui.text(8, 8, 
        string.format("%s (%3d,%3d)", current_map, memory.readbyte(0x8e), 
          memory.readbyte(0x8f)), 
        "white", "black")
      if in_battle then
        enemy:hud()
      else
        tile = tile_names[memory.readbyte(0xe0) + 1]
        if player.valid_tile and tile ~= nil then
          gui.text(8, 18, 
            string.format( "Tile:  %s", tile), "white", "black")
        end
      end
    end
  else
    enemy:show_hp()
  end
end

function read_levels()
  for i=1,31 do
    levels[i] = rom.readbyte(0xf36c+(i*2)) * 256 + rom.readbyte(0xf36b+(i*2))
  end
end

function update()
  if (player:get_map() > 0) then
    -- update the player and enemy info every 1/4 second
    if (emu.framecount() % 15 == 0) then
      player:update()
      enemy:update()
    end

    -- down + select => grind mode
    if memory.readbyte(0x47) == 36 then
      player.last_command = 0
    end

    if features.repulsive then
      memory.writebyte(0xdb, 0xff)
    end
  end
end


-- for battle detection (enemies or you running away)
function running(address)
  if (player:get_map() > 0) then
    if address == 0xefc8 then
      battle_message(strings.enemyrun, player:get_tile()+1)
    else
      battle_message(strings.playerrun, player:get_tile()+1)
    end
    battle_mode(false, true)
  end
end

-- A thing draws near!
function encounter(address)
  if (player:get_map() > 0) then
    player.valid_tile = false
    battle_message(strings.encounter, memory.readbyte(0x3c)+1)
    pre_battle = true
  end
end

function cheats()
  if debug.cheats_enabled and debug.cheats then
    if (memory.readbyte(0x45) == 1) then
      debug.cheats()
      debug.cheats = false
    end
  end
end


-- main loop
function main()
  -- savestate.load(savestate.object(1))
  memory.registerexecute(0xefc8, running)
  memory.registerexecute(0xe8a4, running)
  memory.registerexecute(0xcf44, encounter)
  memory.registerwrite(0x45, cheats)
  read_levels()

  player = Player
  enemy = Enemy
  player.last_command = emu.framecount()
  enemy:update()
  player:update()
  gui.register(overlay)
  emu.registerafter(update)

  while(true) do
    if player:get_hp() > 0 and player:get_map() > 0 then
      player:grind()
    end
    emu.frameadvance()
  end
end

main()
