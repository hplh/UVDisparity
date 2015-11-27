#include "ground_plane.h"

void PointsOnLineLine(const QImage &source, const cv::Vec4i line, std::vector<cv::Vec2i> &points, const double threshold)
{

	int width = source.width();
	int height = source.height();
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
			double x0 = col;//qGray(source.pixel(col, row));
			double y0 = row;

			if (row > rowStart && col > colStart)
			{
				distance = fabs((double)y0 - a * x0 - b) / sqrt(1.0 + a * a);
				if (distance <= threshold) {
					points.push_back(cv::Vec2i(col, row));
				}
			}
		}
	}
}


void PointsOnLine(const QImage &source, const cv::Vec4i line, std::vector<cv::Vec2i> &points, const double threshold)
{

	int width = source.width();
	int height = source.height();
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
			double x0 = qGray(source.pixel(col, row));
			double y0 = row;

			if (row > rowStart && col > colStart)
			{
				distance = fabs((double)y0 - a * x0 - b) / sqrt(1.0 + a * a);
				if (distance < threshold) {
					points.push_back(cv::Vec2i(col, row));
				}
			}
		}
	}
}

void GroundPlane(const QImage &source, std::vector<cv::Vec2i> &points, std::vector<cv::Vec2i> &groundPlanePoints, const double threshold)
{
	int width = source.width();
	int height = source.height();

	// TUI camera
	//double f = 141.29; // focal length
	//double B = 0.1197; // base line
	//double c_u = 163.4;
	//double c_v = 119.6;

	double f = 486.24;
	double B = 0.119947;
	double c_u = 320;
	double c_v = 240;

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
		int gray = qGray(source.pixel(u, v));
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

	// log detected plane ecuation
	std::fstream fs;
	fs.open("groundPlane.txt", std::fstream::app);
	fs << "a= " << a << " b= " << b << " c= " << c << std::endl;
	fs.flush();
	fs.close();

	// find all 3D points that lie on the plane within a threshold distance
	for (int row = 0; row < height; ++row)
	{
		for (int col = 0; col < width; ++col)
		{
			double x, y, z, dist;
			int gray = qGray(source.pixel(col, row));

			if (gray != 0)
			{
				z = f * B / gray;
				x = ((double)col - c_u) * z / f;
				y = ((double)row - c_v) * z / f;

				dist = fabs(a * x + b * y - z + c) / sqrt(a * a + b * b + c * c);

				if (dist <= threshold)
				{
					groundPlanePoints.push_back(cv::Vec2i(col, row));
				}
			}
		}
	}
}