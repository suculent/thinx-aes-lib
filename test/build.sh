#!/usr/bin/env bash

echo "*** Building ***"
g++ -x c++ --std=c++14 test_ncrypt.c ../src/*.cpp

echo "*** Setting permissions ***"
chmod +x ./a.out

echo "*** Running test ***"

./a.out
