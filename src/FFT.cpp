#include "FFT.hpp"

#define _USE_MATH_DEFINES
#include <math.h>
#include <functional>
#include <iostream>
#include <map>

#define POW_OF_TWO(x) (x && !(x & (x - 1)))

using namespace std::complex_literals;

typedef std::function<double(unsigned int)> WindowFunction;

inline double WindowRectangle(unsigned int k, unsigned int offset, unsigned int width);
inline double WindowVonHann(unsigned int k, unsigned int offset, unsigned int width);
inline double WindowGauss(unsigned int k, unsigned int offset, unsigned int width);

std::vector<std::complex<double>> 
radix2dit(
	const std::vector<double>& list,
	size_t offset,
	size_t N, 
	size_t s,
	WindowFunction winFunc)
{
	std::vector<std::complex<double>> output(N);
	if (N == 1)
	{
		output[0] = winFunc(offset) * (list[offset]);
	}
	else
	{
		size_t halfN = N >> 1;
		std::vector<std::complex<double>> first = radix2dit(list, offset, halfN, s << 1, winFunc);
		std::vector<std::complex<double>> second = radix2dit(list, offset + s, halfN, s << 1, winFunc);

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
	WindowFunctions func, unsigned int width, unsigned int offset)
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
	switch (func)
	{
	case WindowFunctions::RECTANGLE:	f = std::bind(WindowRectangle, std::placeholders::_1, offset, width); break;
	case WindowFunctions::VON_HANN:		f = std::bind(WindowVonHann, std::placeholders::_1, offset, width); break;
	case WindowFunctions::GAUSS:		f = std::bind(WindowGauss, std::placeholders::_1, offset, width); break;
	} 


	std::vector<std::complex<double>> spectrum = radix2dit(signal, 0, N, 1, f);
	double freqRes = (double)sampleRate / (double)N;
	double nyquistLimit = (double)sampleRate / 2.0f;

	std::vector<std::pair<double, double>> output;
	double freq = minFreq;
	if (maxFreq == 0)
		maxFreq = nyquistLimit;

	for (int k = freq / freqRes; freq < nyquistLimit && freq < maxFreq; k++)
	{
		output.push_back(std::make_pair(freq, 2.0f * std::abs(spectrum[k]) / (double)N));
		freq += freqRes;
	}

	return output;
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
