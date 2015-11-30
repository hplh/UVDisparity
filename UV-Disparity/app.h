#ifndef APP_H
#define APP_H

#include <QtWidgets/QWidget>
#include <QMenuBar>
#include <QMenu>
#include <QLabel>
#include <QGridLayout>
#include <QFileDialog>
#include <QDirIterator>

#include "ui_app.h"

#include "GroundPlane.h"

class App : public QWidget
{
	Q_OBJECT

public:
	App(QWidget *parent = 0);
	~App();

private slots:
	void SetHoughLineThicknessThreshSliderValue(int);
	void SetUVDisparityThreshSliderValue(int);
	
	void Load();
	void Save();
	void ProcessDirectories();

	void HoughLinesCall();
	void ProbabilisticHoughLinesCall();
	void CudaHoughLinesCall();
	void CudaProbabilisticHoughLinesCall();

private:
	Ui::AppClass ui;

	QMenuBar *menuBar;
	QMenu *fileMenu;
	QAction *load;
	QAction *save;
	QAction *processDirectories;
	QMenu *runMenu;
	QAction *houghLine;
	QAction *probabilisticHoughLine;
	QAction *cudaHoughLine;
	QAction *cudaProbabilisticHoughLine;
	
	
	QGridLayout *contentLayout;
	static const unsigned int labelMatRows = 4;
	static const unsigned int labelMatCols = 3;
	QLabel *labelMat[labelMatRows][labelMatCols];
	
	QSlider *houghLineThicknessThreshSlider;
	QSlider *uvDisparityThreshSlider;

	Image image;
	GroundPlane groundPlane;

	HoughLinesMethods lastUsedHoughMethod = houghLines;

	void CreateMenus();
	void InitializeComponents();
	void UpdateLabels();
};

#endif // APP_H
