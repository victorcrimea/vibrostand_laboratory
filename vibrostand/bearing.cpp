#include "bearing.h"

Bearing::Bearing(Bearing::BearingType type) {
	qDebug() << "Bearing(Bearing::BearingType type)";
	switch (type) {
	case Bearing::BearingType::TYPE_403:
		diameter = 45.00;
		balls = 6;
		ballsDiameter = 12.7;
		break;
	case Bearing::BearingType::TYPE_405:
		diameter = 55.00;
		balls = 6;
		ballsDiameter = 16.67;
		break;
	case Bearing::BearingType::TYPE_406:
		diameter = 60.00;
		balls = 6;
		ballsDiameter = 19.05;
		break;
	}

	realShaftRPM = 0;
	desiredShaftRPM = 0;

	QTimer *updateTimer = new QTimer(this);
	connect(updateTimer, SIGNAL(timeout()), this, SLOT(calculateRPM()));
}

Bearing::Bearing() {
	Bearing(BearingType::TYPE_405);
}

int Bearing::getRPM() const {
	return realShaftRPM;
}

const QVector<double> &Bearing::getVibration() const {
	return vibration;
}

const QVector<double> &Bearing::getVibrationGood() const {
	return vibrationGood;
}

const QVector<double> &Bearing::getSpectrum() const {
	return spectrum;
}

const QVector<double> &Bearing::getSpectrumGood() const {
	return spectrumGood;
}

void Bearing::setDefect(Bearing::DefectType type, double severity) {
	defects[type] = severity;
}

void Bearing::setDiscrFreq(int frequency) {
	// If frequency is negative or zero - > set dt to 0.001
	if (frequency > 0) {
		dt = 1.0 / frequency;
	} else {
		dt = 0.001;
	}
}

void Bearing::setStepDuration(double duration) {
	this->stepDuration = duration;
}

int Bearing::getDiscrFreq() const {
	return static_cast<int>(ceil(1.0 / dt));
}

void Bearing::nextStep() {
	generateVibration();
	calculateSpectrum();
}

void Bearing::setDesiredRPM(int desiredRPM) {
	desiredShaftRPM = desiredRPM;
	realShaftRPM = desiredRPM;
}

void Bearing::calculateRPM() {
}

void Bearing::applyDefects() {
	//DEFECT_1

	//DEFECT_2
	double defectFrequency2 = realShaftRPM;
	double defectAmplitude2 = VIBRATION_AMPLITUDE * defects[DefectType::DEFECT_2] * 1;
	//QVector<double> defect2(vibration.length());
	//Generating sine defect
	double omega2 = (defectFrequency2 / 60.0) * 2 * M_PI;
	for (int i = 0; i < vibration.length(); ++i) {
		vibration[i] += defectAmplitude2 * sin(omega2 * dt * i);
		//qDebug() << "test";
	}

	//DEFECT_3
	double defectFrequency3 = realShaftRPM * 3;
	double defectAmplitude3 = VIBRATION_AMPLITUDE * defects[DefectType::DEFECT_3] * 0.5;
	//QVector<double> defect2(vibration.length());
	//Generating sine defect
	double omega3 = (defectFrequency3 / 60.0) * 2 * M_PI;
	for (int i = 0; i < vibration.length(); ++i) {
		vibration[i] += defectAmplitude3 * sin(omega3 * dt * i);
		//qDebug() << "test";
	}

	//DEFECT_4

	//DEFECT_5
}

void Bearing::generateVibration() {
	size_t realisationLength = static_cast<int>(ceil(stepDuration / dt));

	std::random_device rd;
	auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
	std::mt19937 gen(seed);
	//auto vibration = std::make_shared<QVector<double>>();
	//std::normal_distribution<double> d(0, 1);
	std::normal_distribution<double> d(0, 1);

	vibrationGood.resize(realisationLength);
	for (size_t i = 0; i < realisationLength; ++i) {
		vibrationGood[i] = d(gen);
	}

	//	double omega = (realShaftRPM / 60.0) * 2 * M_PI;

	vibration = QVector<double>(vibrationGood);
	//	qDebug() << "Duration = " << stepDuration << "; dt= " << dt << "; omega=" << omega;
	//	for (size_t i = 0; i < realisationLength; ++i) {
	//		//vibration->append(sin(fmod((omega * dt * i), 360) * M_PI / 180));

	//		vibration[i] += sin(omega * dt * i);
	//	}

	qDebug() << "vibration.length()" << vibration.length();

	//Add required defects
	applyDefects();

	qDebug() << "Bearing RPM: " << realShaftRPM;
	//this->vibration = vibration;
}

void Bearing::calculateSpectrum() {
	size_t vibrationLength = vibration.length();

	ffft::FFTReal<double> fft_object(vibrationLength);

	// Allocate memory for signal data
	double *pSignal = new double[vibrationLength];
	double *pSpectrum = new double[vibrationLength];

	// Fill signal array with data
	for (int i = 0; i < vibrationLength; ++i) {
		pSignal[i] = vibration.at(i);
	}

	// Apply FFT
	//CFFT::Forward(pSignal, vibrationLength * 2);
	fft_object.do_fft(pSpectrum, pSignal);

	// Utilize transform result
	//auto spectrum = make_shared<QVector<double>>(vibration.length());

	spectrum.resize(vibrationLength / 2);

	for (int i = 2; i <= vibrationLength / 2 - 1; ++i) {
		int spectrum_i = i - 2;
		double real = pSpectrum[i];
		double imag = 0.0;
		if (i > 0 && i < vibrationLength - 1)
			imag = pSpectrum[(vibrationLength) / 2 + i];
		spectrum[spectrum_i] = sqrt(real * real + imag * imag);
		if (spectrum[spectrum_i] == 0)
			spectrum[spectrum_i] = 0.0001;
		spectrum[spectrum_i] = -100 + 20 * log10(spectrum[spectrum_i]);
	}

	//==================== Spectrum good =================================
	//====================================================================

	// Fill signal array with data
	for (int i = 0; i < vibrationGood.length(); ++i) {
		pSignal[i] = vibrationGood.at(i);
	}

	// Apply FFT
	//CFFT::Forward(pSignal, vibrationGood.length() * 2);
	fft_object.do_fft(pSpectrum, pSignal);

	// Utilize transform result
	//auto spectrum = make_shared<QVector<double>>(vibration.length());

	spectrumGood.resize(vibrationLength / 2);

	for (int i = 2; i <= vibrationLength / 2 - 1; ++i) {
		int spectrum_i = i - 2;
		double real = pSpectrum[i];
		double imag = 0.0;
		if (i > 0 && i < vibrationLength - 1)
			imag = pSpectrum[(vibrationLength) / 2 + i];
		spectrumGood[spectrum_i] = sqrt(real * real + imag * imag);
		if (spectrumGood[spectrum_i] == 0)
			spectrumGood[spectrum_i] = 0.0001;
		spectrumGood[spectrum_i] = -100 + 20 * log10(spectrumGood[spectrum_i]);
	}

	// Free memory
	delete[] pSignal;
	delete[] pSpectrum;
}

void Bearing::calculateSpectrumKFR() {
	//univector<double, tag_dynamic_vector> data = vibration.toStdVector();
}
