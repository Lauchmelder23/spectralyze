# spectralyze
A commandline tool for fourier transforming audio files

## Usage
Spectralyze is a commandline tool, type `spectralyze -h` to get a list of available options. Anyways, here is an example command you might run:

```
spectralyze -i 50 coolSong.wav
```

This would read in a file called `coolSong.wav`, split it into 50ms long audio segments and then transform each of those individually. The resulting spectrums will be stored in `coolSong.json`

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

## Used libraries
* [AudioFile](https://github.com/adamstark/AudioFile) for loading audio files
* [JSON for Modern C++](https://github.com/nlohmann/json) for writing JSON data
* [cxxopts](https://github.com/jarro2783/cxxopts) for parsing commandline arguments