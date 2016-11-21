from distutils.core import setup
import py2exe, sys, os
from dwrandomizer import VERSION
 
sys.argv.append('py2exe')
 
setup(
    name = 'dwrandomizer',
    version = VERSION,
    description = 'A randomizer for Dragon Warrior for NES',
    url = 'http://github.com/mcgrew/dwrandomizer',
    author = 'Thomas McGrew',
    options = {'py2exe': {'bundle_files': 2, 'compressed': True}},
    windows = [{'script': 'randomizer_gui.py', 'dest_base': 'dwrandomizer'}],
    zipfile = None
)

# special thanks to Zurkei for figuring out how to make this work :)
