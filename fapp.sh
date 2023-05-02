#!/usr/bin/bash -e

make -B
rm -rf tmp
mkdir tmp
for i in $(seq 1); do
  fapp -C -d tmp/rep${i} -Icpupa -Hevent=pa${i} ./triad.fapp
  fapp -A -d tmp/rep${i} -Icpupa -tcsv -o pa${i}.csv
done
