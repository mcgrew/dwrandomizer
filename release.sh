#!/bin/bash

if [ $# -lt 1 ]; then
    echo "Please specify a version number"
    exit 1
fi;

cd $(dirname $0)/build
qmake DEFINES+="DWR_RELEASE" ../dwrandomizer-cli.pro
make
cp dwrandomizer-cli ../builds/
rm -r *

qmake DEFINES+="DWR_RELEASE" ../dwrandomizer.pro
make
cp dwrandomizer ../builds/
rm -r *

qmake-win DEFINES+="DWR_RELEASE" CONFIG+="c++11" ../dwrandomizer.pro
make
cp release/dwrandomizer.exe ../builds
rm -r *

cd ../builds
cp ../README.md ./
tar Jcf dwrandomizer-$1-linux.tar.xz dwrandomizer-cli dwrandomizer README.md

mv README.md README.txt
zip dwrandomizer-$1-windows.zip dwrandomizer.exe README.txt

