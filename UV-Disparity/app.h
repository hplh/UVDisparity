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
	void loadDefault();
	void loadSource();
	void loadMask();
	void removeMask();
	void saveResult();

	void v_disparityCall();
	void u_disparityCall();
	void houghLinesV_DispCall();
	void probabilisticHoughLinesV_DispCall();
	void cudaHoughLinesV_DispCall();
	void cudaProbabilisticHoughLinesV_DispCall();

private:
	QImage *source = NULL;
	QImage *mask = NULL;
	QImage *result = NULL;

	QImage *v_disparityImage = NULL;
	QImage *v_disparity_N_Image = NULL;

	QImage *u_disparityImage = NULL;

	Ui::AppClass ui;

	QWidget *centralWidget;
	QLayout *centralLayout;

	QLabel *sourceLabel;
	QLabel *maskLabel;
	QLabel *resultLabel;
	QLabel *v_disparityLabel;
	QLabel *v_disparity_N_Label;
	QLabel *u_disparityLabel;

	QWidget *contentWidget;
	QGridLayout *contentLayout;

	QLabel *elapsedTimeLabel;

	void pointsOnLine(const cv::Vec4i line, std::vector<cv::Vec2i> &points, const double threshold = 1);
	void computeResult(const cv::Vec4i line, const double threshold = 0.01);
};

#endif // App_H
