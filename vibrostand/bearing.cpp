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
	Bearing(BearingType::TYPE_403);
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

void Bearing::applyDefects(QVector<double> vibration) {
	//DEFECT_1

	//DEFECT_2
	double defectFrequency2 = realShaftRPM * 0.018;
	double defectAmplitude2 = VIBRATION_AMPLITUDE * 30.5;
	QVector<double> defect2(vibration.length());
	//Generating sine defect
	for (size_t i = 0; i < vibration.length(); ++i) {
		//vibration[i] += sin(fmod((i), 360) * M_PI / 180);
		//qDebug() << "test";
	}

	//DEFECT_3

	//DEFECT_4

	//DEFECT_5
}

void Bearing::generateVibration() {
	size_t realisationLength = static_cast<int>(ceil(stepDuration / dt));

	std::random_device rd;
	auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
	std::mt19937 gen(seed);
	//auto vibration = std::make_shared<QVector<double>>();
	std::normal_distribution<double> d(0, 1);

	vibrationGood.resize(realisationLength);
	for (int i = 0; i < realisationLength; ++i) {
		vibrationGood[i] = d(gen);
	}

	double omega = (realShaftRPM / 60.0) * 2 * M_PI;

	vibration = QVector<double>(vibrationGood);
	qDebug() << "Duration = " << stepDuration << "; dt= " << dt << "; omega=" << omega;
	for (size_t i = 0; i < realisationLength; ++i) {
		//vibration->append(sin(fmod((omega * dt * i), 360) * M_PI / 180));

		vibration[i] += sin(omega * dt * i);
	}

	qDebug() << "vibration.length()" << vibration.length();
	//Add required defects
	//applyDefects(*vibration);

	qDebug() << "Bearing RPM: " << realShaftRPM;
	//this->vibration = vibration;
}

void Bearing::calculateSpectrum() {
	//	// Allocate memory for signal data
	//	complex *pSignal = new complex[vibration.length() * 2];

	//	// Fill signal array with data
	//	for (int i = 0; i < vibration.length(); ++i) {
	//		pSignal[i] = vibration.at(i);
	//	}

	//	for (int i = vibration.length(); i < vibration.length() * 2; ++i) {
	//		pSignal[i] += vibration.at(i - vibration.length());
	//	}

	//	// Apply FFT
	//	CFFT::Forward(pSignal, vibration.length() * 2);

	//	// Utilize transform result
	//	//auto spectrum = make_shared<QVector<double>>(vibration.length());

	//	spectrum.resize(vibration.length());

	//	for (int i = 0; i < vibration.length(); ++i) {
	//		spectrum[i] = pSignal[i].re();
	//	}
	//	// Free memory
	//	delete[] pSignal;
}

void Bearing::calculateSpectrumKFR() {
	//univector<double, tag_dynamic_vector> data = vibration.toStdVector();
}
