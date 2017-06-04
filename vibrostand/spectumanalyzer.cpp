#include "spectumanalyzer.h"

SpectumAnalyzer::SpectumAnalyzer(int dF, int realisationLength)
	: fft(realisationLength), realisationLength(realisationLength), dF(dF) {
}

Spectrum SpectumAnalyzer::calculate(const QVector<double> &signal) {
	auto out = calculateLinear(signal);
	for (auto &spectrumBin : out.second) {
		spectrumBin = -100 + 20 * log10(spectrumBin);
	}
	return out;
}

Spectrum SpectumAnalyzer::calculateLinear(const QVector<double> &signal) {

	if (signal.length() != realisationLength) {
		throw std::runtime_error("Signal length should be equal to analyser's length");
	}

	// Allocate memory for signal data
	double *pSignal = new double[realisationLength];
	double *pSpectrum = new double[realisationLength];

	// Fill signal array with data
	for (int i = 0; i < realisationLength; ++i) {
		pSignal[i] = signal.at(i);
	}

	// Apply FFT
	fft.do_fft(pSpectrum, pSignal);

	QVector<double> spectrum(realisationLength / 2 - 2);

	for (int i = 2; i <= realisationLength / 2 - 1; ++i) {
		int spectrum_i = i - 2;
		double real = pSpectrum[i];
		double imag = 0.0;
		if (i > 0 && i < realisationLength - 1)
			imag = pSpectrum[(realisationLength) / 2 + i];

		spectrum[spectrum_i] = sqrt(real * real + imag * imag);

		if (spectrum[spectrum_i] == 0)
			spectrum[spectrum_i] = DBL_MIN;

		//spectrum[spectrum_i] = 20 * log10(spectrum[spectrum_i]);
	}

	//Now need to calculate appropriate frequencies

	auto x = QVector<double>(spectrum.length());

	// Frequency step in spectrum
	double ds = dF / realisationLength;
	for (int i = 0; i < spectrum.length(); ++i) {
		x[i] = i * ds;
	}

	Spectrum out(x, spectrum);

	// Free memory
	delete[] pSignal;
	delete[] pSpectrum;

	return out;
}
