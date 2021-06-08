#!/bin/sh
gcc -c *.c
gcc -o multipilica-matrizes *.o -pthread -lm
./multipilica-matrizes
