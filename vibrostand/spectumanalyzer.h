#ifndef SPECTUMANALYZER_H
#define SPECTUMANALYZER_H

#include <cfloat>

#include <QObject>
#include <QPair>
#include <QVector>

#include "ffft/FFTReal.h"

//Spectrum is a frequencies with according amplitudes
typedef QPair<QVector<double>, QVector<double>> Spectrum;

class SpectumAnalyzer : public QObject {
	Q_OBJECT
public:
	explicit SpectumAnalyzer(int dF, int realisationLength);

	/**
	 * @brief Method for spectrum calculation
	 * @param signal - iput signal
	 * @return amplitude spectrum of the input signal
	 */
	Spectrum calculate(const QVector<double> &signal);

	Spectrum calculateLinear(const QVector<double> &signal);

private:
	ffft::FFTReal<double> fft;

	int realisationLength;

	int dF;
};

#endif // SPECTUMANALYZER_H
