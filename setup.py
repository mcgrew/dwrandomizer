from distutils.core import setup
import py2exe, sys, os
 
sys.argv.append('py2exe')
 
setup(
    options = {'py2exe': {'bundle_files': 2, 'compressed': True}},
    console = [{'script': "randomizer_gui.py"}],
    zipfile = None,
)

#special thanks to Zurkei for this file :)
