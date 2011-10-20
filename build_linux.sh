#!/bin/bash

echo Building main program
qmake
make
make clean

echo Building plugins
cd plugins
qmake
make
make clean
cd ..

echo Building updater
cd updater
qmake
make
make clean

cd ..
