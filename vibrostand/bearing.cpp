#include "bearing.h"

Bearing::Bearing(Bearing::BearingType type) {
	qDebug() << "Bearing(Bearing::BearingType type)";

	changeType(type);

	realShaftRPM = 0;
	desiredShaftRPM = 0;

	QTimer *updateTimer = new QTimer(this);
	connect(updateTimer, SIGNAL(timeout()), this, SLOT(calculateRPM()));
}

Bearing::Bearing() {
	Bearing(BearingType::TYPE_405);
}

void Bearing::changeType(Bearing::BearingType type) {
	switch (type) {
	case Bearing::BearingType::TYPE_403:
		diameter = 45.00;
		balls = 6.0;
		ballsDiameter = 12.7;
		break;
	case Bearing::BearingType::TYPE_405:
		diameter = 55.00;
		balls = 6.0;
		ballsDiameter = 16.67;
		break;
	case Bearing::BearingType::TYPE_406:
		diameter = 60.00;
		balls = 6.0;
		ballsDiameter = 19.05;
		break;
	}
}

int Bearing::getRPM() const {
	return realShaftRPM;
}

const QVector<double> &Bearing::getVibration() const {
	return vibration;
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
}

void Bearing::setDesiredRPM(int desiredRPM) {
	desiredShaftRPM = desiredRPM;
	realShaftRPM = desiredRPM;
}

void Bearing::calculateRPM() {
}

double Bearing::getBallsDiameter() const {
	return ballsDiameter;
}

int Bearing::getBalls() const {
	return balls;
}

double Bearing::getDiameter() const {
	return diameter;
}

void Bearing::applyDefects() {
	//	//Defects' frequencies depends only on real shaft speed
	const auto F_shaft = (realShaftRPM / 60.0);

	const auto F_separator = (F_shaft / 2) * (1 - ballsDiameter / diameter);

	const auto F_balls = (F_shaft / 2) * (diameter / ballsDiameter) * (1 - (ballsDiameter / diameter) * (ballsDiameter / diameter));

	const auto F_outer = balls * (F_shaft / 2) * (1 - ballsDiameter / diameter);

	const auto F_inner = balls * (F_shaft / 2) * (1 + ballsDiameter / diameter);

	//	//DEFECT_1
	const auto f1 = F_outer + F_separator;
	for (int i = 0; i < vibration.length(); ++i) {
		vibration[i] += defects[DefectType::DEFECT_1] * sin(f1 * 2 * M_PI * dt * i);
	}

	//DEFECT_2
	const auto f21 = F_outer + F_inner;
	for (int i = 0; i < vibration.length(); ++i) {
		vibration[i] += defects[DefectType::DEFECT_2] * sin(f21 * 2 * M_PI * dt * i);
	}

	const auto f22 = (F_inner - F_separator) * balls;
	for (int i = 0; i < vibration.length(); ++i) {
		vibration[i] += defects[DefectType::DEFECT_2] * sin(f22 * 2 * M_PI * dt * i);
	}

	//DEFECT_3
	const auto f3 = F_separator + balls;
	for (int i = 0; i < vibration.length(); ++i) {
		vibration[i] += defects[DefectType::DEFECT_3] * sin(f3 * 2 * M_PI * dt * i);
	}

	//DEFECT_4
	const auto f4 = (F_shaft - F_separator) * balls;
	for (int i = 0; i < vibration.length(); ++i) {
		vibration[i] += defects[DefectType::DEFECT_4] * sin(f4 * 2 * M_PI * dt * i);
	}

	//DEFECT_5
	const auto f5 = 2 * (diameter / ballsDiameter) * (1 + ballsDiameter / diameter) + 1;

	for (int i = 0; i < vibration.length(); ++i) {
		vibration[i] += defects[DefectType::DEFECT_5] * sin(f5 * 2 * M_PI * dt * i);
	}
}

void Bearing::generateVibration() {
	size_t realisationLength = static_cast<int>(ceil(stepDuration / dt));

	std::random_device rd;
	auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
	std::mt19937 gen(seed);
	//auto vibration = std::make_shared<QVector<double>>();
	//std::normal_distribution<double> d(0, 1);
	std::normal_distribution<double> d(0, 1);

	vibration.resize(realisationLength);
	for (size_t i = 0; i < realisationLength; ++i) {
		vibration[i] = d(gen);
	}

	qDebug() << "vibration.length()" << vibration.length();

	//Add required defects
	applyDefects();

	qDebug() << "Bearing RPM: " << realShaftRPM;
	//this->vibration = vibration;
}

void Bearing::calculateSpectrumKFR() {
	//univector<double, tag_dynamic_vector> data = vibration.toStdVector();
}
