#!/bin/bash -e 

echo -n "Compiling... "

asm6f -q -f -dDEBUG credits.asm credits.nes

echo "Done."

