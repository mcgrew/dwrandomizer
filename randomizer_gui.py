#!/usr/bin/env python3

from tkinter import *
from tkinter import filedialog
import dwrandomizer
import random
import os
import os.path

HOME=os.path.expanduser('~' + os.sep)

class RandomizerUI(Frame):
  def __init__(self, master=None):
    Frame.__init__(self, master)
    self.columnconfigure(0, weight=1)
    self.columnconfigure(1, weight=1)
    self.createWidgets()
    self.grid()
    self.args = dwrandomizer.parse_args()

  def createWidgets(self):
    self.rom_frame = RomFrame(self)
    self.rom_frame.grid(column=0, row=0, sticky="NSEW")
    self.output_frame = OutputFrame(self)
    self.output_frame.grid(column=0, row=1, sticky="NSEW")
    self.seed_frame = SeedFrame(self)
    self.seed_frame.grid(column=1, row=0, sticky="NSEW")
    self.flags_frame = FlagsFrame(self)
    self.flags_frame.grid(column=1, row=1, sticky="NSEW")
    self.toggle_frame = ToggleFrame(self)
    self.toggle_frame.grid(column=0, row=2, columnspan=2,sticky="NSEW")
    self.level_frame = LevelingFrame(self, "Leveling Speed", 'f', 0, 3)
    self.growth_frame = UltraFrame(self, "Growth Type", 'g',  0, 4)
    self.spell_frame = UltraFrame(self, "Spell Learning", 's', 0, 5)
    self.attack_frame = UltraFrame(self, "Enemy Attack Patterns", 'p', 1, 3)
    self.zone_frame = UltraFrame(self, "Enemy Zone Randomization", 'z', 1, 4)
    self.randomize_button = Button(self, text="Randomize!", command=self.execute)
    self.randomize_button.grid(column=1, row=6, sticky="E")
    self.toggle_frame.from_flags(self.flags_frame.tk_flags.get())

  def execute(self):
    pass

# ==============================================================================

class RomFrame(LabelFrame):
  def __init__(self, master=None, initial_file='', initial_dir=HOME):
    LabelFrame.__init__(self, master, text="Rom File")
    self.tk_filename = StringVar()
    self.createWidgets()

  def createWidgets(self):
    self.file_name_entry = Entry(self, textvariable=self.tk_filename)
    self.file_open_button = Button(self, text="Choose", command=self.choose_file)
    self.file_name_entry.grid(column=0, row=0, sticky="NSEW")
    self.file_open_button.grid(column=1, row=0, sticky="E")

  def choose_file(self):
    self.set(filedialog.askopenfilename(defaultextension='.nes', 
      filetypes=[('NES ROMs', '.nes'), ('All files', '.*')],
      initialdir=HOME
    ))

  def set(self, filename):
    self.tk_filename.set(filename)

  def get(self):
    return self.tk_filename.get()

# ==============================================================================

class OutputFrame(LabelFrame):
  def __init__(self, master=None, initial_dir=HOME):
    LabelFrame.__init__(self, master, text="Ouput Folder")
    self.initial_dir = initial_dir
    self.tk_dirname = StringVar()
    self.createWidgets()

  def createWidgets(self):
    self.dir_name_entry = Entry(self, textvariable=self.tk_dirname)
    self.dir_open_button = Button(self, text="Choose", command=self.choose_dir)
    self.dir_name_entry.grid(column=0, row=0, sticky="NSEW")
    self.dir_open_button.grid(column=1, row=0, sticky="E")

  def choose_dir(self):
    self.set(filedialog.askdirectory(initialdir=self.initial_dir))

  def set(self, dirname):
    self.tk_dirname.set(dirname)

  def get(self):
    return self.tk_dirname.get()

# ==============================================================================

class SeedFrame(LabelFrame):
  def __init__(self, master=None):
    LabelFrame.__init__(self, master, text="Seed")
    self.tk_seed = IntVar()
    self.createWidgets()

  def createWidgets(self):
    self.seed_entry = Entry(self, textvariable=self.tk_seed)
    self.seed_entry.grid(column=0, row=0)
    self.random_button = Button(self, text="Random", command=self.random_seed)
    self.random_button.grid(column=1, row=0, sticky="E")

  def random_seed(self):
    self.set(random.randint(0,sys.maxsize))

  def set(self, value):
    self.tk_seed.set(value)

  def get(self):
    return self.tk_seed.get()

# ==============================================================================

class FlagsFrame(LabelFrame):
  def __init__(self, master=None):
    LabelFrame.__init__(self, master, text="Flags")
    self.tk_flags = StringVar(value="AICTZPWGfM")
    self.createWidgets()

  def createWidgets(self):
    self.flag_entry = Entry(self, textvariable=self.tk_flags)
    self.flag_entry.grid(column=0, row=0, sticky="NSEW")

  def set(self, flags):
    self.tk_flags.set(flags)

  def get(self):
    return self.tk_flags.get()
  
# ==============================================================================
    
class ToggleFrame(LabelFrame):
  def __init__(self, master=None):
    LabelFrame.__init__(self, master, text="Options")
    self.map_var = IntVar()
    self.towns_var = IntVar()
    self.ips_var = IntVar()
    self.chests_var = IntVar()
    self.searchable_var = IntVar()
    self.remake_var = IntVar()
    self.speed_hacks_var = IntVar()
    self.death_necklace_var = IntVar()
    self.createWidgets()

  def createWidgets(self):
    self.map_button   = \
      Checkbutton(self,text="Generate A New Map", variable=self.map_var)
    self.towns_button = \
      Checkbutton(self, text="Randomize Towns", variable=self.towns_var)
    self.ips_button   = \
      Checkbutton(self, text="Generate IPS Patch File", variable=self.ips_var)
    self.chests_button = \
      Checkbutton(self, text="Shuffle Chests", variable=self.chests_var)
    self.searchable_button = \
      Checkbutton(self, text="Shuffle Searchable Items", 
          variable=self.searchable_var)
    self.remake_button = \
      Checkbutton(self, text="Use Remake Enemy Stats", variable=self.remake_var)
    self.speed_hacks_button = \
      Checkbutton(self, text="Enable Speed Hacks (experimental)", 
          variable=self.speed_hacks_var)
    self.death_necklace_button = \
      Checkbutton(self, text="Enable Death Necklace Funcionality", 
          variable=self.death_necklace_var)
    self.map_button.grid(           column=0, row=0, sticky="NSW")
    self.towns_button.grid(         column=0, row=1, sticky="NSW")
    self.ips_button.grid(           column=0, row=2, sticky="NSW")
    self.chests_button.grid(        column=1, row=0, sticky="NSW")
    self.searchable_button.grid(    column=1, row=1, sticky="NSW")
    self.remake_button.grid(        column=2, row=0, sticky="NSW")
    self.speed_hacks_button.grid(   column=2, row=1, sticky="NSW")
    self.death_necklace_button.grid(column=2, row=2, sticky="NSW")

  def from_flags(self, flags):
    if 'c' in flags.lower():
      self.chests_var.set(1)
    if 'd' in flags.lower():
      self.death_necklace_var.set(1)
    if 'H' in flags:
      self.speed_hacks_var.set(1)
    if 'i' in flags.lower():
      self.searchable_var.set(1)
    if 'm' in flags.lower():
      self.map_var.set(1)
    if 'r' in flags.lower():
      self.remake_var.set(1)
    if 't' in flags.lower():
      self.towns_var.set(1)


  def flags(self):
    flags = ''
    if self.chests_var.get():
      flags += 'C'
    if self.death_necklace_var.get():
      flags += 'D'
    if self.speed_hacks_var.get():
      flags += 'H'
    if self.searchable_var.get():
      flags += 'I'
    if self.map_var.get():
      flags += 'M'
    if self.remake_var.get():
      flags += 'R'
    if self.towns_var.get():
      flags += 'T'
  

# ==============================================================================

class UltraFrame(LabelFrame):
  def __init__(self, master=None, text='Label', flag='', col=0, row=0):
    LabelFrame.__init__(self, master, text=text)
    self.variable = StringVar(value=text + "_var")
    self.variable.set('ultra')
    self.flag = flag
    self.grid(sticky="NSEW", column=col, row=row)
    self.createWidgets()

  def createWidgets(self):
    none = Radiobutton(self, text="None", variable=self.variable, value="none")
    none.grid(column=0, row=0)
    normal = Radiobutton(self, text="Normal", variable=self.variable, value="normal")
    normal.grid(column=1, row=0)
    ultra = Radiobutton(self, text="Ultra Random", variable=self.variable, value="ultra")
    ultra.grid(column=2, row=0)

  def from_flags(self, flags):
    if flag.upper() in flags:
      self.set('ultra')
    elif flag.lower() in flags:
      self.set('normal')
    else:
      self.set('none')

  def flags(self):
    if self.get() == 'ultra':
      return self.flag.upper()
    if self.get() == 'normal':
      return self.flag.lower()
    return ''


  def set(self, value):
    self.variable.set(value)

  def get(self):
    return self.variable.get()
  
# ==============================================================================

class LevelingFrame(UltraFrame):
  def __init__(self, master=None, text='Label', flag='', col=0, row=0):
    UltraFrame.__init__(self, master, text, flag, col, row)

  def createWidgets(self):
    none = Radiobutton(self, text="Normal", variable=self.variable, value="normal")
    none.grid(column=0, row=0)
    normal = Radiobutton(self, text="Fast", variable=self.variable, value="fast")
    normal.grid(column=1, row=0)
    ultra = Radiobutton(self, text="Very Fast", variable=self.variable, value="veryfast")
    ultra.grid(column=2, row=0)

  def flags(self):
    if self.get() == 'veryfast':
      return self.flag.upper()
    if self.get() == 'fast':
      return self.flag.lower()
    return ''

# ==============================================================================

if __name__ == '__main__':
    dwr = RandomizerUI()
    dwr.master.title("dwrandomizer %s" % dwrandomizer.VERSION)
    dwr.mainloop()

