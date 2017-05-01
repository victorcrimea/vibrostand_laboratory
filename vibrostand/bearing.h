#ifndef BEARING_H
#define BEARING_H

#include <chrono>
#include <cmath>
#include <ctime>
#include <memory>
#include <random>

#include "QVector"
#include <QDebug>
#include <QMap>
#include <QMetaEnum>
#include <QObject>
#include <QThread>
#include <QTimer>

//#include "fft.h"
//#include <kfr/dft.hpp>

//using namespace kfr;

class Bearing : QObject {
	Q_OBJECT
public:
	enum class BearingType {
		TYPE_403,
		TYPE_405,
		TYPE_406,
	};

	enum class DefectType {
		DEFECT_1,
		DEFECT_2,
		DEFECT_3,
		DEFECT_4,
		DEFECT_5
	};

	Bearing(BearingType type);
	Bearing();
	/**
	 * @brief getRPM provides real rpm of the bearing shaft
	 * @return
	 */
	int getRPM() const;

	/**
	 * @brief getVibration provides vibration signal of simulated bearing of desired length including defects
	 * @param length viration signal realisation length
	 * @return vector of vibration points
	 */
	const QVector<double> &getVibration() const;

	/**
	 * @brief getVibration provides vibration signal of simulated bearing of desired length without defects
	 * @param length viration signal realisation length
	 * @return vector of vibration points
	 */
	const QVector<double> &getVibrationGood() const;

	/**
	 * @brief getSpectrum provides spectrum representation of simulated bearing
	 * @return
	 */
	const QVector<double> &getSpectrum() const;

	void setDefect(DefectType type, double severity);
	void setDiscrFreq(int frequency);
	void setStepDuration(double duration);

	int getDiscrFreq() const;

	void nextStep();

public slots:
	void setDesiredRPM(int desiredRPM);

private slots:
	void calculateRPM();

signals:
	void RPMChanged(int rpm);

protected:
	double diameter;
	int balls;
	double ballsDiameter;

	int realShaftRPM;
	int desiredShaftRPM;

	double stepDuration;

	QVector<double> vibration;
	QVector<double> vibrationGood;

	QVector<double> spectrum;
	QVector<double> spectrumGood;

	/**
	 * Discretization time
	 */
	double dt = 0.001;

	QMap<DefectType, double> defects;

	void applyDefects(QVector<double> vibration);
	void generateVibration();
	void calculateSpectrum();

	void calculateSpectrumKFR();

	const double VIBRATION_AMPLITUDE = 1.0;
};

#endif // BEARING_H
