#!/bin/sh

# Download video sample in y4m format to data dir
cd data
sudo rm -rf *
wget "https://media.xiph.org/video/derf/y4m/coastguard_qcif_mono.y4m"

# Change file name
mv coastguard_qcif_mono.y4m input_video.y4m

cd -