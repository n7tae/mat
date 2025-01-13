# mat
M17 Audio Tools

## Introduction

This is the machinery used to make the 3200 bps Codec2 `*.dat` files for [*mspot*](https://github.com/n7tae/mspot), an M17-only hot-spot/repeater. You can use this to create your own *mspot* audio files, or you can use it to create Codec2 dat file from any wav file.

The `*dat` files produced are binary files containing just the C2_3200 encoded data. That means that there are 16 byes of data for every M17 *voice-only* 40 millisecond frame. The size of the `*.dat` file produced will always be a multiple of 16.

## Building the tools

1. Make sure you have the needed packages: `sudo apt install git build-essential`
2. Download this repo: `git clone https://github.com/n7tae/mat`
3. Move to the tools folder: `cd mat/tools`
4. Build the tools: `make`

This will build three programs:
- *c2enc* is used to create a Codec2 `dat` file from a monaural, 16-bit, little-endian, 8000 Hz, `raw` audio file.
- *c2dec* is used to create a `raw` audio file from a Codec2 `dat` file.
- *index* is used to produce a `list` and `index` file from the `raw` audio file.

All three programs print usage messages if you don't supply any parameters, but you for everything described here, you won't be using any of these tools directly.

Finally, move back to the repo's base folder to get ready for the next step: `cd ..`

## Make the *mspot* `dat` files

You'll need to do this on a debian-based computer that can play audio. It's a good idea to go through this even if you are going to provide you own `wav` files, just to see how everything works. But first, we need some more packages:

`sudo apt install espeak alsa-utils sox`

From the repo's base directory:
1. Create a directory for all your files and go there: `mkdir mspot && cd mspot`
2. Make the `wav` files: `../speakwav`
3. Do the rest of the processing: `../make`

Step 2 will create the `wav` files using espeak. You can hear what each `wav` sound like, for example: `aplay speak.wav`

Step 3 does all the heavy lifting:
1. The `wav` files are converted to `raw` files.
2. The `raw` files are analyzed. In blocks of 160 16-bit values, the maximum absolute value is found and listed by block index in the `list` files. These `list` files are then used to find spoken word boundaries to produce the `index` files.
3. The `raw` wave files are encoded into `dat` files by *c2enc*.
4. Finally, *dd* is used to trim quiet from the beginning and the end of each `dat` file, and if quiet was trimmed from the beginning of the file, a new, corrected `index` file will be produced.
