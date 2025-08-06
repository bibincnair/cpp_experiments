#!/bin/bash

# Configure and build the project
./build.sh

# Run the tests
cd ./build
ctest -C Debug
cd ../
