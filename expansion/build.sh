#!/bin/bash -e 

echo -n "Compiling... "

asm6f -q -m -dDEBUG credits.asm credits.nes

echo "Done."

