#!/bin/bash
cd "$(dirname -- "$(readlink -fn -- "${0}")")"

for i in *.png; do
  printf "Resize $i\n"
  convert "$i" -resize 256x256 "$i"
done

convert sponza_flagpole_diff.png  -resize   64x64 sponza_flagpole_diff.png
convert sponza_ceiling_a_diff.png -resize 128x128 sponza_ceiling_a_diff.png
convert sponza_column_c_diff.png  -resize   64x64 sponza_column_c_diff.png
