#!/bin/sh
rm -r *.o
gcc -c *.c
gcc -o multipilica-matrizes *.o -pthread -lm
./multipilica-matrizes