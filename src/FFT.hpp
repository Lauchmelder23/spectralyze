#pragma once
#include <vector>
#include <complex>

enum class WindowFunctions {
	RECTANGLE,
	GAUSS,
	VON_HANN,
	TRIANGLE,
	BLACKMAN
};

extern std::vector<std::pair<double, double>> FFT(const std::vector<double>::const_iterator& begin,
	const std::vector<double>::const_iterator& end,
	size_t sampleRate,
	double minFreq, double maxFreq,
	unsigned int zeropadding);

extern void SetWindowFunction(WindowFunctions func, unsigned int width);
extern void UseFastFunctions();