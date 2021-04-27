# spectralyze
A commandline tool for fourier transforming audio files

## Usage
```
spectralyze file1 [file2...]
```

## Supported Formats
* WAV
* AIFF

## What does it do
Spectralyze reads one or more audio files from the standard arguments and creates JSON files containing the spectrum of the file. 
Each audio channel in the file is transformed separately. The resulting JSON has the following structure
```
{
  "channel_1": [
    {"freq": 0.0, "mag": 1.0"},
    {"freq": 0.1, "mag": 1.1"},
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