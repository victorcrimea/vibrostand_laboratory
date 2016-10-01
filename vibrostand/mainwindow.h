#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <algorithm>

#include "noise.h"
#include "qcustomplot.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

private slots:
	void on_exit_triggered();
	void drawPlot();
	void redrawPlot();


	void on_actionRegenerate_signal_triggered();

private:
	Ui::MainWindow *ui;
	QCustomPlot *signalSpectrum;
	QCustomPlot *inputSignalPlot;
	std::shared_ptr<QVector<double>> x;
};

#endif // MAINWINDOW_H
