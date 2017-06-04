#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtCharts>
#include <algorithm>

#include <QVector>
#include <functional> //std::bind
#include <thread>     // std::thread

#include "bearing.h"
//#include "fft.h"
#include "qcustomplot.h"
#include "spectumanalyzer.h"
#include "windowfilter.h"

//#include <kfr/dft.hpp>
//using namespace kfr;

QT_CHARTS_USE_NAMESPACE

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

	void replotSignal();
	void replotFR();
	void replotSpectrum();

private slots:
	void on_exit_triggered();
	void drawPlot();
	void redrawPlot();

	void on_actionRegenerate_signal_triggered();

	void on_shaft_rpm_dial_valueChanged(int value);

	void on_shaft_rpm_valueChanged(int arg1);

	void on_shaft_rpm_editingFinished();

	void on_defect1Slider_valueChanged(int value);

	void on_defect2Slider_valueChanged(int value);

	void on_defect3Slider_valueChanged(int value);

	void on_defect4Slider_valueChanged(int value);

	void on_defect5Slider_valueChanged(int value);

	void on_defect1Spin_valueChanged(double arg1);

	void on_defect2Spin_valueChanged(double arg1);

	void on_defect3Spin_valueChanged(double arg1);

	void on_defect4Spin_valueChanged(double arg1);

	void on_defect5Spin_valueChanged(double arg1);

	void on_discr_freq_valueChanged(int arg1);

	void on_desiredFPS_valueChanged(int arg1);

	void on_left_bound_valueChanged(int leftBound);

	void on_right_bound_valueChanged(int rightBound);

	void on_high_cutoff_valueChanged(int arg1);

	void on_spinBox_valueChanged(int arg1);

	void on_comboBox_3_currentIndexChanged(const QString &arg1);

	void on_low_cutoff_valueChanged(int arg1);

	void on_left_bound_2_valueChanged(int arg1);

	void on_right_bound_2_valueChanged(int arg1);

private:
	Ui::MainWindow *ui;
	QCustomPlot *filterFr;
	QCustomPlot *filterFrBig;
	QCustomPlot *inputSignalPlot;
	QCustomPlot *signalSpectrum;
	QCustomPlot *signalSpectrumBig;

	QPair<int, int> manageCutoff();

	/**
	 * @brief Simulation time to be shown in 'signal' window.
	 */
	double duration = 0.1;

	/**
	 * @brief Discretisation frequency. May be changed via GUI.
	 */
	double dF = 40960;

	/**
	 * @brief Subject bearing. May be with defects.
	 */
	Bearing bearingA;

	/**
	 * @brief Reference bearing. Without defects.
	 */
	Bearing bearingB;

	/**
	 * @brief Filter device
	 */
	WindowFilter filter;

	/**
	 * @brief Spectrum analyzer device
	 */
	SpectumAnalyzer analyzer;

	QTimer replotTimer;
};

#endif // MAINWINDOW_H
