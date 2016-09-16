#include "mainwindow.h"
#include "qdebug.h"
#include "qstring.h"
#ifdef Q_OS_WIN
#include "ui_mainwindow_windows.h"
#else
#include "ui_mainwindow.h"
#endif


MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);

}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::on_exit_triggered()
{
	exit(EXIT_SUCCESS);
}
