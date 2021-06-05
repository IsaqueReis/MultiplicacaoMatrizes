#!/bin/sh
rm -r *.o
rm -r $PWD/output/*.dat
rm -r $PWD/input/*.dat
gcc -c *.c
gcc -o multipilica-matrizes *.o -pthread -lm
./multipilica-matrizes