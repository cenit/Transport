#!/bin/bash

mkdir -p build
cd build
{ 
  cmake ..
  cmake --build . 
} # > ../log.out 2> ../log.err

