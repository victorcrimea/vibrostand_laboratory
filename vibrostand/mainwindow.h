#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtCharts>
#include <algorithm>

#include <QVector>
#include <functional> //std::bind
#include <thread>     // std::thread

#include "fft.h"
#include "noise.h"
#include "qcustomplot.h"

QT_CHARTS_USE_NAMESPACE

namespace Ui
{
class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

	void replotSignal(std::shared_ptr<QVector<double>> signal);
	void replotHist(std::shared_ptr<QVector<double>> signal);
	void replotSpectrum(std::shared_ptr<QVector<double>> signal);

private slots:
	void on_exit_triggered();
	void drawPlot();
	void redrawPlot();

	void on_actionRegenerate_signal_triggered();

private:
	Ui::MainWindow *ui;
	QCustomPlot *signalHist;
	QCustomPlot *inputSignalPlot;
	QCustomPlot *signalSpectrum;
	std::shared_ptr<QVector<double>> x;
	int realisationlength;
	int histLen;

	QVector<double> hist(int histLength, shared_ptr<QVector<double>> data);
};

#endif // MAINWINDOW_H
