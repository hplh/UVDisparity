#include "GroundPlane.h"

GroundPlane::GroundPlane()
{
}

QImage& GroundPlane::getImagePixelsFromHoughLine()
{
	return imagePixelsFromHoughLine;
}

QImage& GroundPlane::getDetectedPlane()
{
	return detectedPlane;
}

QImage& GroundPlane::getDrownOnSource()
{
	return drownOnSource;
}

void GroundPlane::paintImagePixelsFromHoughLine(const QImage &image, std::vector<cv::Vec2i> &points)
{
	int width = image.width();
	int height = image.height();

	imagePixelsFromHoughLine = QImage(width, height, QImage::Format_RGB888);
	imagePixelsFromHoughLine.fill(Qt::black);

	for (std::vector<cv::Vec2i>::iterator it = points.begin(); it != points.end(); ++it)
	{
		int col = (*it)[0];
		int row = (*it)[1];
		imagePixelsFromHoughLine.setPixel(col, row, image.pixel(col, row));
	}
}

void GroundPlane::paintDetectedPlane(const QImage &image, std::vector<cv::Vec2i> &points)
{
	int width = image.width();
	int height = image.height();
	
	detectedPlane = QImage(width, height, QImage::Format_RGB888);
	detectedPlane.fill(Qt::black);

	for (std::vector<cv::Vec2i>::iterator it = points.begin(); it != points.end(); ++it)
	{
		int col = (*it)[0];
		int row = (*it)[1];
		detectedPlane.setPixel(col, row, image.pixel(col, row));
	}
}

void GroundPlane::paintDrownOnSource(const QImage &image, std::vector<cv::Vec2i> &points)
{
	if (!image.isNull())
	{
		drownOnSource = QImage(image.convertToFormat(QImage::Format_ARGB32_Premultiplied));
		QPainter painter(&drownOnSource);
		painter.setPen(QColor(255, 0, 0, 128));

		for (std::vector<cv::Vec2i>::iterator it = points.begin(); it != points.end(); ++it)
		{
			int col = (*it)[0];
			int row = (*it)[1];
			painter.drawPoint(col, row);
		}

		painter.end();
	}
}

void GroundPlane::Detect(Image &image, HoughLinesMethods houghLinesMethod, const double thresholdHoughLineThickness, const double thresholdGroundPlaneThickness)
{
	QImage disparity = image.getDisparity();
	QImage source = image.getSource();

	int width = disparity.width();
	int height = disparity.height();
	
	cv::Vec4i line;
	
	switch (houghLinesMethod) {
	case houghLines:
		image.HoughLinesDetection(line, thresholdHoughLineThickness);
		break;
	case probabilisticHoughLines :
		image.ProbabilisticHoughLinesDetection(line, thresholdHoughLineThickness);
		break;
	case cudaHoughLines:
		image.CudaHoughLinesDetection(line, thresholdHoughLineThickness);
		break;
	case cudaProbabilisticHoughLines:
		image.CudaProbabilisticHoughLinesDetection(line, thresholdHoughLineThickness);
		break;
	}

	// find pixels in disparity image coresponding to detected line in vDisparity
	std::vector<cv::Vec2i> pixels = Utils::HoughLineToImagePixels(disparity, line, thresholdHoughLineThickness);

	if (pixels.size() == 0)
	{
		return;
	}

	paintImagePixelsFromHoughLine(disparity, pixels);

	// fit plane in the 3D point cloud formed by the found ground plane pixels
	// build 3D point cloud
	std::vector <cv::Vec3f> pointCloud;
	for (std::vector<cv::Vec2i>::iterator it = pixels.begin(); it != pixels.end(); ++it)
	{
		int u = (*it)[0];
		int v = (*it)[1];
		double x, y, z;
		int gray = qGray(disparity.pixel(u, v));
		if (gray != 0)
		{
			z = f * B / gray;
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
	std::vector<cv::Vec2i> groundPlanePoints;
	for (int row = 0; row < height; ++row)
	{
		for (int col = 0; col < width; ++col)
		{
			double x, y, z, dist;
			int gray = qGray(disparity.pixel(col, row));

			if (gray != 0)
			{
				z = f * B / gray;
				x = ((double)col - c_u) * z / f;
				y = ((double)row - c_v) * z / f;

				dist = fabs(a * x + b * y - z + c) / sqrt(a * a + b * b + c * c);

				if (dist <= thresholdGroundPlaneThickness)
				{
					groundPlanePoints.push_back(cv::Vec2i(col, row));
				}
			}
		}
	}

	paintDetectedPlane(disparity, groundPlanePoints);
	paintDrownOnSource(source, groundPlanePoints);
}

GroundPlane::~GroundPlane()
{
}
