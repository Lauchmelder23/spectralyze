#pragma once
#define _USE_MATH_DEFINES
#include <math.h>
#include <vector>
#include <complex>

#define TWO_PI (double)6.28318530718f
#define POW_OF_TWO(x) (x && !(x & (x - 1)))

using namespace std::complex_literals;

std::vector<std::complex<double>> radix2dit(const std::vector<double>::const_iterator& begin, size_t N, size_t s)
{
	std::vector<std::complex<double>> output(N);
	if (N == 1) 
	{
		output[0] = *begin;
	}
	else
	{
		size_t halfN = N >> 1;
		std::vector<std::complex<double>> first = radix2dit(begin, halfN, s << 1);
		std::vector<std::complex<double>> second = radix2dit(begin + s, halfN, s << 1);

		/*if (s == 1) {
			std::future<std::vector<std::complex<double>>> firstFuture = std::async(&radix2dit, begin, halfN, s << 1);
			std::future<std::vector<std::complex<double>>> secondFuture = std::async(&radix2dit, begin + s, halfN, s << 1);

			first = firstFuture.get();
			second = secondFuture.get();
		}
		else {
			first = radix2dit(begin, halfN, s << 1);
			second = radix2dit(begin + 1, halfN, s << 1);
		}*/

		std::complex<double> coeff = -M_PI * 1.0i / (double)halfN;

		for (int k = 0; k < N >> 1; k++)
		{
			std::complex<double> p = first[k];
			std::complex<double> q = std::exp(coeff * (double)k) * second[k];

			output[k] = p + q;
			output[halfN + k] = p - q;
		}
	}

	return output;
}

std::vector<std::pair<double, double>> FFT(const std::vector<double>::const_iterator& begin, const std::vector<double>::const_iterator& end, size_t sampleRate)
{
	std::vector<double> signal(begin, end);
	size_t N = signal.size();
	while (!POW_OF_TWO(N))
	{
		// Pad with zeros
		signal.push_back(0.0f);
		N++;
	}

	std::vector<std::complex<double>> spectrum = radix2dit(signal.cbegin(), N, 1);
	double freqRes = (double)sampleRate / (double)N;
	double nyquistLimit = (double)sampleRate / 2.0f;

	std::vector<std::pair<double, double>> output;
	double freq = 0.0f;
	for (int k = 0; freq < nyquistLimit; k++) 
	{
		output.push_back(std::make_pair(freq, 2.0f * std::abs(spectrum[k]) / (double)N));
		freq += freqRes;
	}

	return output;
}
