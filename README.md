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
3. Then do the rest of the processing: `../makedat`

Step 2 will create the `wav` files using espeak. You can hear what each `wav` sound like, for example: `aplay speak.wav`

Step 3, the *makedat* script, does all the heavy lifting:
1. The `wav` files are converted to `raw` files.
2. The `raw` files are analyzed. A preliminary index file is generated showing where quiet gaps are and this is used to trim the leading and trailing quiet regions of the `raw` file.
3. Then, in blocks of 160 16-bit values, the maximum absolute value is found and listed by block index in the `list` files. These `list` files are then used to generate new `index` files.
4. The `raw` wave files are encoded into `dat` files by *c2enc*.

Please note that there are no usage messages for either the *speakwav* or the *makedat* script, but *makedat* will take exactly one parameter that will be covered in the next section.

In the end, only the `dat` files and `speak.index` is needed as `speak.dat` is the only file that *mspot* slices up to produce either your repeater callsign or destination callsign `dat`.

## Make your own voice files

You need `wav` files, or other files that *sox* can translate to `wav` files, or monaural, 16-bit, little-endian, 8000 Hz `raw` files. The contents of these files are listed in the `speakwav` script in the quoted string used to create each file. Filenames are chosen to be descriptive and you must use those exact names for your files. The best way to make your voice files is with a audio tool like *audacity* or *garageband* using a decent microphone. Using these tools you can easily edit and enhance the recording for the best possible playback on *mspot*.

Once you have recorded all the needed files, you can move them to a new folder and process them with the *make* script just like in the previous section. The `speak.raw` file is the most critical to get right. The *index* program must be able to find exactly 67 words in this file. They will be indexed with `0` through `66`. If *index* is not finding 67 words, first, listen to it to make sure you have recorded all 67 words: `space, A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, zee, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, dash, slash, dot, alpha, bravo, Charlie, delta, echo, foxtrot, golf, hotel, India, juliette, kilo, Lima, Mike, november, Oscar, papa, Quebec, romeo, Sierra, tango, uniform, Victor, whiskey, X-ray, yankee, zulu, M 17`.

You should be listening to the `dat` file. If you're in your new folder: `../tools/c2dec 3200 speak.dat - | aplay -f s16_le`

If the recording volume was too low or too high, you'll have to re-record. The `speak.list` file should show values no greater than about 20k-25k and gaps between words should ideally have values below 10. However if the noise floor between words is larger, you can try reprocessing the `raw` files: `../makedat <threshold>` where `<threshold>` is a value slightly larger than the values seen between words in your `speak.list` file. The object here, you want *index* to find exactly 67 words. Using too small of a threshold will find two few words while using a too large threshold will find too many words.

Once you are happy with the outcome, move all your `dat` files and `speak.index` to your *mspot* system. Then, make sure `mspot.ini` points to that location. Don't put them the the `mspot/audio` folder because you won't be able to update your *mspot* repo.
