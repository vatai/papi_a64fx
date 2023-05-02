#!/usr/bin/bash -e

make -B
rm -rf rep1
fapp -C -d rep1 -Icpupa -Hevent=pa1 ./triad.fapp
fapp -A -d rep1 -Icpupa -tcsv -o pa1.csv
