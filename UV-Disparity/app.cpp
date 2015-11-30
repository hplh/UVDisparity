#include "app.h"

App::App(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	CreateMenus();
	InitializeComponents();
}

// slots
void App::SetHoughLineThicknessThreshSliderValue(int)
{
	if (!groundPlane.getDetectedPlane().isNull())
	{
		groundPlane.Detect(image, lastUsedHoughMethod, houghLineThicknessThreshSlider->value());
	}

	UpdateLabels();
}

void App::SetUVDisparityThreshSliderValue(int value)
{
	image.setUVDisparityThreshold(value);
	image.VDisparity();
	image.UDisparity();

	if (!groundPlane.getDetectedPlane().isNull())
	{
		groundPlane.Detect(image, lastUsedHoughMethod, houghLineThicknessThreshSlider->value());
	}

	UpdateLabels();
}

void App::Load()
{
	QString sourceFile;
	QString disparityFile;
	QString maskFile;
	
	QFileDialog fileDialog(this, tr("Load source"), QDir::currentPath());
	while (fileDialog.exec() == QDialog::Accepted)
	{
		sourceFile = fileDialog.selectedFiles().first();
		break;
	}

	if (!sourceFile.isNull() && sourceFile.endsWith(".pgm"))
	{
		QString base = sourceFile.remove(".pgm");
		disparityFile = base.append("_disp.pgm");
		maskFile = base.append("_mask.pgm");

		image.Load(sourceFile, disparityFile, maskFile);

		image.VDisparity();
		image.UDisparity();

		UpdateLabels();
	}
}

void App::Save()
{
	QImage groundPlaneOnSource = groundPlane.getDrownOnSource();

	QImage result(groundPlaneOnSource.width() + 256, groundPlaneOnSource.height() + 256, QImage::Format_RGB888);
	QPainter painter(&result);
	painter.drawImage(0, 0, groundPlaneOnSource);
	painter.drawImage(0, groundPlaneOnSource.height(), image.getUDisparityLog());
	painter.drawImage(groundPlaneOnSource.width(), 0, image.getVDisparityLog());
	painter.end();

	QString filename = QFileDialog::getSaveFileName(this, tr("Save"), QDir::currentPath(), tr("Documents (*.doc)"));

	result.save(filename);
}

void App::ProcessDirectories()
{
	//QString dirImageRaw = "E://Stereo Datasets//Poland//2015_0707_raw_outdoor//2015_0707_raw_pgm//";
	//QString dirImageDisp = "E://Stereo Datasets//Poland//2015_0707_raw_outdoor//ELAS disp//";
	//QString resultImageDisp = "E://UVDisparity//Results//";
	
	QString dirImageRaw = "D://sov//images//raw//";
	QString dirImageDisp = "D://sov//images//disp//";
	QString resultImageDisp = "D://sov//images//results//";

	uint16_t num_of_frames = 2000;

	std::chrono::duration<double> elapsed_seconds;

	for (int32_t i = 0; i < num_of_frames; i++) {

		// input file names
		char sourceBasename[256];
		sprintf(sourceBasename, "%d.pgm", i);

		char dispBasename[256];
		sprintf(dispBasename, "%d_disp.pgm", i);

		std::string sourceFilename = dirImageRaw.toStdString() + "/left_" + sourceBasename;
		std::string dispFilename = dirImageDisp.toStdString() + "/left_" + dispBasename;

		if (!QFile(QString(dispFilename.c_str())).exists() || !QFile(QString(sourceFilename.c_str())).exists())
		{
			continue;
		}

		image.Load(QString(sourceFilename.c_str()), QString(dispFilename.c_str()), "");
		image.VDisparity();
		image.UDisparity();
		groundPlane.Detect(image, cudaProbabilisticHoughLines, houghLineThicknessThreshSlider->value());

		QImage groundPlaneOnSource = groundPlane.getDrownOnSource();

		QImage result(groundPlaneOnSource.width() + 256, groundPlaneOnSource.height() + 256, QImage::Format_RGB888);
		QPainter painter(&result);
		painter.drawImage(0, 0, groundPlaneOnSource);
		painter.drawImage(0, groundPlaneOnSource.height(), image.getUDisparityLog());
		painter.drawImage(groundPlaneOnSource.width(), 0, image.getVDisparityLog());
		painter.end();

		char resultBasename[256];
		sprintf(resultBasename, "%d.png", i);

		std::string resultFilename = resultImageDisp.toStdString() + "/result_" + resultBasename;
		result.save(resultFilename.c_str());
	}
}

void App::HoughLinesCall()
{
	groundPlane.Detect(image, houghLines, houghLineThicknessThreshSlider->value());
	lastUsedHoughMethod = houghLines;
	UpdateLabels();
}

void App::ProbabilisticHoughLinesCall()
{
	groundPlane.Detect(image, probabilisticHoughLines, houghLineThicknessThreshSlider->value());
	lastUsedHoughMethod = houghLines;
	UpdateLabels();
}

void App::CudaHoughLinesCall()
{
	groundPlane.Detect(image, cudaHoughLines, houghLineThicknessThreshSlider->value());
	lastUsedHoughMethod = cudaHoughLines;
	UpdateLabels();
}

void App::CudaProbabilisticHoughLinesCall()
{
	groundPlane.Detect(image, cudaProbabilisticHoughLines, houghLineThicknessThreshSlider->value());
	lastUsedHoughMethod = cudaProbabilisticHoughLines;
	UpdateLabels();
}

// private methods
void App::CreateMenus()
{
	menuBar = new QMenuBar(this);
	menuBar->setGeometry(QRect(0, 0, 600, 21));
	
	// file menu
	fileMenu = new QMenu("File", menuBar);
	menuBar->addMenu(fileMenu);

	load = new QAction(fileMenu);
	load->setText("Load");
	connect(load, SIGNAL(triggered()), this, SLOT(Load()));

	save = new QAction(fileMenu);
	save->setText("Save");
	connect(save, SIGNAL(triggered()), this, SLOT(Save()));

	processDirectories = new QAction(fileMenu);
	processDirectories->setText("Process Directories");
	connect(processDirectories, SIGNAL(triggered()), this, SLOT(ProcessDirectories()));
	
	fileMenu->addAction(load);	
	fileMenu->addAction(save);
	fileMenu->addAction(processDirectories);

	// run menu
	runMenu = new QMenu("Run", menuBar);
	menuBar->addMenu(runMenu);

	houghLine = new QAction(runMenu);
	houghLine->setText("HoughLines");
	connect(houghLine, SIGNAL(triggered()), this, SLOT(HoughLinesCall()));

	probabilisticHoughLine = new QAction(runMenu);
	probabilisticHoughLine->setText("PHoughLines");
	connect(probabilisticHoughLine, SIGNAL(triggered()), this, SLOT(ProbabilisticHoughLinesCall()));

	cudaHoughLine = new QAction(runMenu);
	cudaHoughLine->setText("CudaHoughLines");
	connect(cudaHoughLine, SIGNAL(triggered()), this, SLOT(CudaHoughLinesCall()));

	cudaProbabilisticHoughLine = new QAction(runMenu);
	cudaProbabilisticHoughLine->setText("CudaPHoughLines");
	connect(cudaProbabilisticHoughLine, SIGNAL(triggered()), this, SLOT(CudaProbabilisticHoughLinesCall()));

	runMenu->addAction(houghLine);
	runMenu->addAction(probabilisticHoughLine);
	runMenu->addAction(cudaHoughLine);
	runMenu->addAction(cudaProbabilisticHoughLine);
}

void App::InitializeComponents()
{
	contentLayout = new QGridLayout(this);

	// Labels
	for (int i = 0; i < labelMatRows; ++i)
	{
		for (int j = 0; j < labelMatCols; ++j)
		{
			labelMat[i][j] = new QLabel;
			labelMat[i][j]->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
			labelMat[i][j]->setScaledContents(true);
			contentLayout->addWidget(labelMat[i][j], i, j);
		}
	}

	// hough line thickness threshold slider
	houghLineThicknessThreshSlider = new QSlider(Qt::Vertical);
	houghLineThicknessThreshSlider->setRange(0, 20);
	houghLineThicknessThreshSlider->setTickPosition(QSlider::TicksRight);
	connect(houghLineThicknessThreshSlider, SIGNAL(valueChanged(int)), this, SLOT(SetHoughLineThicknessThreshSliderValue(int)));
	contentLayout->addWidget(houghLineThicknessThreshSlider, 2, 3);

	// uv-disparity threshold slider
	uvDisparityThreshSlider = new QSlider(Qt::Vertical);
	uvDisparityThreshSlider->setRange(0, 20);
	uvDisparityThreshSlider->setTickPosition(QSlider::TicksRight);
	connect(uvDisparityThreshSlider, SIGNAL(valueChanged(int)), this, SLOT(SetUVDisparityThreshSliderValue(int)));
	contentLayout->addWidget(uvDisparityThreshSlider, 0, 3);
}

void App::UpdateLabels() {
	QImage disparity = image.getDisparity();
	if (!disparity.isNull())
	{
		labelMat[0][0]->setPixmap(QPixmap::fromImage(disparity));
		labelMat[0][0]->adjustSize();
	}

	QImage source = image.getSource();
	if (!source.isNull())
	{
		labelMat[3][1]->setPixmap(QPixmap::fromImage(source));
		labelMat[3][1]->adjustSize();
	}

	QImage mask = image.getMask();
	if (!mask.isNull())
	{
		labelMat[0][1]->setPixmap(QPixmap::fromImage(mask));
		labelMat[0][1]->adjustSize();
	}

	QImage vDisparity = image.getVDisparity();
	if (!vDisparity.isNull())
	{
		labelMat[0][2]->setPixmap(QPixmap::fromImage(vDisparity));
		labelMat[0][2]->adjustSize();
	}
	
	QImage vDisparityLog = image.getVDisparityLog();
	if (!vDisparityLog.isNull())
	{
		labelMat[2][2]->setPixmap(QPixmap::fromImage(vDisparityLog));
		labelMat[2][2]->adjustSize();
	}
	
	QImage uDisparity = image.getUDisparity();
	if (!uDisparity.isNull())
	{
		labelMat[1][1]->setPixmap(QPixmap::fromImage(uDisparity));
		labelMat[1][1]->adjustSize();
	}
	
	QImage uDisparityLog = image.getUDisparityLog();
	if (!uDisparityLog.isNull())
	{
		labelMat[1][0]->setPixmap(QPixmap::fromImage(uDisparityLog));
		labelMat[1][0]->adjustSize();
	}

	QImage imagePixelsFromHoughLine = groundPlane.getImagePixelsFromHoughLine();
	if (!imagePixelsFromHoughLine.isNull())
	{
		labelMat[2][1]->setPixmap(QPixmap::fromImage(imagePixelsFromHoughLine));
		labelMat[2][1]->adjustSize();
	}

	QImage detectedPlane = groundPlane.getDetectedPlane();
	if (!detectedPlane.isNull())
	{
		labelMat[2][0]->setPixmap(QPixmap::fromImage(detectedPlane));
		labelMat[2][0]->adjustSize();
	}

	QImage drownOnSource = groundPlane.getDrownOnSource();
	if (!drownOnSource.isNull())
	{
		labelMat[3][0]->setPixmap(QPixmap::fromImage(drownOnSource));
		labelMat[3][0]->adjustSize();
	}
}

App::~App()
{

}
