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
	double minFreq, maxFreq;
	unsigned int analyzeChannel;
	unsigned int zeropadding;
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
		int c = setts.analyzeChannel;
		if (c == 0)
			c = 1;
		else
			numChannels = c;

		for (int c = 1; c <= numChannels; c++) {
			PRINTER(setts, "\rAnalyzing " << filename << "... Channel " << c << "/" << numChannels << " 0%                  ");

			std::string chName = "channel_" + std::to_string(c);
			output[chName] = nlohmann::json::array();

			if (setts.splitInterval == 0.0f)
			{
				std::vector<std::pair<double, double>> spectrum = 
					FFT(
						audioFile.samples[c-1].cbegin(), 
						audioFile.samples[c-1].cend(), 
						sampleRate,
						setts.minFreq, setts.maxFreq,
						setts.zeropadding
					);

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
								audioFile.samples[c-1].cend()
							), 
							sampleRate,
							setts.minFreq, setts.maxFreq,
							setts.zeropadding
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
			.positional_help("FILE1 [FILE2...]")
			.add_options()
			("q,quiet", "Suppress text output", cxxopts::value<bool>()->default_value("false"))
			("i,interval", "Splits audio file into intervals of length i milliseconds and transforms them individually (0 to not split file)", cxxopts::value<float>())
			("f,frequency", "Defines the frequency range of the output spectrum (Default: all the frequencies)", cxxopts::value<std::vector<double>>())
			("p,pad", "Add extra zero-padding. By default, the program will pad the signals with 0s until the number of samples is a power of 2 (this would be equivalent to -p 1). With this option you can tell the program to instead pad until the power of 2 after the next one (-p 2) etc. This increases frequency resolution", cxxopts::value<unsigned int>())
			("m,mono", "Analyze only the given channel", cxxopts::value<unsigned int>()->default_value("0"))
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

		if (!result.count("frequency"))
		{
			setts.minFreq = 0.0f;
			setts.maxFreq = 0.0f;
		}
		else
		{
			setts.minFreq = result["frequency"].as<std::vector<double>>()[0];
			setts.maxFreq = result["frequency"].as<std::vector<double>>()[1];
		}

		if (!result.count("files"))
		{
			std::cerr << "At least one positional argument is required." << std::endl;
			exit(1);
		}

		setts.files = result["files"].as<std::vector<std::filesystem::path>>();
		setts.quiet = (result.count("quiet") ? result["quiet"].as<bool>() : false);
		setts.splitInterval = (result.count("interval") ? result["interval"].as<float>() : 0.0f);
		setts.analyzeChannel = (result.count("mono") ? result["mono"].as<unsigned int>() : 0);
		setts.zeropadding = (result.count("pad") ? result["pad"].as<unsigned int>() : 1);
		

		if (setts.maxFreq <= setts.minFreq && (setts.maxFreq != 0))
		{
			std::cerr << "Maximum frequency cannot be smaller than minimum frequency" << std::endl;
			exit(1);
		}
	}
	catch (const cxxopts::OptionException& e)
	{
		std::cout << "Invalid parameters: " << e.what() << std::endl;
		exit(1);
	}

	return setts;
}
