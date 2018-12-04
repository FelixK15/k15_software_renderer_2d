#!/bin/bash
C_FILE_TO_COMPILE="k15_x11_software_renderer_2d.c"
EXECUTABLE_FILE_NAME="x11_example"
GCC_OPTIONS="-ansi -std=c99 -g3 -L/usr/X11/lib -lX11 -o $EXECUTABLE_FILE_NAME"
gcc $C_FILE_TO_COMPILE $GCC_OPTIONS
