#!/bin/bash
set -e
START_BYTES=8
TEMP=2**30
END_BYTES=$START_BYTES*$TEMP
echo "start bash script "
for ((c=$START_BYTES;c<=$END_BYTES;c*=2))
do
  make clean
  make all FILE_SIZE="$c"
done
echo "end bash script "
