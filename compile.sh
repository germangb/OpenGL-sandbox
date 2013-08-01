#!/bin/sh
echo hold on, I\'m compiling...
g++ src/*.cpp src/GLee.c -o $1 -lSDL -lGL -lGLU -lSDL_image -lSOIL -std=c++0x
echo done! :D
