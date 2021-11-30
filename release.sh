#!/bin/bash

if [ $# -lt 1 ]; then
    echo "Please specify a version number"
    exit 1
fi;

cd $(dirname $0)/build
qmake CONFIG+="RELEASE CLI" ../dwrandomizer-cli.pro
make
cp dwrandomizer ../builds/dwrandomizer-cli
rm -r *

qmake CONFIG+=RELEASE ../dwrandomizer.pro
make
cp dwrandomizer ../builds/
rm -r *

qmake-win CONFIG+=RELEASE ../dwrandomizer.pro
make
cp release/dwrandomizer.exe ../builds
rm -r *

cd ../builds
cp ../README.md ./
tar Jcf dwrandomizer-$1-linux.tar.xz dwrandomizer-cli dwrandomizer README.md

mv README.md README.txt
zip dwrandomizer-$1-windows.zip dwrandomizer.exe README.txt

