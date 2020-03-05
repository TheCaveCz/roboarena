#!/bin/bash

for RAWNAME in data-raw/*.wav
do
  NAME=$(basename "$RAWNAME" .wav)
  rm data/$NAME.mp3
  ffmpeg -i data-raw/$NAME.wav -ar 48000 -ac 2 -b:a 128k -metadata encoder=foobar data/$NAME.mp3
  id3v2 -D data/$NAME.mp3
done
