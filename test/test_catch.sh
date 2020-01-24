#!/usr/bin/env bash

# Build test-cases and test with Check2

g++ -std=c++14 -Wall -o 020-TestCase 020-TestCase.cpp ../src/*.cpp && ./020-TestCase --success
