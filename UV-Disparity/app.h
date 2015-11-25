#ifndef App_H
#define App_H

#include <QLabel>
#include <QFileDialog>
#include <QDir>
#include <QHBoxLayout>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/qPushButton>
#include <QtWidgets/qlayout>
#include <qcolor.h>

#include <chrono>
#include <ctime>

#include "ui_app.h"
#include "uv_disparity.h"
#include "hough_lines.h"

class App : public QMainWindow
{
	Q_OBJECT

public:
	App(QWidget *parent = 0);
	~App();

private slots:
	void LoadDisparity();
	void LoadSource();
	void LoadMask();
	void RemoveMask();

	void VDisparityCall();
	void UDisparityCall();
	void HoughLinesDetectionCall();
	void ProbabilisticHoughLinesDetectionCall();
	void CudaHoughLinesDetectionCall();
	void CudaProbabilisticHoughLinesDetectionCall();

private:
	QImage *source = NULL;
	QImage *disparity = NULL;
	QImage *mask = NULL;

	QImage *v_disparity = NULL;
	QImage *v_disparityNormalized = NULL;

	QImage *u_disparity = NULL;

	QImage *pointsOnLine = NULL;
	QImage *groundPlane = NULL;

	QImage *result = NULL;

	Ui::AppClass ui;

	QWidget *centralWidget;
	QLayout *centralLayout;

	QLabel *label_0x0;
	QLabel *label_0x1;
	QLabel *label_0x2;
	QLabel *label_1x0;
	QLabel *label_2x0;
	QLabel *label_2x1;
	QLabel *label_2x2;
	QLabel *label_3x0;
	QLabel *label_3x1;

	QWidget *contentWidget;
	QGridLayout *contentLayout;

	QLabel *elapsedTimeLabel;
};

#endif // App_H
