#!/usr/bin/bash -e

make -B
rm -rf tmp
mkdir tmp
for i in $(seq 17); do
  fapp -C -d tmp/rep${i} -Icpupa -Hevent=pa${i} ./triad.fapp
  fapp -A -d tmp/rep${i} -Icpupa -tcsv -o tmp/pa${i}.csv
  fapp -A -d tmp/rep${i} -Icpupa -txml -o tmp/pa${i}.xml
done
