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

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	signalHist = ui->signal_hist;
	inputSignalPlot = ui->input_signal;
	signalSpectrum = ui->signal_spectrum;
	realisationlength = 102400;
	histLen = 700;
	inputSignalPlot->setOpenGl(false);
	signalHist->setOpenGl(false);

	drawPlot();

	// high_resolution_clock::time_point t1 = high_resolution_clock::now();
	// for(int i=0; i<60; ++i){
	//	redrawPlot();
	//}
	// high_resolution_clock::time_point t2 = high_resolution_clock::now();

	// auto duration = duration_cast<milliseconds>( t2 - t1 ).count();

	// std::cout << "Duration: " << duration << std::endl;

	QTimer *timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(redrawPlot()));
	const int fps = 1000;
	timer->start(1000 / fps);
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::on_exit_triggered()
{
	exit(EXIT_SUCCESS);
}

void MainWindow::drawPlot()
{
	Noise noise;
	auto signal = noise.getWhiteNoise(realisationlength);
	x = make_unique<QVector<double>>();

	for (int i = 0; i < realisationlength; ++i) {
		x->append(i);
	}

	qDebug() << "xSize: " << x->size();
	qDebug() << "signalSize: " << signal->size();

	inputSignalPlot->addGraph();
	inputSignalPlot->xAxis->setRange(0, realisationlength);
	inputSignalPlot->xAxis->setLabel("Входной вибросигнал");
	inputSignalPlot->yAxis->setRange(-2.5, 2.5);
	inputSignalPlot->replot();

	// Probability histogram
	signalHist->addGraph();
	signalHist->xAxis->setLabel("Гистограмма");
	signalHist->replot();

	// Signal spectum
	signalSpectrum->addGraph();
	signalSpectrum->xAxis->setRange(0, realisationlength);
	signalSpectrum->yAxis->setRange(0, 5);
	signalSpectrum->xAxis->setLabel("Спектр вибросигнала");
	signalSpectrum->replot();

	// Actual plotting
	replotSignal(signal);
	replotHist(signal);
	replotSpectrum(signal);
}

void MainWindow::redrawPlot()
{
	static auto time1 = high_resolution_clock::now();
	auto time2 = high_resolution_clock::now();
	auto diff = duration_cast<microseconds>(time2 - time1).count();
	time1 = time2;
	if (diff != 0) {
		const double realFPS = (1000000.0 / diff);
		qDebug() << "realFPS: " << realFPS;
		ui->statusBar->showMessage(QString("FPS: %1").arg(realFPS));
	}

	Noise noise;
	auto signal = Noise::getWhiteNoise(realisationlength);
	// std::thread t1(&MainWindow::replotSignal, this, signal);
	// std::thread t2(&MainWindow::replotHist, this, signal);
	// std::thread t3(&MainWindow::replotSpectrum, this, signal);

	replotSignal(signal);
	replotHist(signal);
	replotSpectrum(signal);
}

void MainWindow::on_actionRegenerate_signal_triggered()
{
	drawPlot();
}

QVector<double> MainWindow::hist(int histLength, shared_ptr<QVector<double>> data)
{
	QVector<double> hist(histLength);
	for (int i = 0; i < realisationlength; ++i) {
		int index = round((*data)[i] * 100) + histLength / 2;
		if (index < 0 || index >= histLength) {
			// printf("%f \n", index);
			continue;
		}
		hist[index] += 1;
	}
	return hist;
}

void MainWindow::replotSignal(std::shared_ptr<QVector<double>> signal)
{
	inputSignalPlot->graph(0)->setData(*x, *signal, true);
	inputSignalPlot->replot();
}

void MainWindow::replotHist(std::shared_ptr<QVector<double>> signal)
{
	// Probability histogram
	auto yHist = hist(histLen, signal);

	QVector<double> xhist(histLen);
	for (int i = 0; i < histLen; ++i) {
		xhist[i] = i;
	}

	signalHist->graph(0)->setData(xhist, yHist, true);
	signalHist->yAxis->setRange(0, *std::max_element(yHist.begin(), yHist.end()));
	signalHist->xAxis->setRange(0, histLen);
	signalHist->replot();
}

void MainWindow::replotSpectrum(std::shared_ptr<QVector<double>> signal)
{
	// Allocate memory for signal data
	complex *pSignal = new complex[realisationlength];

	// Fill signal array with data
	for (int i = 0; i < realisationlength; ++i) {
		pSignal[i] = signal->at(i);
	}
	// Apply FFT
	CFFT::Forward(pSignal, realisationlength);

	// Utilize transform result
	auto spectrum = make_shared<QVector<double>>(realisationlength);
	for (int i = 0; i < realisationlength; ++i) {
		(*spectrum)[i] = pSignal[i].re();
	}
	// Free memory
	delete[] pSignal;

	signalSpectrum->graph(0)->setData(*x, *spectrum, true);
	signalSpectrum->replot();
}
