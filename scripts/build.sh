#!/bin/bash

# Configure the project
cmake -S . -B ./build

# Build the project
cmake --build ./build
