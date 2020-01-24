#!/usr/bin/env bash

# Build test-cases and test with Check2 and coverage using Sonarcloud.io wrapper

if [[ $(uname) == "Darwin" ]]; then
  BUILD_WRAPPER="../build-wrapper-macosx-x86/build-wrapper-macosx-x86"
else
  BUILD_WRAPPER="../build-wrapper-linux-x86/build-wrapper-linux-x86"
fi

${BUILD_WRAPPER} \
--out-dir build_wrapper_output_directory \
g++ -fprofile-arcs -ftest-coverage -std=c++14 -Wall -o 020-TestCase 020-TestCase.cpp ../src/*.cpp

pushd coverage
rm -rf

# Collect coverage

gcov ../020-TestCase

popd
