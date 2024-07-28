#!/bin/bash -e 

echo -n "Compiling... "

asm6f -dDEBUG credits.asm credits.nes

echo "Done."

