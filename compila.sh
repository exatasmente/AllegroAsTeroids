#!/bin/bash
gcc main.c -o test -lm -lallegro -lallegro_image -lallegro_font -lallegro_ttf -lallegro_audio -lallegro_acodec
./test