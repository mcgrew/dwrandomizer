#!/usr/bin/env python3

from tkinter import *
from tkinter import filedialog, messagebox
import dwrandomizer
import random
import os
import os.path
import json
from contextlib import redirect_stdout

HOME = os.path.expanduser('~' + os.sep)
FLAGS = 'ACGHIMPTWZf'


class Logger(Text):
    def __init__(self, master):
        Text.__init__(self, master)

    def write(self, s):
        if s.startswith("IPS Checksum:") and self.master.toggle_frame.ips_copy_var.get():
            self.clipboard_clear()
            self.clipboard_append(s.strip(), type='STRING')
            s += " (copied to clipboard)"
        self.insert(END, s)


# ==============================================================================

class RandomizerUI(Frame):
    def __init__(self, master=None):
        Frame.__init__(self, master)
        self.update_lock = 0
        self.columnconfigure(0, weight=1)
        self.columnconfigure(1, weight=1)
        self.create_widgets()
        self.grid(sticky="NSEW")
        self.args = dwrandomizer.parse_args()
        self.master.geometry('750x500+100+100')  # set a reasonable default
        self.load_config()
        self.master.columnconfigure(0, weight=1)
        self.master.rowconfigure(0, weight=1)
        self.winfo_toplevel().protocol("WM_DELETE_WINDOW", self.quit)

    def create_widgets(self):
        self.rom_frame = RomFrame(self)
        self.rom_frame.grid(column=0, row=0, columnspan=2, sticky="NSEW")
        self.output_frame = OutputFrame(self)
        self.output_frame.grid(column=0, row=1, columnspan=2, sticky="NSEW")
        self.seed_frame = SeedFrame(self)
        self.seed_frame.grid(column=0, row=2, sticky="NSEW")
        self.flags_frame = FlagsFrame(self)
        self.flags_frame.grid(column=1, row=2, sticky="NSEW")
        self.flags_frame.trace(self.update_settings)
        self.toggle_frame = ToggleFrame(self)
        self.toggle_frame.grid(column=0, row=3, columnspan=2, sticky="NSEW")
        self.toggle_frame.trace(self.update_flags)
        self.level_frame = LevelingFrame(self, "Leveling Speed", 'f', 0, 4)
        self.level_frame.trace(self.update_flags)
        self.growth_frame = UltraFrame(self, "Growth Randomization", 'g', 0, 5)
        self.growth_frame.trace(self.update_flags)
        self.spell_frame = UltraFrame(self, "Spell Learning Randomization", 'm', 0, 6)
        self.spell_frame.trace(self.update_flags)
        self.attack_frame = UltraFrame(self, "Enemy Attack Pattern Randomization", 'p', 1, 4)
        self.attack_frame.trace(self.update_flags)
        self.zone_frame = UltraFrame(self, "Enemy Zone Randomization", 'z', 1, 5)
        self.zone_frame.trace(self.update_flags)
        self.randomize_button = Button(self, text="Randomize!", command=self.execute)
        self.randomize_button.grid(column=1, row=6, sticky="E")
        self.logger = Logger(self)
        self.logger.grid(column=0, row=7, columnspan=2, sticky='NSEW')
        self.rowconfigure(7, weight=1)
        self.update_settings()

    def update_settings(self, *args):
        if self.update_lock:
            return
        self.update_lock = 1
        flags = self.flags_frame.tk_flags.get()
        self.toggle_frame.from_flags(flags)
        self.level_frame.from_flags(flags)
        self.growth_frame.from_flags(flags)
        self.spell_frame.from_flags(flags)
        self.attack_frame.from_flags(flags)
        self.zone_frame.from_flags(flags)
        self.update_lock = 0

    def update_flags(self, *args):
        if self.update_lock:
            return
        self.update_lock = 1
        flags = ''
        flags += self.toggle_frame.flags()
        flags += self.level_frame.flags()
        flags += self.growth_frame.flags()
        flags += self.spell_frame.flags()
        flags += self.attack_frame.flags()
        flags += self.zone_frame.flags()

        # sort the flags alphabetically
        flags = list(flags)
        flags.sort()
        self.flags_frame.set(''.join(flags))
        self.update_lock = 0

    def execute(self):
        self.args.filename = self.rom_frame.get()
        if not os.path.exists(self.args.filename):
            messagebox.showerror("No such ROM", "The specified ROM file does not exist.")
            return
        self.args.output_dir = self.output_frame.get()
        if not os.path.isdir(self.args.output_dir):
            messagebox.showerror("No such directory", "The specified output directory does not exist.")
            return
        self.args.seed = self.seed_frame.get()
        self.args.force = True
        flags = self.toggle_frame.flags()
        self.args.no_map = not 'A' in flags
        self.args.no_chests = not 'C' in flags
        self.args.speed_hacks = 'H' in flags
        self.args.no_search_items = not 'I' in flags
        self.args.shuffle_music = 'K' in flags
        self.args.disable_music = 'Q' in flags
        self.args.no_towns = not 'T' in flags
        self.args.no_shops = not 'W' in flags
        self.args.death_necklace = 'D' in flags
        self.args.menu_wrap = 'R' in flags
        self.args.ips = bool(self.toggle_frame.ips_var.get())

        self.args.no_zones = self.zone_frame.get() == 'none'
        self.args.ultra_zones = self.zone_frame.get() == 'ultra'
        self.args.no_patterns = self.attack_frame.get() == 'none'
        self.args.ultra_patterns = self.attack_frame.get() == 'ultra'
        self.args.no_growth = self.growth_frame.get() == 'none'
        self.args.ultra_growth = self.growth_frame.get() == 'ultra'
        self.args.no_spells = self.spell_frame.get() == 'none'
        self.args.ultra_spells = self.spell_frame.get() == 'ultra'
        self.args.very_fast_leveling = (self.level_frame.get() == 'ultra')
        self.args.fast_leveling = (self.level_frame.get() == 'normal')

        self.logger.delete('1.0', END)

        with redirect_stdout(self.logger):
            try:
                self.save_config()
                dwrandomizer.randomize(self.args)
            except Exception as e:
                print(str(e))
                raise e

    def quit(self):
        self.save_config()
        self.winfo_toplevel().destroy()

    def save_config(self):
        self.configuration = {'flags': self.flags_frame.get(),
                              'seed': self.seed_frame.get(),
                              'ips': bool(self.toggle_frame.ips_var.get()),
                              'ips_copy': bool(self.toggle_frame.ips_copy_var.get()),
                              'geometry': self.master.geometry(),
                              'rom': self.rom_frame.get(), 'output': self.output_frame.get()
                              }
        try:
            if not os.path.exists(HOME + ".config/"):
                os.makedirs(HOME + ".config/")
            with open(HOME + ".config/dwrandomizer.conf", 'w') as cfgfile:
                json.dump(self.configuration, cfgfile)
        except OSError:
            print("Unable to save configuration.")

    def load_config(self):
        if os.path.exists(HOME + ".config/dwrandomizer.conf"):
            try:
                with open(HOME + ".config/dwrandomizer.conf", 'r') as cfgfile:
                    self.configuration = json.load(cfgfile)
                cfg = self.configuration
                if 'flags' in cfg: self.flags_frame.set(cfg['flags'])
                if 'seed' in cfg: self.seed_frame.set(cfg['seed'])
                if 'ips' in cfg: self.toggle_frame.ips_var.set(int(cfg['ips']))
                if 'ips_copy' in cfg: self.toggle_frame.ips_copy_var.set(int(cfg['ips_copy']))
                if 'geometry' in cfg: self.master.geometry(cfg['geometry'])
                if 'rom' in cfg: self.rom_frame.set(cfg['rom'])
                if 'output' in cfg: self.output_frame.set(cfg['output'])
            except OSError:
                print("Unable to load configuration.")


# ==============================================================================

class RomFrame(LabelFrame):
    def __init__(self, master=None, initial_file='', initial_dir=HOME):
        LabelFrame.__init__(self, master, text="Rom File")
        self.tk_filename = StringVar()
        self.create_widgets()

    def create_widgets(self):
        self.file_name_entry = Entry(self, textvariable=self.tk_filename)
        self.file_open_button = Button(self, text="Choose", command=self.choose_file)
        self.file_name_entry.grid(column=0, row=0, sticky="NSEW")
        self.file_open_button.grid(column=1, row=0, sticky="E")
        self.columnconfigure(0, weight=1)
        self.columnconfigure(1, weight=0)

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
        self.create_widgets()

    def create_widgets(self):
        self.dir_name_entry = Entry(self, textvariable=self.tk_dirname)
        self.dir_open_button = Button(self, text="Choose", command=self.choose_dir)
        self.dir_name_entry.grid(column=0, row=0, sticky="NSEW")
        self.dir_open_button.grid(column=1, row=0, sticky="E")
        self.columnconfigure(0, weight=1)
        self.columnconfigure(1, weight=0)

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
        self.create_widgets()

    def create_widgets(self):
        valcommand = (self.register(self.validate), '%P')
        self.seed_entry = Entry(self, textvariable=self.tk_seed, validate="key", validatecommand=valcommand)
        self.seed_entry.grid(column=0, row=0, sticky="NSEW")
        self.random_button = Button(self, text="Random", command=self.random_seed)
        self.random_button.grid(column=1, row=0, sticky="E")
        self.columnconfigure(0, weight=1)
        self.columnconfigure(1, weight=0)

    def random_seed(self):
        self.set(random.randint(0, sys.maxsize))

    def set(self, value):
        self.tk_seed.set(value)

    def get(self):
        return self.tk_seed.get()

    @staticmethod
    def validate(current):
        """
        Validates the seed number input

        :param current: The newly edited text
        :return: True if the new text is allowed (numbers only)
        """
        try:
            if len(current):
                int(current)
            return True
        except ValueError:
            return False


# ==============================================================================

class FlagsFrame(LabelFrame):
    def __init__(self, master=None):
        LabelFrame.__init__(self, master, text="Flags")
        self.tk_flags = StringVar(value=FLAGS)
        self.create_widgets()

    def create_widgets(self):
        self.flag_entry = Entry(self, textvariable=self.tk_flags)
        self.flag_entry.grid(column=0, row=0, sticky="NSEW")
        self.columnconfigure(0, weight=1)

    def trace(self, func):
        self.tk_flags.trace('w', func)

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
        self.shops_var = IntVar()
        self.speed_hacks_var = IntVar()
        self.death_necklace_var = IntVar()
        self.menu_wrap_var = IntVar()
        self.ips_copy_var = IntVar()
        self.music_var = IntVar()
        self.music_off_var = IntVar()
        self.create_widgets()

    def create_widgets(self):
        self.map_button = \
            Checkbutton(self, text="Generate A New Map", variable=self.map_var)
        self.towns_button = \
            Checkbutton(self, text="Randomize Towns", variable=self.towns_var)
        self.shops_button = \
            Checkbutton(self, text="Randomize Weapon Shops", variable=self.shops_var)
        self.ips_button = \
            Checkbutton(self, text="Generate IPS Patch File", variable=self.ips_var)
        self.chests_button = \
            Checkbutton(self, text="Shuffle Chests", variable=self.chests_var)
        self.searchable_button = \
            Checkbutton(self, text="Shuffle Searchable Items",
                        variable=self.searchable_var)
        self.speed_hacks_button = \
            Checkbutton(self, text="Enable Speed Hacks",
                        variable=self.speed_hacks_var)
        self.death_necklace_button = \
            Checkbutton(self, text="Enable Death Necklace Funcionality",
                        variable=self.death_necklace_var)
        self.menu_wrap_button = \
            Checkbutton(self, text="Enable Menu Wrap-around",
                        variable=self.menu_wrap_var)
        self.music_button = \
            Checkbutton(self, text="Shuffle game music(beta)",
                        variable=self.music_var)
        self.music_off_button = \
            Checkbutton(self, text="Disable game music(beta)",
                        variable=self.music_off_var)
        self.ips_copy_button = \
            Checkbutton(self, text="Copy IPS Checksum to Clipboard", variable=self.ips_copy_var)
        self.map_button.grid(column=0, row=0, sticky="NSW")
        self.towns_button.grid(column=0, row=1, sticky="NSW")
        self.shops_button.grid(column=0, row=2, sticky="NSW")
        self.music_button.grid(column=0, row=3, sticky="NSW")
        self.chests_button.grid(column=1, row=0, sticky="NSW")
        self.searchable_button.grid(column=1, row=1, sticky="NSW")
        self.menu_wrap_button.grid(column=1, row=2, sticky="NSW")
        self.music_off_button.grid(column=1, row=3, sticky="NSW")
        self.death_necklace_button.grid(column=2, row=0, sticky="NSW")
        self.speed_hacks_button.grid(column=2, row=1, sticky="NSW")
        self.ips_button.grid(column=2, row=2, sticky="NSW")
        self.ips_copy_button.grid(column=2, row=3, sticky="NSW")
        self.columnconfigure(0, weight=1)
        self.columnconfigure(1, weight=1)
        self.columnconfigure(2, weight=1)

    def trace(self, func):
        self.map_var.trace('w', func)
        self.chests_var.trace('w', func)
        self.death_necklace_var.trace('w', func)
        self.menu_wrap_var.trace('w', func)
        self.speed_hacks_var.trace('w', func)
        self.searchable_var.trace('w', func)
        self.shops_var.trace('w', func)
        self.music_var.trace('w', func)
        self.music_off_var.trace('w', func)
        self.towns_var.trace('w', func)

    def from_flags(self, flags):
        if 'A' in flags:
            self.map_var.set(1)
        else:
            self.map_var.set(0)
        if 'c' in flags.lower():
            self.chests_var.set(1)
        else:
            self.chests_var.set(0)
        if 'd' in flags.lower():
            self.death_necklace_var.set(1)
        else:
            self.death_necklace_var.set(0)
        if 'H' in flags:
            self.speed_hacks_var.set(1)
        else:
            self.speed_hacks_var.set(0)
        if 'i' in flags.lower():
            self.searchable_var.set(1)
        else:
            self.searchable_var.set(0)
        if 'k' in flags.lower():
            self.music_var.set(1)
        else:
            self.music_var.set(0)
        if 'q' in flags.lower():
            self.music_off_var.set(1)
        else:
            self.music_off_var.set(0)
        if 'r' in flags.lower():
            self.menu_wrap_var.set(1)
        else:
            self.menu_wrap_var.set(0)
        if 't' in flags.lower():
            self.towns_var.set(1)
        else:
            self.towns_var.set(0)
        if 'w' in flags.lower():
            self.shops_var.set(1)
        else:
            self.shops_var.set(0)

    def flags(self):
        flags = ''
        if self.map_var.get():
            flags += 'A'
        if self.chests_var.get():
            flags += 'C'
        if self.death_necklace_var.get():
            flags += 'D'
        if self.speed_hacks_var.get():
            flags += 'H'
        if self.searchable_var.get():
            flags += 'I'
        if self.music_var.get():
            flags += 'K'
        if self.menu_wrap_var.get():
            flags += 'R'
        if self.music_off_var.get():
            flags += 'Q'
        if self.towns_var.get():
            flags += 'T'
        if self.shops_var.get():
            flags += 'W'
        return flags


# ==============================================================================

class UltraFrame(LabelFrame):
    def __init__(self, master=None, text='Label', flag='', col=0, row=0):
        LabelFrame.__init__(self, master, text=text)
        self.variable = StringVar(value=text + "_var")
        self.variable.set('ultra')
        self.flag = flag
        self.grid(sticky="NSEW", column=col, row=row)
        self.create_widgets()

    def create_widgets(self):
        none = Radiobutton(self, text="None", variable=self.variable, value="none")
        none.grid(column=0, row=0)
        normal = Radiobutton(self, text="Normal", variable=self.variable, value="normal")
        normal.grid(column=1, row=0)
        ultra = Radiobutton(self, text="Ultra Random", variable=self.variable, value="ultra")
        ultra.grid(column=2, row=0)
        self.columnconfigure(0, weight=1)
        self.columnconfigure(1, weight=1)
        self.columnconfigure(2, weight=1)

    def trace(self, func):
        self.variable.trace('w', func)

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

    def from_flags(self, flags):
        if self.flag.upper() in flags:
            self.variable.set('ultra')
        elif self.flag.lower() in flags:
            self.variable.set('normal')
        else:
            self.variable.set('none')

    def set(self, value):
        self.variable.set(value)

    def get(self):
        return self.variable.get()


# ==============================================================================

class LevelingFrame(UltraFrame):
    def __init__(self, master=None, text='Label', flag='', col=0, row=0):
        UltraFrame.__init__(self, master, text, flag, col, row)
        self.variable.set('normal')

    def create_widgets(self):
        none = Radiobutton(self, text="Normal", variable=self.variable, value="none")
        none.grid(column=0, row=0)
        normal = Radiobutton(self, text="Fast", variable=self.variable, value="normal")
        normal.grid(column=1, row=0)
        ultra = Radiobutton(self, text="Very Fast", variable=self.variable, value="ultra")
        ultra.grid(column=2, row=0)
        self.columnconfigure(0, weight=1)
        self.columnconfigure(1, weight=1)
        self.columnconfigure(2, weight=1)


# ==============================================================================

if __name__ == '__main__':
    dwr = RandomizerUI()
    dwr.master.title("dwrandomizer %s" % dwrandomizer.VERSION)
    dwr.mainloop()
