#include "mainwindow.h"
#include "QDebug"
#include "QString"

#include <chrono>
#include <iostream>


//#ifdef Q_OS_WIN
//#include "ui_mainwindow_windows.h"
//#else
#include "ui_mainwindow96dpi.h"
//#endif
using namespace std::chrono;

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);



	signalSpectrum = ui->signal_spectrum;
	inputSignalPlot =  ui->input_signal;

	//inputSignalPlot->setOpenGl(true);


	drawPlot();

	high_resolution_clock::time_point t1 = high_resolution_clock::now();
	for(int i=0; i<60; ++i){
		redrawPlot();
	}
	high_resolution_clock::time_point t2 = high_resolution_clock::now();

	auto duration = duration_cast<milliseconds>( t2 - t1 ).count();

	std::cout << "Duration: " << duration << std::endl;

	//QTimer *timer = new QTimer(this);
	//connect(timer, SIGNAL(timeout()), this, SLOT(redrawPlot()));
	//timer->start(20);
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
	const int length = 10240;
	Noise noise;
	auto signal = noise.getWhiteNoise(length);
	x = make_unique<QVector<double>>();

	for(int i=0; i<length; ++i){
		x->append(i);
	}

	qDebug() << "xSize: " << x->size();
	qDebug() << "signalSize: " << signal->size();

	inputSignalPlot->addGraph();
	inputSignalPlot->graph(0)->setData(*x, *signal, true);
	inputSignalPlot->xAxis->setRange(0, length);
	inputSignalPlot->yAxis->setRange(-2.5, 2.5);
	inputSignalPlot->replot();

	//exit(EXIT_SUCCESS);

	//Probability histogram
	int hist_len = 700;
	QVector<double> hist(hist_len);
	for(int i=0; i<length; ++i){
		int index = round((*signal)[i]*100)+hist_len/2;
		if(index<0 || index >= hist_len){
			//printf("%f \n", index);
			continue;
		}
		hist[index] +=1;
	}
	QVector<double> xhist(hist_len);
	for(int i=0; i<hist_len; ++i){
		xhist[i] = i;
	}
	printf("drawing...");

	signalSpectrum->addGraph();
	signalSpectrum->graph(0)->setData(xhist, hist);
	signalSpectrum->xAxis->setRange(0, hist_len);
	signalSpectrum->yAxis->setRange(0, *std::max_element(hist.begin(), hist.end()));
	signalSpectrum->replot();
}

void MainWindow::redrawPlot()
{
	const int length = 10240;
	Noise noise;
	auto signal = Noise::getWhiteNoise(length);


	static int counter= 0;

	//inputSignalPlot->graph(0)->setData()
	inputSignalPlot->graph(0)->setData(*x, *signal, true);

	inputSignalPlot->xAxis->setRange(0, length);
	inputSignalPlot->yAxis->setRange(-2.5, 2.5);
	inputSignalPlot->replot();
}

void MainWindow::on_actionRegenerate_signal_triggered()
{
	drawPlot();
}
