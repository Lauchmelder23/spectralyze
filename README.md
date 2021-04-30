# spectralyze
A commandline tool for fourier transforming audio files

## Usage
Spectralyze is a commandline tool, type `spectralyze -h` to get a list of available options. Anyways, here is an example command you might run:

```
spectralyze -i 50 coolSong.wav
```

This would read in a file called `coolSong.wav`, split it into 50ms long audio segments and then transform each of those individually. The resulting spectrums will be stored in `coolSong.json`

## Setting a frequency range
By default, spectralyze will output the entire frequency spectrum, all the way up to the nyquist limit (which is half the sampling rate). If you are not interested in the entire spectrum you can tell the program the only output frequencies in the range you specify:
```
spectralyze -f 0,2500 coolSong.wav
```
This command would only output frequencies ranging from 0kHz-2kHz, greatly decreasing file size.

## Disabling channels
By default this program will analyze all channels in the given audio file, if you are only interested in noe specific channel you can tell the program that via the `-m` flag:
```
spectralyze -m 1 coolSong.wav
```
will only analyze the first audio channel

## Zero-padding
The FFT algorithm implemented here can only work if the number of samples is a power of 2. So by default, before performing the transformation, this program will zero-pad the signal until we reach such a sample size. Essentially, it appends a bunch of zeros to the end until it is a power of two. By using the `-p` flag you can go further than this. `-p 2` will tell the program to pad up until the power of two *after* the next one, essentially doubling the sample size. This results in a higher resolution in the frequency spectrum
```
spectralyze -p 3 coolSong.wav
```
This will tell the program to pad to the 3rd-next power of 2! This means, if the number if samples given is 100, it would pad it to 256 by default, and due to the `-p` switch all the way to 1024.

**RESOLUTION (and thus file size) SCALES WITH 2^p**

## Window functions
Window functions are used to "cut out" parts of the signal. When you use the `-i` flag, you are only looking at a certain interval in the audio file. This is equivalent to multiplying the whole audio file with a rectangular window function (it is 0 everywhere except in the interval, where it is 1). With the `-w` flag you can choose between different window functions. Currently supported are the Von-Hann function, and the Gauss function. Both of these yield "smoother" spectra and get rid of a lot of noise.

## Example command
```
spectralyze -i 20 -f 0,1000 -p 3 coolSong.wav
```
After the file coolSong.wav is read, the audio signal is split into 20ms long clips, which are then each individually given to the transformation function. 

Due to `-p 3`, the zero padding will go two powers of two higher than it normally would, essentially quadrupling output resolution.

`-f 0,1000` will limit the outputted spectrum to a range between 0kHz and 1kHz

## Supported Formats
* WAV
* AIFF

## What does it do
Spectralyze reads one or more audio files from the standard arguments and creates JSON files containing the spectrum of the file. 
Each audio channel in the file is transformed separately. The resulting JSON has the following structure
```
{
  "channel_1": [                          | Channel of the current spectrum
    {                                     |
      "begin": 0,                         | Index of the first sample of this segment
      "end": 1000,                        | Index of the last sample of this segment
      "spectrum": [                       |
        {"freq": 0.0, "mag": 1.0"},       | First spectrum datapoint
        {"freq": 0.1, "mag": 1.1"},       | Second spectrum datapoint
        ...
      ]
    },
    ...
  ],
  "channel_2": ...
}
```
Every supplied audio file will result in one JSON file. The magnitude is the absolute value of the real and imaginary part of the Fourier transformation.

## Example use case
This tool can theoretically be used to visualize music. The visualization part has to be written by you, though. For my little experiment I used python with matplotlib to create a line diagram from the spectra:

https://user-images.githubusercontent.com/24511538/116688886-a22e8880-a9b7-11eb-9a3d-b9b5069de697.mp4

Visualization written by [mpsparrow](https://github.com/mpsparrow)

## Used libraries
* [AudioFile](https://github.com/adamstark/AudioFile) for loading audio files
* [JSON for Modern C++](https://github.com/nlohmann/json) for writing JSON data
* [cxxopts](https://github.com/jarro2783/cxxopts) for parsing commandline arguments
