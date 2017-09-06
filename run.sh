#!/bin/bash

make
./server &
./test.py localhost 9999 500 10 10 500
make clean
