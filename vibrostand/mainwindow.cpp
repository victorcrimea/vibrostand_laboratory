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
: QMainWindow(parent),
ui(new Ui::MainWindow),
analyzer(dF, duration * dF),
replotTimer(this),
bearingA(Bearing::BearingType::TYPE_403),
bearingB(Bearing::BearingType::TYPE_403) {
	ui->setupUi(this);

	filterFr = ui->filter_FR;
	filterFrBig = ui->filter_FR_big;
	inputSignalPlot = ui->input_signal;
	signalSpectrum = ui->signal_spectrum;
	signalSpectrumBig = ui->signal_spectrum_big;

	//Disable OpenGl rendering
	inputSignalPlot->setOpenGl(false);
	filterFr->setOpenGl(false);
	filterFrBig->setOpenGl(false);
	signalSpectrum->setOpenGl(false);
	signalSpectrumBig->setOpenGl(false);

	//Set bearings' parameters
	bearingA.setDiscrFreq(dF);
	bearingA.setDesiredRPM(ui->shaft_rpm->value());
	bearingA.setStepDuration(duration);
	bearingA.nextStep();

	bearingB.setDiscrFreq(dF);
	bearingB.setDesiredRPM(ui->shaft_rpm->value());
	bearingB.setStepDuration(duration);
	bearingB.nextStep();

	filter.setHighCutoffFrequency(ui->high_cutoff->value());
	filter.setSamplingFrequency(dF);

	ui->z->setValue(bearingA.getBalls());
	ui->bearing_diameter->setValue(bearingA.getDiameter());
	ui->balls_diameter->setValue(bearingA.getBallsDiameter());

	drawPlot();

	connect(&replotTimer, SIGNAL(timeout()), this, SLOT(redrawPlot()));
	const int fps = ui->desiredFPS->value();
	replotTimer.start(1000 / fps);
}

MainWindow::~MainWindow() {
	delete ui;
}

void MainWindow::on_exit_triggered() {
	exit(EXIT_SUCCESS);
}

void MainWindow::drawPlot() {
	auto signal = bearingA.getVibration();
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

	// Filter frequency response
	filterFr->setBackground(Qt::lightGray);
	filterFr->axisRect()->setBackground(Qt::black);
	filterFr->addGraph();
	filterFr->graph(0)->setPen(QPen(Qt::green, 0.5));
	filterFr->xAxis->setLabel("АЧХ фильтра");
	filterFr->yAxis->setLabel("dB");
	filterFr->replot();

	filterFrBig->setBackground(Qt::lightGray);
	filterFrBig->axisRect()->setBackground(Qt::black);
	filterFrBig->addGraph();
	filterFrBig->graph(0)->setPen(QPen(Qt::green, 0.5));
	filterFrBig->xAxis->setLabel("АЧХ фильтра");
	filterFrBig->replot();

	// Signal spectum
	signalSpectrum->setBackground(Qt::lightGray);
	signalSpectrum->axisRect()->setBackground(Qt::black);
	signalSpectrum->addGraph();
	signalSpectrum->addGraph();
	signalSpectrum->graph(0)->setPen(QPen(Qt::yellow, 0.5));
	signalSpectrum->graph(1)->setPen(QPen(Qt::green, 0.5));
	signalSpectrum->yAxis->setRange(-80, 10);
	signalSpectrum->xAxis->setRange(ui->left_bound->value(), ui->right_bound->value());
	signalSpectrum->xAxis->setLabel("Спектр вибросигнала, Hz");
	signalSpectrum->yAxis->setLabel("dB");
	signalSpectrum->replot();

	signalSpectrumBig->setBackground(Qt::lightGray);
	signalSpectrumBig->axisRect()->setBackground(Qt::black);
	signalSpectrumBig->addGraph();
	signalSpectrumBig->addGraph();
	signalSpectrumBig->graph(0)->setPen(QPen(Qt::yellow, 0.5));
	signalSpectrumBig->graph(1)->setPen(QPen(Qt::green, 0.5));
	signalSpectrumBig->yAxis->setRange(-80, 10);
	signalSpectrumBig->xAxis->setRange(ui->left_bound->value(), ui->right_bound->value());
	signalSpectrumBig->xAxis->setLabel("Спектр вибросигнала, Hz");
	signalSpectrumBig->yAxis->setLabel("dB");
	signalSpectrumBig->replot();

	// Actual plotting
	replotSignal();
	replotFR();
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
	replotFR();
	replotSpectrum();

	//Ask bearing to generate next frame of data
	bearingA.nextStep();
	bearingB.nextStep();
}

void MainWindow::on_actionRegenerate_signal_triggered() {
	drawPlot();
}

void MainWindow::replotSignal() {
	if (ui->tabWidget->currentIndex() == 0) {
		auto signal = bearingA.getVibration();
		auto signalGood = bearingB.getVibration();
		auto x = QVector<double>(signal.length());

		for (int i = 0; i < signal.length(); ++i) {
			x[i] = i * (duration / signal.length());
		}

		inputSignalPlot->xAxis->setRange(0, duration);
		inputSignalPlot->graph(0)->setData(x, signal, true);
		inputSignalPlot->graph(1)->setData(x, signalGood, true);
		inputSignalPlot->replot();
	}
}

void MainWindow::replotFR() {
	if (ui->tabWidget->currentIndex() == 0 || ui->tabWidget->currentIndex() == 1) {
		// Calculating filter frequency response
		auto signal = bearingA.getVibration();
		auto filtered = filter.filter(signal);

		auto signalSpectrum = analyzer.calculateLinear(signal);
		auto filteredSpectrum = analyzer.calculateLinear(filtered);

		QVector<double> fr;

		//Calculating FR
		for (int i = 0; i < signalSpectrum.second.length(); ++i) {
			fr.push_back(20 * log10(filteredSpectrum.second.at(i) / signalSpectrum.second.at(i)));
		}

		if (ui->tabWidget->currentIndex() == 0) {
			filterFr->graph(0)->setData(signalSpectrum.first, fr, true);
			filterFr->yAxis->setRange(-60, 20);
			filterFr->xAxis->setRange(signalSpectrum.first.first(), 20000);
			filterFr->replot();
		} else if (ui->tabWidget->currentIndex() == 1) {
			filterFrBig->graph(0)->setData(signalSpectrum.first, fr, true);
			filterFrBig->yAxis->setRange(-60, 20);
			filterFrBig->xAxis->setRange(signalSpectrum.first.first(), 20000);
			filterFrBig->replot();
		}
	}
}

void MainWindow::replotSpectrum() {
	if (ui->tabWidget->currentIndex() == 0 || ui->tabWidget->currentIndex() == 2) {
		auto spectrumA = analyzer.calculate(filter.filter(bearingA.getVibration()));
		//auto spectrum_filtered = analyzer.calculate(filter.filter(bearingA.getVibration()));
		auto spectrumB = analyzer.calculate(filter.filter(bearingB.getVibration()));
		if (ui->tabWidget->currentIndex() == 0) {
			signalSpectrum->graph(0)->setData(spectrumA.first, spectrumA.second, true);
			signalSpectrum->graph(1)->setData(spectrumB.first, spectrumB.second, true);
			signalSpectrum->replot();
		} else if (ui->tabWidget->currentIndex() == 2) {
			signalSpectrumBig->graph(0)->setData(spectrumA.first, spectrumA.second, true);
			signalSpectrumBig->graph(1)->setData(spectrumB.first, spectrumB.second, true);
			signalSpectrumBig->replot();
		}
	}
}

//Connection between dial rpm control and rpm_window
void MainWindow::on_shaft_rpm_dial_valueChanged(int value) {
	ui->shaft_rpm->setValue(value);
}

//Connection between dial rpm control and rpm_window
void MainWindow::on_shaft_rpm_valueChanged(int value) {
	ui->shaft_rpm_dial->setValue(value);
	this->bearingA.setDesiredRPM(value);
	this->bearingB.setDesiredRPM(value);
	ui->shaft_rpm_real->display((int)(value / 60.0));
}

void MainWindow::on_shaft_rpm_editingFinished() {
	int value = 0;
	value = ui->shaft_rpm->value();
	ui->shaft_rpm_dial->setValue(value);
}

void MainWindow::on_defect1Slider_valueChanged(int value) {
	double severity = value / 100.0;
	ui->defect1Spin->setValue(severity);
	this->bearingA.setDefect(Bearing::DefectType::DEFECT_1, severity);
}

void MainWindow::on_defect2Slider_valueChanged(int value) {
	double severity = value / 100.0;
	ui->defect2Spin->setValue(severity);
	this->bearingA.setDefect(Bearing::DefectType::DEFECT_2, severity);
}

void MainWindow::on_defect3Slider_valueChanged(int value) {
	double severity = value / 100.0;
	ui->defect3Spin->setValue(severity);
	this->bearingA.setDefect(Bearing::DefectType::DEFECT_3, severity);
}

void MainWindow::on_defect4Slider_valueChanged(int value) {
	double severity = value / 100.0;
	ui->defect4Spin->setValue(severity);
	this->bearingA.setDefect(Bearing::DefectType::DEFECT_4, severity);
}

void MainWindow::on_defect5Slider_valueChanged(int value) {
	double severity = value / 100.0;
	ui->defect5Spin->setValue(severity);
	this->bearingA.setDefect(Bearing::DefectType::DEFECT_5, severity);
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
	bearingA.setDiscrFreq(arg1);
	bearingB.setDiscrFreq(arg1);
}

void MainWindow::on_desiredFPS_valueChanged(int arg1) {
	if (arg1 == 0) {
		replotTimer.stop();
	} else {
		replotTimer.setInterval(1000 / arg1);
		replotTimer.start();
	}
}

void MainWindow::on_left_bound_valueChanged(int leftBound) {

	int rightBound = ui->right_bound->value();

	if (rightBound < leftBound) {
		leftBound = rightBound;
		ui->left_bound->setValue(leftBound);
	}
	signalSpectrum->xAxis->setRange(leftBound, rightBound);
	signalSpectrumBig->xAxis->setRange(leftBound, rightBound);
}

void MainWindow::on_right_bound_valueChanged(int rightBound) {
	int leftBound = ui->left_bound->value();

	if (rightBound < leftBound) {
		rightBound = leftBound;
		ui->right_bound->setValue(rightBound);
	}

	signalSpectrum->xAxis->setRange(leftBound, rightBound);
	signalSpectrumBig->xAxis->setRange(leftBound, rightBound);
}

void MainWindow::on_high_cutoff_valueChanged(int arg1) {
}

void MainWindow::on_spinBox_valueChanged(int arg1) {
	filter.setLevel(arg1);
}

void MainWindow::on_comboBox_3_currentIndexChanged(const QString &arg1) {

	auto lowCutoff = manageCutoff().first;
	auto highCutoff = manageCutoff().second;

	if (arg1 == "Lowpass") {
		filter.setHighCutoffFrequency(highCutoff);
		filter.setType(LPF);
		ui->low_cutoff->setEnabled(false);
		ui->high_cutoff->setEnabled(true);
	} else if (arg1 == "Highpass") {
		filter.setLowCutoffFrequency(lowCutoff);
		filter.setType(HPF);
		ui->low_cutoff->setEnabled(true);
		ui->high_cutoff->setEnabled(false);
	} else if (arg1 == "Passband") {
		filter.setLowCutoffFrequency(lowCutoff);
		filter.setHighCutoffFrequency(highCutoff);
		filter.setType(BPF);
		ui->low_cutoff->setEnabled(true);
		ui->high_cutoff->setEnabled(true);
	}
}

void MainWindow::on_low_cutoff_valueChanged(int arg1) {
}

QPair<int, int> MainWindow::manageCutoff() {
	auto lowCutoff = ui->low_cutoff->value();
	auto highCutoff = ui->high_cutoff->value();
	auto type = ui->comboBox_3->currentText();

	if (type == "Passband") {
		if (lowCutoff >= highCutoff) {
			lowCutoff = highCutoff - 100;
			ui->low_cutoff->setValue(lowCutoff);
		}
	}

	return QPair<int, int>(lowCutoff, highCutoff);
}

void MainWindow::on_left_bound_2_valueChanged(int arg1) {
	ui->left_bound->setValue(arg1);
	on_left_bound_valueChanged(arg1);
}

void MainWindow::on_right_bound_2_valueChanged(int arg1) {
	ui->right_bound->setValue(arg1);
	on_right_bound_valueChanged(arg1);
}

void MainWindow::on_low_cutoff_editingFinished() {
	auto lowCutoff = manageCutoff().first;
	auto highCutoff = manageCutoff().second;

	filter.setLowCutoffFrequency(lowCutoff);
}

void MainWindow::on_high_cutoff_editingFinished() {
	auto lowCutoff = manageCutoff().first;
	auto highCutoff = manageCutoff().second;

	filter.setHighCutoffFrequency(highCutoff);
}

void MainWindow::on_about_triggered() {
	auto about = new AboutWindow();
	about->show();
}

void MainWindow::on_bearing_type_currentIndexChanged(const QString &type) {
	if (type == "403") {
		bearingA.changeType(Bearing::BearingType::TYPE_403);
		bearingB.changeType(Bearing::BearingType::TYPE_403);
	} else if (type == "405") {
		bearingA.changeType(Bearing::BearingType::TYPE_405);
		bearingB.changeType(Bearing::BearingType::TYPE_405);
	} else if (type == "406") {
		bearingA.changeType(Bearing::BearingType::TYPE_406);
		bearingB.changeType(Bearing::BearingType::TYPE_406);
	}

	ui->z->setValue(bearingA.getBalls());
	ui->bearing_diameter->setValue(bearingA.getDiameter());
	ui->balls_diameter->setValue(bearingA.getBallsDiameter());
}

void MainWindow::on_pushButton_4_clicked() {
	QString folderName = QFileDialog::getExistingDirectory(this, tr("Выберите папку для сохранения"), "", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
	ui->target_folder->setText(folderName);
}

void MainWindow::on_save_file_clicked() {
	//Target folder path
	auto folder = ui->target_folder->text();

	//Number of files to generate
	auto num = ui->number_of_files->value();

	auto referencePrefix = "/" + ui->reference_bearing_prefix->text();
	auto diagPrefix = "/" + ui->diag_bearing_prefix->text();

	for (int i = 0; i < num; ++i) {
		//Get vibration vector
		bearingA.nextStep();
		auto vibrationA = bearingA.getVibration();
		auto spectrumA = analyzer.calculate(vibrationA);

		bearingB.nextStep();
		auto vibrationB = bearingB.getVibration();
		auto spectrumB = analyzer.calculate(vibrationB);

		//Create file diag
		QFile fileOut(folder + diagPrefix + QString::number(i) + ".txt");
		if (fileOut.open(QIODevice::WriteOnly | QIODevice::Text)) {
			QTextStream out(&fileOut);
			out << bearingA.getRPM() / 60.0 << endl;
			out << bearingA.getBalls() * 1.0 << endl;
			out << bearingA.getDiameter() * 1.0 << endl;
			out << bearingA.getBallsDiameter() * 1.0 << endl;
			out << (double)bearingA.getDiscrFreq() / (double)vibrationA.length() << endl;

			for (double &sample : spectrumA.second) {
				out << sample << endl;
			}
		}
		fileOut.close();

		//Create file diag
		fileOut.setFileName(folder + referencePrefix + QString::number(i) + ".txt");
		if (fileOut.open(QIODevice::WriteOnly | QIODevice::Text)) {
			QTextStream out(&fileOut);
			out << bearingB.getRPM() / 60.0 << endl;
			out << bearingB.getBalls() * 1.0 << endl;
			out << bearingB.getDiameter() * 1.0 << endl;
			out << bearingB.getBallsDiameter() * 1.0 << endl;
			out << (double)bearingB.getDiscrFreq() / (double)vibrationB.length() << endl;
			for (double &sample : spectrumB.second) {
				out << sample << endl;
			}
		}
		fileOut.close();
	}
}

void MainWindow::on_pushButton_2_clicked() {
	ui->tabWidget->setCurrentIndex(3);
}
