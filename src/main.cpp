#include <iostream>
#include <fstream>
#include <iomanip>
#include <filesystem>

#include "AudioFile.h"
#include "json.hpp"
#include "cxxopts.hpp"
#include "FFT.hpp"

#define PRINTER(s, x) if(!s.quiet) { std::cout << x; }

struct Settings {
	std::vector<std::filesystem::path> files;
	bool quiet;
	float splitInterval;
};

Settings Parse(int argc, char** argv);

int main(int argc, char** argv)
{
	Settings setts;
	setts = Parse(argc, argv);

	int numFiles = setts.files.size();
	for (auto& file : setts.files) {
		AudioFile<double> audioFile;

		if (!audioFile.load(file.string()))
		{
			continue;
		}

		std::string filename = file.filename().string();

		int sampleRate = audioFile.getSampleRate();
		int numChannels = audioFile.getNumChannels();

		nlohmann::json output;
		for (int c = 1; c <= numChannels; c++) {
			PRINTER(setts, "\rAnalyzing " << filename << "... Channel " << c << "/" << numChannels << " 0%                  ");

			std::string chName = "channel_" + std::to_string(c);
			output[chName] = nlohmann::json::array();

			if (setts.splitInterval == 0.0f)
			{
				std::vector<std::pair<double, double>> spectrum = FFT(audioFile.samples[c-1].cbegin(), audioFile.samples[c-1].cend(), sampleRate);

				output[chName] = nlohmann::json::array();
				for (const std::pair<double, double>& pair : spectrum) {
					output[chName].push_back({ {"freq", pair.first}, {"mag", pair.second } });
				}
			}
			else
			{
				int sampleInterval = sampleRate * setts.splitInterval / 1000;
				int currentSample;
				for (currentSample = 0; currentSample < audioFile.samples[c - 1].size(); currentSample += sampleInterval)
				{
					std::vector<std::pair<double, double>> spectrum = 
						FFT(
							audioFile.samples[c-1].cbegin() + currentSample, 
							std::min(
								audioFile.samples[c-1].cbegin() + currentSample + sampleInterval, 
								audioFile.samples[c-1].cend()), 
							sampleRate
						);

					output[chName].push_back({
						{"begin", currentSample},
						{"end", currentSample + sampleInterval},
						{"spectrum", nlohmann::json::array()}
					});

					for (const std::pair<double, double>& pair : spectrum) {
						output[chName].back()["spectrum"].push_back({ {"freq", pair.first}, {"mag", pair.second } });
					}

					PRINTER(setts, "\rAnalyzing " << filename << "... Channel " << c << "/" << numChannels << " " << (int)std::floor((float)currentSample / (float)audioFile.samples[c-1].size() * 100.0f) << "%                  ");
				}
			}
		}

		std::ofstream ofs(file.replace_extension("json"));
		ofs << std::setw(4) << output << std::endl;
		ofs.close();

		PRINTER(setts, "\rAnalyzing " << filename << "... 100%                      " << std::endl);
	}

	return 0;
}

Settings Parse(int argc, char** argv)
{
	Settings setts;

	try
	{
		cxxopts::Options options("spectralyze", "Fourier transforms audio files");
		options
			.set_width(70)
			.positional_help("file1 [file2...]")
			.add_options()
			("q,quiet", "Suppress text output", cxxopts::value<bool>()->default_value("false"))
			("i,interval", "Splits audio file into intervals of length i milliseconds and transforms them individually (0 to not split file)", cxxopts::value<float>())
			("files", "Files to fourier transform", cxxopts::value<std::vector<std::filesystem::path>>())
			("h,help", "Print usage")
			;

		options.parse_positional("files");

		auto result = options.parse(argc, argv);
		if (result.count("help"))
		{
			std::cout << options.help() << std::endl;
			exit(0);
		}

		if (!result.count("files"))
		{
			std::cerr << "At least one positional argument is required." << std::endl;
			exit(1);
		}

		setts.files = result["files"].as<std::vector<std::filesystem::path>>();
		setts.quiet = (result.count("quiet") ? result["quiet"].as<bool>() : false);
		setts.splitInterval = (result.count("interval") ? result["interval"].as<float>() : 0.0f);
	}
	catch (const cxxopts::OptionException& e)
	{
		std::cout << "Invalid parameters: " << e.what() << std::endl;
		exit(1);
	}

	return setts;
}
