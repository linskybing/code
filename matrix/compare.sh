#! /bin/bash

if [ -f "c.dat" ]; then
    rm "c.dat"
fi

make clean
make

time ./matrix_v2

mv c.dat ans.dat

time ./matrix_v3

diff c.dat ans.dat