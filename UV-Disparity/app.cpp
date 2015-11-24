#include "app.h"

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

	sourceLabel = new QLabel;
	sourceLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	sourceLabel->setScaledContents(true);
	contentLayout->addWidget(sourceLabel, 0, 0);

	maskLabel = new QLabel;
	maskLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	maskLabel->setScaledContents(true);
	contentLayout->addWidget(maskLabel, 0, 1);

	v_disparityLabel = new QLabel;
	v_disparityLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	v_disparityLabel->setScaledContents(true);
	contentLayout->addWidget(v_disparityLabel, 0, 2);

	v_disparity_N_Label = new QLabel;
	v_disparity_N_Label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	v_disparity_N_Label->setScaledContents(true);
	contentLayout->addWidget(v_disparity_N_Label, 2, 2);

	u_disparityLabel = new QLabel;
	u_disparityLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	u_disparityLabel->setScaledContents(true);
	contentLayout->addWidget(u_disparityLabel, 2, 1);

	resultLabel = new QLabel;
	resultLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	resultLabel->setScaledContents(true);
	contentLayout->addWidget(resultLabel, 2, 0);

	contentWidget->setLayout(contentLayout);

	centralLayout->addWidget(elapsedTimeLabel);
	centralLayout->addWidget(contentWidget);
	centralWidget->setLayout(centralLayout);

	setCentralWidget(centralWidget);

	connect(ui.action_Load_default, SIGNAL(triggered()), this, SLOT(loadDefault()));
	connect(ui.action_Load_source, SIGNAL(triggered()), this, SLOT(loadSource()));
	connect(ui.action_Load_mask, SIGNAL(triggered()), this, SLOT(loadMask()));
	connect(ui.action_Remove_mask, SIGNAL(triggered()), this, SLOT(removeMask()));
	connect(ui.action_Save, SIGNAL(triggered()), this, SLOT(saveResult()));
	connect(ui.actionV_Disparity, SIGNAL(triggered()), this, SLOT(v_disparityCall()));
	connect(ui.actionU_Disparity, SIGNAL(triggered()), this, SLOT(u_disparityCall()));
	connect(ui.actionHoughLineV_Disp, SIGNAL(triggered()), this, SLOT(houghLinesV_DispCall()));
	connect(ui.actionPHoughLineV_Disp, SIGNAL(triggered()), this, SLOT(probabilisticHoughLinesV_DispCall()));
	connect(ui.actionCudaHoughLineV_Disp, SIGNAL(triggered()), this, SLOT(cudaHoughLinesV_DispCall()));
	connect(ui.actionCudaPHoughLineV_Disp, SIGNAL(triggered()), this, SLOT(cudaProbabilisticHoughLinesV_DispCall()));
}

void App::loadDefault()
{
	source = new QImage("D:\\3\\SoundOfVision\\UVDisparity\\docs\\img\\left_4061_disp");
	if (!source->isNull())
	{
		sourceLabel->setPixmap(QPixmap::fromImage(*source));
		sourceLabel->adjustSize();
	}

	mask = new QImage("D:\\3\\SoundOfVision\\UVDisparity\\docs\\img\\left_4061");
	if (!mask->isNull())
	{
		maskLabel->setPixmap(QPixmap::fromImage(*mask));
		maskLabel->adjustSize();
	}
}

void App::loadSource()
{
	QFileDialog fileDialog(this, tr("Load source"), "D:\\3\\SoundOfVision\\UVDisparity\\docs\\img");
	while (fileDialog.exec() == QDialog::Accepted)
	{
		source = new QImage(fileDialog.selectedFiles().first());
		if (!source->isNull())
		{
			sourceLabel->setPixmap(QPixmap::fromImage(*source));
			sourceLabel->adjustSize();
			break;
		}
	}
}

void App::loadMask()
{
	QFileDialog fileDialog(this, tr("Load mask"), "D:\\3\\SoundOfVision\\UVDisparity\\docs\\img");
	while (fileDialog.exec() == QDialog::Accepted)
	{
		mask = new QImage(fileDialog.selectedFiles().first());
		if (!source->isNull())
		{
			maskLabel->setPixmap(QPixmap::fromImage(*mask));
			maskLabel->adjustSize();
			break;
		}
	}
}

void App::removeMask()
{
	delete mask;
	mask = NULL;
	maskLabel->clear();
	maskLabel->adjustSize();
}

void App::saveResult()
{
	if (result != NULL)
	{
		QFileDialog fileDialog(this, tr("Load mask"), "D:\\3\\SoundOfVision");

		while (true)
		{
			QString fileName = fileDialog.getSaveFileName();
			if (!fileName.isNull() && (fileName.endsWith(".jpg") || fileName.endsWith(".png"))) 
			{
				result->save(fileName);
				break;
			}
		}
	}
}

void App::v_disparityCall()
{
	if (source != NULL)
	{
		v_disparityImage = new QImage(256, source->height(),QImage::Format_RGB888);
		v_disparityImage->fill(0);
		v_disparity_N_Image = new QImage(256, source->height(), QImage::Format_RGB888);
		v_disparity_N_Image->fill(0);

		std::chrono::duration<double> elapsed_seconds;

		if (mask == NULL) 
		{
			v_disparity(*source, *v_disparityImage, elapsed_seconds);

			char txt[32];
			sprintf(txt, "V-Disparity: %f", elapsed_seconds.count());

			elapsedTimeLabel->setText(QString(txt).append(" s"));

			v_disparity_N(*source, *v_disparity_N_Image, elapsed_seconds);
		}
		else
		{
			v_disparity(*source, *mask, *v_disparityImage, elapsed_seconds);

			char txt[32];
			sprintf(txt, "V-Disparity: %f", elapsed_seconds.count());

			elapsedTimeLabel->setText(QString(txt).append(" s"));

			v_disparity_N(*source, *mask, *v_disparity_N_Image, elapsed_seconds);
		}
		
		v_disparityLabel->setPixmap(QPixmap::fromImage(*v_disparityImage));
		v_disparityLabel->adjustSize();
		v_disparity_N_Label->setPixmap(QPixmap::fromImage(*v_disparity_N_Image));
		v_disparity_N_Label->adjustSize();
	}
}

void App::u_disparityCall()
{
	if (source != NULL)
	{
		u_disparityImage = new QImage(source->width(), 256, QImage::Format_RGB888);
		std::chrono::duration<double> elapsed_seconds;

		u_disparity(*source, *u_disparityImage, elapsed_seconds);

		char txt[32];
		sprintf(txt, "U-Disparity: %f", elapsed_seconds.count());

		elapsedTimeLabel->setText(QString(txt).append(" s"));
		u_disparityLabel->setPixmap(QPixmap::fromImage(*u_disparityImage));
		u_disparityLabel->adjustSize();
	}
}

void App::houghLinesV_DispCall()
{
	//if (v_disparityImage != NULL)
	//{
	//	QImage img(*v_disparity_N_Image);
	//	std::chrono::duration<double> elapsed_seconds;

	//	houghLines(*v_disparityImage, img, elapsed_seconds);

	//	char txt[128];
	//	sprintf(txt, "Hough Lines V-Disparity: %f", elapsed_seconds.count());
	//	elapsedTimeLabel->setText(QString(txt).append(" s"));

	//	v_disparity_N_Label->setPixmap(QPixmap::fromImage(img));
	//	v_disparity_N_Label->adjustSize();

	//	computeResult(img);
	//	resultLabel->setPixmap(QPixmap::fromImage(*result));
	//	resultLabel->adjustSize();
	//}
}

void App::probabilisticHoughLinesV_DispCall()
{
	if (v_disparityImage != NULL)
	{
		QImage img(*v_disparity_N_Image);
		std::chrono::duration<double> elapsed_seconds;
		cv::Vec4i line;

		probabilisticHoughLines(*v_disparityImage, img, line, elapsed_seconds);

		char txt[128];
		sprintf(txt, "Probabilistic Hough Lines V-Disparity: %f", elapsed_seconds.count());
		elapsedTimeLabel->setText(QString(txt).append(" s"));
		
		v_disparity_N_Label->setPixmap(QPixmap::fromImage(img));
		v_disparity_N_Label->adjustSize();

		computeResult(line);
		resultLabel->setPixmap(QPixmap::fromImage(*result));
		resultLabel->adjustSize();
	}
}

void App::cudaHoughLinesV_DispCall()
{
	//if (v_disparityImage != NULL)
	//{
	//	QImage img(*v_disparity_N_Image);
	//	std::chrono::duration<double> elapsed_seconds;

	//	cudaHoughLines(*v_disparityImage, img, elapsed_seconds);

	//	char txt[128];
	//	sprintf(txt, "Cuda Hough Lines V-Disparity: %f", elapsed_seconds.count());
	//	elapsedTimeLabel->setText(QString(txt).append(" s"));

	//	v_disparity_N_Label->setPixmap(QPixmap::fromImage(img));
	//	v_disparity_N_Label->adjustSize();

	//	computeResult(img);
	//	resultLabel->setPixmap(QPixmap::fromImage(*result));
	//	resultLabel->adjustSize();
	//}
}

void App::cudaProbabilisticHoughLinesV_DispCall()
{
	if (v_disparityImage != NULL)
	{
		QImage img(*v_disparity_N_Image);
		std::chrono::duration<double> elapsed_seconds;
		cv::Vec4i line;

		cudaProbabilisticHoughLines(*v_disparityImage, img, line, elapsed_seconds);

		char txt[128];
		sprintf(txt, "Cuda Probabilistic Hough Lines V-Disparity: %f", elapsed_seconds.count());
		elapsedTimeLabel->setText(QString(txt).append(" s"));

		v_disparity_N_Label->setPixmap(QPixmap::fromImage(img));
		v_disparity_N_Label->adjustSize();

		computeResult(line);
		resultLabel->setPixmap(QPixmap::fromImage(*result));
		resultLabel->adjustSize();
	}
}

void App::pointsOnLine(const cv::Vec4i line, std::vector<cv::Vec2i> &points, const double threshold)
{
	if (result != NULL)
	{
		delete result;
	}
	result = new QImage(source->width(), source->height(), QImage::Format_RGB888);

	int width = source->width();
	int height = source->height();
	QColor houghLineColor(255, 0, 0);
	QColor black(0, 0, 0);

	double a = (double)(line[3] - line[1]) / (line[2] - line[0]);
	double b = (double)line[1] - a * line[0];

	double distance;

	int rowStart = (line[1] < line[3]) ? line[1] : line[3];
	int colStart = (line[0] < line[2]) ? line[0] : line[2];

	for (int row = 0; row < height; ++row)
	{
		for (int col = 0; col < width; ++col)
		{
			double x0 = qGray(source->pixel(col, row));
			double y0 = row;

			distance = fabs((double)y0 - a * x0 - b) / sqrt(1.0 + a * a);
			if (row > rowStart && col > colStart && distance < threshold) {
				result->setPixel(col, row, source->pixel(col, row));
				points.push_back(cv::Vec2i(col, row));
			}

			else
			{
				result->setPixel(col, row, black.rgb());
			}
		}
	}

	u_disparityLabel->setPixmap(QPixmap::fromImage(*result));
	u_disparityLabel->adjustSize();
}


void App::computeResult(const cv::Vec4i line, const double threshold)
{
	if (result != NULL)
	{
		delete result;
	}

	int width = source->width();
	int height = source->height();

	result = new QImage(width, height, QImage::Format_RGB888);

	// TUI camera
		double f = 141.29; // focal length
		double B = 0.1197; // base line
		double c_u = 163.4;
		double c_v = 119.6;

	// find pixels in the image that generated the line in vDisp map
	std::vector <cv::Vec2i> points;
	pointsOnLine(line, points);

	if (points.size() == 0) 
	{
		return;
	}

	// fit plane in the 3D point cloud formed by the found ground plane pixels
	// build 3D point cloud
	std::vector <cv::Vec3f> pointCloud;
	for (std::vector<cv::Vec2i>::iterator it = points.begin(); it != points.end(); ++it)
	{
		int u = (*it)[0];
		int v = (*it)[1];
		double x, y, z;
		z = f * B / qGray(source->pixel(u, v));
		if (z != INFINITY)
		{
			x = ((double)u - c_u) * z / f;
			y = ((double)v - c_v) * z / f;
			pointCloud.push_back(cv::Vec3f(x, y, z));
		}
	}
	
	// find least squared plane
	CvMat *res = cvCreateMat(3, 1, CV_32FC1); // a, b, c for plane ax + by + c = 0;
	// matA * res = matB;
	CvMat *matA = cvCreateMat(pointCloud.size(), 3, CV_32FC1);
	CvMat *matB = cvCreateMat(pointCloud.size(), 1, CV_32FC1);
	int i = 0;
	for (std::vector<cv::Vec3f>::iterator it = pointCloud.begin(); it != pointCloud.end(); ++it)
	{
		cvmSet(matA, i, 0, (*it)[0]);
		cvmSet(matA, i, 1, (*it)[1]);
		cvmSet(matA, i, 2, 1);
		
		cvmSet(matB, i, 0, (*it)[2]);

		++i;
	}

	// solve the ecuation matA * res = matB;
	cvSolve(matA, matB, res, CV_SVD);

	// ax + by + c = z
	double a, b, c;
	a = cvmGet(res, 0, 0);
	b = cvmGet(res, 1, 0);
	c = cvmGet(res, 2, 0);

	// find all 3D points that lie on the plane within a threshold distance
	for (int row = 0; row < height; ++row)
	{
		for (int col = 0; col < width; ++col)
		{
			double x, y, z, dist;
			z = f * B / qGray(source->pixel(col, row));
			if (z != INFINITY)
			{
				x = ((double)col - c_u) * z / f;
				y = ((double)row - c_v) * z / f;
				
				dist = fabs(a * x + b * y - z + c) / sqrt(a * a + b * b + c * c);
				
				if (dist <= threshold) 
				{
					result->setPixel(col, row, source->pixel(col, row));
				} 
				else
				{
					result->setPixel(col, row, QColor(0,0,0).rgb());
				}

			}
		}
	}
}

App::~App()
{
	//
}
