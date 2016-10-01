#include "mainwindow.h"
#include "QDebug"
#include "QString"

#include "qcustomplot.h"

//#ifdef Q_OS_WIN
//#include "ui_mainwindow_windows.h"
//#else
#include "ui_mainwindow96dpi.h"
//#endif


MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	drawPlot();
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
	const int length = 102400;
	Noise noise;
	std::shared_ptr<QVector<double>> signal = noise.getWhiteNoise(length);
	QVector<double> x;
	for(int i=0; i<length; ++i){
		x.append(i);
	}

	QCustomPlot *customPlot =  ui->input_signal;
	customPlot->addGraph();
	customPlot->graph(0)->setData(x, *signal);
	customPlot->xAxis->setRange(0, length);
	customPlot->yAxis->setRange(-2.5, 2.5);
	customPlot->replot();

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
	QCustomPlot *signalSpectrum = ui->signal_spectrum;
	signalSpectrum->addGraph();
	signalSpectrum->graph(0)->setData(xhist, hist);
	signalSpectrum->xAxis->setRange(0, hist_len);
	signalSpectrum->yAxis->setRange(0, *std::max_element(hist.begin(), hist.end()));
	signalSpectrum->replot();
}

void MainWindow::on_actionRegenerate_signal_triggered()
{
	drawPlot();
}
