#include "FFT.hpp"

#define _USE_MATH_DEFINES
#include <math.h>
#include <functional>
#include <iostream>
#include <map>

#define POW_OF_TWO(x) (x && !(x & (x - 1)))

using namespace std::complex_literals;

typedef std::function<double(unsigned int)> WindowFunction;

static WindowFunction window;

inline double WindowRectangle(unsigned int k, unsigned int offset, unsigned int width);
inline double WindowVonHann(unsigned int k, unsigned int offset, unsigned int width);
inline double WindowGauss(unsigned int k, unsigned int offset, unsigned int width);
inline double WindowTriangle(unsigned int k, unsigned int offset, unsigned int width);
inline double WindowBlackman(unsigned int k, unsigned int offset, unsigned int width);

std::vector<std::complex<double>> 
radix2dit(
	const std::vector<double>& list,
	size_t offset,
	size_t N, 
	size_t s)
{
	std::vector<std::complex<double>> output(N);
	if (N == 1)
	{
		output[0] = window(offset) * (list[offset]);
	}
	else
	{
		size_t halfN = N >> 1;
		std::vector<std::complex<double>> first = radix2dit(list, offset, halfN, s << 1);
		std::vector<std::complex<double>> second = radix2dit(list, offset + s, halfN, s << 1);

		std::complex<double> coeff = -M_PI * 1.0i / (double)halfN;

		for (int k = 0; k < halfN; k++)
		{
			std::complex<double> p = first[k];
			std::complex<double> q = std::exp(coeff * (double)k) * second[k];

			output[k] = p + q;
			output[halfN + k] = p - q;
		}
	}

	return output;
}

std::vector<std::pair<double, double>>
FFT(const std::vector<double>::const_iterator& begin,
	const std::vector<double>::const_iterator& end,
	size_t sampleRate,
	double minFreq, double maxFreq,
	unsigned int zeropadding,
	bool mertz)
{
	std::vector<double> signal(begin, end);
	size_t N = signal.size();
	while (!POW_OF_TWO(N))
	{
		// Pad with zeros
		signal.push_back(0.0f);
		N++;
	}

	if (zeropadding > 1) {
		N = (signal.size() << (zeropadding - 1));
		signal.insert(signal.end(), N - signal.size(), 0);
	}

	WindowFunction f;
	


	std::vector<std::complex<double>> spectrum = radix2dit(signal, 0, N, 1);
	double freqRes = (double)sampleRate / (double)N;
	double nyquistLimit = (double)sampleRate / 2.0f;

	std::vector<std::pair<double, double>> output;
	double freq = minFreq;
	if (maxFreq == 0)
		maxFreq = nyquistLimit;

	for (int k = freq / freqRes; freq < nyquistLimit && freq < maxFreq; k++)
	{
		if(!mertz)
			output.push_back(std::make_pair(freq, 2.0f * std::abs(spectrum[k]) / (double)N));
		else
			output.push_back(std::make_pair(freq, 2.0f * (spectrum[k] * std::exp(-1i * std::arg(spectrum[k]))).real() / (double)N));

		freq += freqRes;
	}

	return output;
}

void SetWindowFunction(WindowFunctions func, unsigned int width)
{
	switch (func)
	{
	case WindowFunctions::RECTANGLE:	window = std::bind(WindowRectangle, std::placeholders::_1, 0, width); break;
	case WindowFunctions::VON_HANN:		window = std::bind(WindowVonHann, std::placeholders::_1, 0, width); break;
	case WindowFunctions::GAUSS:		window = std::bind(WindowGauss, std::placeholders::_1, 0, width); break;
	case WindowFunctions::TRIANGLE:		window = std::bind(WindowTriangle, std::placeholders::_1, 0, width); break;
	case WindowFunctions::BLACKMAN:		window = std::bind(WindowBlackman, std::placeholders::_1, 0, width); break;
	}
}

inline double WindowRectangle(unsigned int k, unsigned int offset, unsigned int width)
{
	return ((offset < k) && (k < width));
}

inline double WindowVonHann(unsigned int k, unsigned int offset, unsigned int width)
{
	return ((offset < k) && (k < width)) ? (0.5f * (1.0f - cos(2.0f * M_PI * k / (width - 1)))) : 0;
}

inline double WindowGauss(unsigned int k, unsigned int offset, unsigned int width)
{
	double coeff = (k - (width - 1) * 0.5f) / (0.4f * (width - 1) * 0.5f);
	return ((offset < k) && (k < width)) ? (std::exp(-0.5f * coeff * coeff)) : 0;
}

inline double WindowTriangle(unsigned int k, unsigned int offset, unsigned int width)
{
	return 1.0f - std::abs(((double)k - ((double)width / 2.0f)) / ((double)width / 2.0f));
}

inline double WindowBlackman(unsigned int k, unsigned int offset, unsigned int width)
{
	return 0.5f * (1.0f - 0.16f) - 0.5f * cos(2.0f * M_PI * k / (width - 1)) + 0.5f * 0.16f * cos(4.0f * M_PI * k / (width - 1));
}
