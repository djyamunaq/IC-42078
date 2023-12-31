#!/bin/sh

# Download video sample in y4m format to data dir
cd data
rm -rf *
wget https://media.xiph.org/video/derf/y4m/elephants_dream_360p24.y4m.xz

# Change file name
mv elephants_dream_360p24.y4m.xz input_video.y4m.xz

# Decompress file
xz --decompress --verbose input_video.y4m.xz

cd -