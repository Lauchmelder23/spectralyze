#include <iostream>
#include <fstream>
#include <iomanip>
#include <filesystem>

#include "AudioFile.h"
#include "json.hpp"
#include "FFT.hpp"

void PrintUsage();

int main(int argc, char** argv)
{
	if (argc < 2) {
		PrintUsage();
		return 1;
	}

	int numFiles = argc - 1;
	for (int i = 1; i < argc; i++) {
		AudioFile<double> audioFile;
		audioFile.load(argv[i]);

		std::string filename = std::filesystem::path(argv[i]).filename().string();

		int sampleRate = audioFile.getSampleRate();
		int numChannels = audioFile.getNumChannels();

		nlohmann::json output;
		for (int c = 1; c <= numChannels; c++) {
			std::cout << "\rAnalyzing " << filename << "... Channel " << c << "/" << numChannels << "            ";
			std::vector<std::pair<double, double>> spectrum = FFT(audioFile.samples[c-1], sampleRate);

			std::string chName = "channel_" + std::to_string(c);
			output[chName] = nlohmann::json::array();
			for (const std::pair<double, double>& pair : spectrum) {
				output[chName].push_back({ {"freq", pair.first}, {"mag", pair.second } });
			}
		}

		std::ofstream ofs(std::filesystem::path(argv[i]).replace_extension("json"));
		ofs << std::setw(4) << output << std::endl;
		ofs.close();
		std::cout << "\rAnalyzing " << filename << "... Done!        " << std::endl;
	}

	return 0;
}

void PrintUsage()
{
	std::cerr << "Usage: spectralyze file1 [file2...]" << std::endl;
}
