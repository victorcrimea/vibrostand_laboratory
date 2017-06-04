#include "mainwindow.h"
#include "QDebug"
#include "QString"

#include <chrono>
#include <iostream>
#include <thread>

//#ifdef Q_OS_WIN
//#include "ui_mainwindow_windows.h"
//#else
#include "ui_mainwindow96dpi.h"
//#endif
using namespace std::chrono;
using std::shared_ptr;
using std::make_unique;
using std::make_shared;

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent), ui(new Ui::MainWindow) {
	ui->setupUi(this);

	signalHist = ui->signal_hist;
	inputSignalPlot = ui->input_signal;
	signalSpectrum = ui->signal_spectrum;
	//realisationLength = 1;
	duration = 0.1;
	histLen = 700;
	inputSignalPlot->setOpenGl(false);
	signalHist->setOpenGl(false);
	signalSpectrum->setOpenGl(false);

	bearing.setDiscrFreq(40960);
	bearing.setDesiredRPM(5000);
	bearing.setStepDuration(duration);
	bearing.nextStep();

	drawPlot();

	QTimer *timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(redrawPlot()));
	const int fps = 16;
	timer->start(1000 / fps);
}

MainWindow::~MainWindow() {
	delete ui;
}

void MainWindow::on_exit_triggered() {
	exit(EXIT_SUCCESS);
}

void MainWindow::drawPlot() {
	auto signal = bearing.getVibration();
	auto signalGood = bearing.getVibrationGood();
	auto x = QVector<double>(signal.length());
	for (int i = 0; i < signal.length(); ++i) {
		x[i] = i;
	}

	qDebug() << "xSize: " << x.size();
	qDebug() << "signalSize: " << signal.size();

	inputSignalPlot->setBackground(Qt::lightGray);
	inputSignalPlot->addGraph();
	inputSignalPlot->axisRect()->setBackground(Qt::black);

	inputSignalPlot->graph(0)->setPen(QPen(Qt::yellow, 0.5));

	inputSignalPlot->addGraph();
	inputSignalPlot->graph(1)->setPen(QPen(Qt::green, 0.5));
	inputSignalPlot->xAxis->setRange(0, signal.length());
	inputSignalPlot->xAxis->setLabel("Входной вибросигнал, ceк");
	inputSignalPlot->yAxis->setRange(-5, 5);
	inputSignalPlot->yAxis->setLabel("mV");
	inputSignalPlot->replot();

	// Probability histogram
	signalHist->setBackground(Qt::lightGray);
	signalHist->axisRect()->setBackground(Qt::black);
	signalHist->addGraph();
	signalHist->xAxis->setLabel("АЧХ фильтра");
	signalHist->replot();

	// Signal spectum
	signalSpectrum->setBackground(Qt::lightGray);
	signalSpectrum->axisRect()->setBackground(Qt::black);
	signalSpectrum->addGraph();
	signalSpectrum->addGraph();
	signalSpectrum->graph(0)->setPen(QPen(Qt::yellow, 0.5));
	signalSpectrum->graph(1)->setPen(QPen(Qt::green, 0.5));
	signalSpectrum->xAxis->setRange(0, signal.length() / 2);
	signalSpectrum->yAxis->setRange(-80, 10);
	signalSpectrum->xAxis->setLabel("Спектр вибросигнала, Hz");
	signalSpectrum->yAxis->setLabel("dB");
	signalSpectrum->replot();

	// Actual plotting
	replotSignal();
	//replotHist(signal);
	replotSpectrum();
}

void MainWindow::redrawPlot() {
	//Update GUI data
	//	ui->shaft_rpm_real->display(bearing.getRPM());

	static auto time1 = high_resolution_clock::now();
	auto time2 = high_resolution_clock::now();
	auto diff = duration_cast<microseconds>(time2 - time1).count();
	time1 = time2;
	if (diff != 0) {
		const double realFPS = (1000000.0 / diff);
		//qDebug() << "realFPS: " << realFPS;
		ui->statusBar->showMessage(QString("FPS: %1").arg(realFPS));
	}

	replotSignal();
	//replotHist(signal);
	replotSpectrum();

	//Ask bearing to generate next frame of data
	bearing.nextStep();
}

void MainWindow::on_actionRegenerate_signal_triggered() {
	drawPlot();
}

QVector<double> MainWindow::hist(int histLength, const QVector<double> &data) {
	QVector<double> hist(histLength);
	for (int i = 0; i < data.length(); ++i) {
		int index = round(data[i] * 100) + histLength / 2;
		if (index < 0 || index >= histLength) {
			// printf("%f \n", index);
			continue;
		}
		hist[index] += 1;
	}
	return hist;
}

void MainWindow::replotSignal() {
	auto signal = bearing.getVibration();
	auto signalGood = bearing.getVibrationGood();
	auto x = QVector<double>(signal.length());

	for (int i = 0; i < signal.length(); ++i) {
		x[i] = i * (duration / signal.length());
	}

	inputSignalPlot->xAxis->setRange(0, duration);
	inputSignalPlot->graph(0)->setData(x, signal, true);
	inputSignalPlot->graph(1)->setData(x, signalGood, true);
	inputSignalPlot->replot();
}

void MainWindow::replotHist(const QVector<double> &signal) {
	// Probability histogram
	auto yHist = hist(histLen, signal);

	QVector<double> xhist(histLen);
	for (int i = 0; i < histLen; ++i) {
		xhist[i] = i;
	}

	signalHist->graph(0)->setData(xhist, yHist, true);
	signalHist->yAxis->setRange(0, 15);
	signalHist->xAxis->setRange(0, histLen);
	signalHist->replot();
}

void MainWindow::replotSpectrum() {

	auto spectrum = bearing.getSpectrum();
	auto spectrumGood = bearing.getSpectrumGood();
	auto x = QVector<double>(spectrum.length());
	for (int i = 0; i < spectrum.length(); ++i) {
		x[i] = i * (spectrum.length() * 4 / 1000.0);
	}

	signalSpectrum->xAxis->setRange(0, spectrum.length());
	signalSpectrum->graph(0)->setData(x, spectrum, true);
	signalSpectrum->graph(1)->setData(x, spectrumGood, true);
	signalSpectrum->replot();
}

//Connection between dial rpm control and rpm_window
void MainWindow::on_shaft_rpm_dial_valueChanged(int value) {
	ui->shaft_rpm->setValue(value);
}

//Connection between dial rpm control and rpm_window
void MainWindow::on_shaft_rpm_valueChanged(int value) {
	ui->shaft_rpm_dial->setValue(value);
	this->bearing.setDesiredRPM(value);
	ui->shaft_rpm_real->display((int)(value / 60.0));
}

void MainWindow::on_shaft_rpm_editingFinished() {
	int value = 0;
	value = ui->shaft_rpm->value();
	ui->shaft_rpm_dial->setValue(value);

	//Change desired speed of the shaft
}

void MainWindow::on_defect1Slider_valueChanged(int value) {
	double severity = value / 100.0;
	ui->defect1Spin->setValue(severity);
	this->bearing.setDefect(Bearing::DefectType::DEFECT_1, severity);
}

void MainWindow::on_defect2Slider_valueChanged(int value) {
	double severity = value / 100.0;
	ui->defect2Spin->setValue(severity);
	this->bearing.setDefect(Bearing::DefectType::DEFECT_2, severity);
}

void MainWindow::on_defect3Slider_valueChanged(int value) {
	double severity = value / 100.0;
	ui->defect3Spin->setValue(severity);
	this->bearing.setDefect(Bearing::DefectType::DEFECT_3, severity);
}

void MainWindow::on_defect4Slider_valueChanged(int value) {
	double severity = value / 100.0;
	ui->defect4Spin->setValue(severity);
	this->bearing.setDefect(Bearing::DefectType::DEFECT_4, severity);
}

void MainWindow::on_defect5Slider_valueChanged(int value) {
	double severity = value / 100.0;
	ui->defect5Spin->setValue(severity);
	this->bearing.setDefect(Bearing::DefectType::DEFECT_5, severity);
}

void MainWindow::on_defect1Spin_valueChanged(double arg1) {
	ui->defect1Slider->setValue((int)(arg1 * 100.0));
}

void MainWindow::on_defect2Spin_valueChanged(double arg1) {
	ui->defect2Slider->setValue((int)(arg1 * 100.0));
}

void MainWindow::on_defect3Spin_valueChanged(double arg1) {
	ui->defect3Slider->setValue((int)(arg1 * 100.0));
}

void MainWindow::on_defect4Spin_valueChanged(double arg1) {
	ui->defect4Slider->setValue((int)(arg1 * 100.0));
}

void MainWindow::on_defect5Spin_valueChanged(double arg1) {
	ui->defect5Slider->setValue((int)(arg1 * 100.0));
}

void MainWindow::on_discr_freq_valueChanged(int arg1) {
	bearing.setDiscrFreq(arg1);
}
