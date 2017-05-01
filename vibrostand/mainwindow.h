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
	void replotHist(const QVector<double> &signal);
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

private:
	Ui::MainWindow *ui;
	QCustomPlot *signalHist;
	QCustomPlot *inputSignalPlot;
	QCustomPlot *signalSpectrum;
	//int realisationLength;

	double duration;

	int histLen;
	Bearing bearing;

	QVector<double> hist(int histLength, const QVector<double> &data);
};

#endif // MAINWINDOW_H
