#include "app.h"

#include <iostream>
#include <QDirIterator>


App::App(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	centralWidget = new QWidget(this);
	centralLayout = new QVBoxLayout(this);

	contentWidget = new QWidget(this);
	contentLayout = new QGridLayout(this);

	elapsedTimeLabel = new QLabel("Elapsed time: 0 s");
	elapsedTimeLabel->setFixedSize(300, 15);

	label_0x0 = new QLabel;
	label_0x0->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	label_0x0->setScaledContents(true);
	contentLayout->addWidget(label_0x0, 0, 0);

	label_0x1 = new QLabel;
	label_0x1->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	label_0x1->setScaledContents(true);
	contentLayout->addWidget(label_0x1, 0, 1);

	label_0x2 = new QLabel;
	label_0x2->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	label_0x2->setScaledContents(true);
	contentLayout->addWidget(label_0x2, 0, 2);

	label_1x0 = new QLabel;
	label_1x0->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	label_1x0->setScaledContents(true);
	contentLayout->addWidget(label_1x0, 1, 0);

	label_2x0 = new QLabel;
	label_2x0->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	label_2x0->setScaledContents(true);
	contentLayout->addWidget(label_2x0, 2, 0);

	label_2x1 = new QLabel;
	label_2x1->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	label_2x1->setScaledContents(true);
	contentLayout->addWidget(label_2x1, 2, 1);

	label_2x2 = new QLabel;
	label_2x2->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	label_2x2->setScaledContents(true);
	contentLayout->addWidget(label_2x2, 2, 2);

	label_3x0 = new QLabel;
	label_3x0->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	label_3x0->setScaledContents(true);
	contentLayout->addWidget(label_3x0, 3, 0);

	label_3x1 = new QLabel;
	label_3x1->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	label_3x1->setScaledContents(true);
	contentLayout->addWidget(label_3x1, 3, 1);


	houghThreshSlider = new QSlider(Qt::Vertical);
	houghThreshSlider->setRange(0, 20);
	houghThreshSlider->setTickPosition(QSlider::TicksRight);
	houghThreshSlider->setValue(0);
	//houghThreshSlider->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);

	connect(houghThreshSlider, SIGNAL(valueChanged(int)), this, SLOT(setHoughThreshSliderValue(int)));
	contentLayout->addWidget(houghThreshSlider, 2, 3);

	contentWidget->setLayout(contentLayout);

	centralLayout->addWidget(elapsedTimeLabel);
	centralLayout->addWidget(contentWidget);
	centralWidget->setLayout(centralLayout);

	setCentralWidget(centralWidget);

	connect(ui.action_Load_disparity, SIGNAL(triggered()), this, SLOT(LoadDisparity()));
	connect(ui.action_Load_source, SIGNAL(triggered()), this, SLOT(LoadSource()));
	connect(ui.action_Load_mask, SIGNAL(triggered()), this, SLOT(LoadMask()));
	connect(ui.action_Remove_mask, SIGNAL(triggered()), this, SLOT(RemoveMask()));
	connect(ui.actionLoad_directory, SIGNAL(triggered()), this, SLOT(LoadDirectory()));

	connect(ui.actionV_Disparity, SIGNAL(triggered()), this, SLOT(VDisparityCall()));
	connect(ui.actionU_Disparity, SIGNAL(triggered()), this, SLOT(UDisparityCall()));
	connect(ui.actionHoughLineV_Disp, SIGNAL(triggered()), this, SLOT(HoughLinesDetectionCall()));
	connect(ui.actionPHoughLineV_Disp, SIGNAL(triggered()), this, SLOT(ProbabilisticHoughLinesDetectionCall()));
	connect(ui.actionCudaHoughLineV_Disp, SIGNAL(triggered()), this, SLOT(CudaHoughLinesDetectionCall()));
	connect(ui.actionCudaPHoughLineV_Disp, SIGNAL(triggered()), this, SLOT(CudaProbabilisticHoughLinesDetectionCall()));
}

void App::LoadDisparity()
{
	QFileDialog fileDialog(this, tr("Load disparity"), QDir::currentPath());
	while (fileDialog.exec() == QDialog::Accepted)
	{
		disparity = new QImage(fileDialog.selectedFiles().first());
		if (!disparity->isNull())
		{
			label_0x0->setPixmap(QPixmap::fromImage(*disparity));
			label_0x0->adjustSize();
			break;
		}
	}
}

void App::LoadSource()
{
	QFileDialog fileDialog(this, tr("Load source"), QDir::currentPath());
	while (fileDialog.exec() == QDialog::Accepted)
	{
		source = new QImage(fileDialog.selectedFiles().first());
		if (!source->isNull())
		{
			label_3x1->setPixmap(QPixmap::fromImage(*source));
			label_3x1->adjustSize();
			break;
		}
	}
}

void App::LoadMask()
{
	QFileDialog fileDialog(this, tr("Load mask"), QDir::currentPath());
	while (fileDialog.exec() == QDialog::Accepted)
	{
		mask = new QImage(fileDialog.selectedFiles().first());
		if (!mask->isNull())
		{
			label_0x1->setPixmap(QPixmap::fromImage(*mask));
			label_0x1->adjustSize();
			break;
		}
	}
}

void App::RemoveMask()
{
	delete mask;
	mask = NULL;
	label_0x1->clear();
	label_0x1->adjustSize();
}

void App::VDisparityCall()
{
	if (disparity != NULL)
	{
		v_disparity = new QImage(256, disparity->height(), QImage::Format_RGB888);
		v_disparity->fill(0);

		v_disparityNormalized = new QImage(256, disparity->height(), QImage::Format_RGB888);
		v_disparityNormalized->fill(0);

		std::chrono::duration<double> elapsed_seconds;

		if (mask == NULL)
		{
			VDisparity(*disparity, *v_disparity, elapsed_seconds);
			VDisparityNormalized(*disparity, *v_disparityNormalized);
		}
		else
		{
			VDisparity(*disparity, *mask, *v_disparity, elapsed_seconds);
			VDisparityNormalized(*disparity, *mask, *v_disparityNormalized);
		}

		PrintElapsedTime(elapsed_seconds, "V-Disparity");

		label_0x2->setPixmap(QPixmap::fromImage(*v_disparity));
		label_0x2->adjustSize();

		label_2x2->setPixmap(QPixmap::fromImage(*v_disparityNormalized));
		label_2x2->adjustSize();
	}
}

void App::UDisparityCall()
{
	if (disparity != NULL)
	{
		u_disparity = new QImage(disparity->width(), 256, QImage::Format_RGB888);
		u_disparity->fill(0);
		std::chrono::duration<double> elapsed_seconds;

		UDisparity(*disparity, *u_disparity, elapsed_seconds);

		PrintElapsedTime(elapsed_seconds, "U-Disparity");

		label_1x0->setPixmap(QPixmap::fromImage(*u_disparity));
		label_1x0->adjustSize();
	}
}

void App::HoughLinesDetectionCall()
{
	if (v_disparity != NULL)
	{
		QImage img(*v_disparityNormalized);
		std::chrono::duration<double> elapsed_seconds;

		HoughLinesDetection(*v_disparity, img, elapsed_seconds);

		PrintElapsedTime(elapsed_seconds, "Hough Lines");

		label_2x2->setPixmap(QPixmap::fromImage(img));
		label_2x2->adjustSize();
	}
}

void App::ProbabilisticHoughLinesDetectionCall()
{
	if (v_disparity != NULL)
	{
		QImage img(*v_disparityNormalized);
		std::chrono::duration<double> elapsed_seconds;
		cv::Vec4i line;

		ProbabilisticHoughLinesDetection(*v_disparity, img, line, elapsed_seconds);

		PrintElapsedTime(elapsed_seconds, "Probabilistic Hough Lines");

		label_2x2->setPixmap(QPixmap::fromImage(img));
		label_2x2->adjustSize();

		VDisparityLineToGroundPlane(line);
	}
}

void App::CudaHoughLinesDetectionCall()
{
	if (v_disparity != NULL)
	{
		QImage img(*v_disparityNormalized);
		std::chrono::duration<double> elapsed_seconds;

		CudaHoughLinesDetection(*v_disparity, img, elapsed_seconds);

		PrintElapsedTime(elapsed_seconds, "Cuda Hough Lines");

		label_2x2->setPixmap(QPixmap::fromImage(img));
		label_2x2->adjustSize();
	}
}

void App::CudaProbabilisticHoughLinesDetectionCall()
{
	if (v_disparity != NULL)
	{
		QImage img(*v_disparityNormalized);
		std::chrono::duration<double> elapsed_seconds;
		cv::Vec4i line;

		CudaProbabilisticHoughLinesDetection(*v_disparity, img, line, elapsed_seconds, houghThreshSlider->value());

		PrintElapsedTime(elapsed_seconds, "Cuda Probabilistic Hough Lines");

		label_2x2->setPixmap(QPixmap::fromImage(img));
		label_2x2->adjustSize();

		VDisparityLineToGroundPlane(line);
	}
}

void App::PrintElapsedTime(const std::chrono::duration<double> elapsed_seconds, const char* method)
{
	char txt[128];
	sprintf(txt, "%s: %f", method, elapsed_seconds.count());
	elapsedTimeLabel->setText(QString(txt).append(" s"));
}

void App::VDisparityLineToGroundPlane(const cv::Vec4i line)
{
	// find all points that lie on the detected line within a threshold distance
	std::vector<cv::Vec2i> points;
	PointsOnLine(*disparity, line, points);

	pointsOnLine = new QImage(disparity->width(), disparity->height(), QImage::Format_RGB888);
	pointsOnLine->fill(0);
	for (std::vector<cv::Vec2i>::iterator it = points.begin(); it != points.end(); ++it)
	{
		int col = (*it)[0];
		int row = (*it)[1];
		pointsOnLine->setPixel(col, row, disparity->pixel(col, row));
	}

	label_2x1->setPixmap(QPixmap::fromImage(*pointsOnLine));
	label_2x1->adjustSize();

	// find ground plane 
	std::vector<cv::Vec2i> groundPlanePoints;
	GroundPlane(*disparity, points, groundPlanePoints);

	groundPlane = new QImage(disparity->width(), disparity->height(), QImage::Format_RGB888);
	groundPlane->fill(0);

	for (std::vector<cv::Vec2i>::iterator it = groundPlanePoints.begin(); it != groundPlanePoints.end(); ++it)
	{
		int col = (*it)[0];
		int row = (*it)[1];
		groundPlane->setPixel(col, row, disparity->pixel(col, row));
	}

	label_2x0->setPixmap(QPixmap::fromImage(*groundPlane));
	label_2x0->adjustSize();

	// paint detected ground plane on result image
	if (source != NULL)
	{
		result = new QImage(source->convertToFormat(QImage::Format_ARGB32_Premultiplied));
		QPainter painter(result);
		painter.setPen(QColor(255, 0, 0, 128));

		for (std::vector<cv::Vec2i>::iterator it = groundPlanePoints.begin(); it != groundPlanePoints.end(); ++it)
		{
			int col = (*it)[0];
			int row = (*it)[1];
			painter.drawPoint(col, row);
		}

		painter.end();
		label_3x0->setPixmap(QPixmap::fromImage(*result));
		label_3x0->adjustSize();
	}
}

App::~App()
{
	//
}


void App::setHoughThreshSliderValue(int newValue)
{
	if (v_disparity != NULL)
	{
		QImage img(*v_disparityNormalized);
		std::chrono::duration<double> elapsed_seconds;
		cv::Vec4i line;

		CudaProbabilisticHoughLinesDetection(*v_disparity, img, line, elapsed_seconds, houghThreshSlider->value());

		PrintElapsedTime(elapsed_seconds, "Cuda Probabilistic Hough Lines");

		label_2x2->setPixmap(QPixmap::fromImage(img));
		label_2x2->adjustSize();

		VDisparityLineToGroundPlane(line);
	}
}



//load dir
void App::LoadDirectory()
{
	//QString dir = "TransferFunctions";
	QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"), "", QFileDialog::DontResolveSymlinks);

	if (!dir.isNull()) 
	{
		QDirIterator it(dir, QDirIterator::Subdirectories);
		while (it.hasNext()) {
			it.next();
			if (it.fileInfo().isFile())
			{

				std::cout << it.filePath().toStdString() << "\n";
				//fileList.push_back(it.filePath());
				QImage currImg;
				currImg.load(it.filePath());

				std::cout << currImg.size().width() << ", " << currImg.size().height() << '\n';
			}
		}
	}

}