#!/bin/bash -e 

echo -n "Compiling... "

asm6f -q -m -f -dDEBUG credits.asm credits.nes

echo "Done."

