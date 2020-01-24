#!/usr/bin/env bash

# Build test-cases and test with Check2 and coverage

g++ -fprofile-arcs -ftest-coverage -std=c++14 -Wall -o 020-TestCase 020-TestCase.cpp ../src/*.cpp

pushd coverage
rm -rf

# Collect coverage

gcov ../020-TestCase

popd
