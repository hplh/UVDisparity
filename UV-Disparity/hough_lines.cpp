#include "hough_lines.h"

void HoughLinesDetection(const QImage &source, QImage &result, std::chrono::duration<double> &elapsed_seconds)
{
	// Qsource source to Mat src considering Qsource::Format_RGB888
	QImage swappedSource = source.rgbSwapped();
	cv::Mat src = cv::Mat(swappedSource.height(), swappedSource.width(), CV_8UC3, const_cast<uchar*>(swappedSource.bits()), swappedSource.bytesPerLine()); // CV_8UC3 = 8-bit, 3 channel

	QImage swappedResult = result.rgbSwapped();
	cv::Mat res = cv::Mat(swappedResult.height(), swappedResult.width(), CV_8UC3, const_cast<uchar*>(swappedResult.bits()), swappedResult.bytesPerLine());

	std::vector<cv::Vec2f> lines;

	cv::Mat houghLinesSrc;
	cv::cvtColor(src, houghLinesSrc, CV_BGR2GRAY);

	std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
	start = std::chrono::high_resolution_clock::now();

	HoughLines(houghLinesSrc, lines, 1, CV_PI / 180, 150);

	end = std::chrono::high_resolution_clock::now();

	for (size_t i = 0; i < lines.size(); i++)
	{
		float rho = lines[i][0], theta = lines[i][1];
		cv::Point pt1, pt2;
		double a = cos(theta), b = sin(theta);
		double x0 = a*rho, y0 = b*rho;
		pt1.x = cvRound(x0 + 1000 * (-b));
		pt1.y = cvRound(y0 + 1000 * (a));
		pt2.x = cvRound(x0 - 1000 * (-b));
		pt2.y = cvRound(y0 - 1000 * (a));
		
		line(res, pt1, pt2, cv::Scalar(0, 0, 255), 1, CV_AA);
	}

	// Mat dst to Qsource result
	result = QImage((uchar *)res.data, res.cols, res.rows, res.step, QImage::Format_RGB888).rgbSwapped();

	elapsed_seconds = end - start;
}

void ProbabilisticHoughLinesDetection(const QImage &source, QImage &result, cv::Vec4i &selectedLine, std::chrono::duration<double> &elapsed_seconds)
{
	// Qsource source to Mat src considering Qsource::Format_RGB888
	QImage swappedSource = source.rgbSwapped();
	cv::Mat src = cv::Mat(swappedSource.height(), swappedSource.width(), CV_8UC3, const_cast<uchar*>(swappedSource.bits()), swappedSource.bytesPerLine()); // CV_8UC3 = 8-bit, 3 channel

	QImage swappedResult = result.rgbSwapped();
	cv::Mat res = cv::Mat(swappedResult.height(), swappedResult.width(), CV_8UC3, const_cast<uchar*>(swappedResult.bits()), swappedResult.bytesPerLine());

	std::vector<cv::Vec4i> lines;

	cv::Mat houghLinesSrc;
	cv::cvtColor(src, houghLinesSrc, CV_BGR2GRAY);

	std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
	start = std::chrono::high_resolution_clock::now();

	HoughLinesP(houghLinesSrc, lines, 1, CV_PI / 360, 150, 50, 0);

	// find the longest line
	int index = 0;
	double max_len = 0;
	double len;
	for (size_t i = 0; i < lines.size(); i++)
	{
		cv::Vec4i l = lines[i];
		len = sqrt((l[2] - l[0]) * (l[2] - l[0]) + (l[3] - l[1]) * (l[3] - l[1]));
		if (len > max_len) {
			max_len = len;
			index = i;
		}

	}

	end = std::chrono::high_resolution_clock::now();

	// paint detected line
	selectedLine = lines[index];
	line(res, cv::Point(selectedLine[0], selectedLine[1]), cv::Point(selectedLine[2], selectedLine[3]), cv::Scalar(0, 0, 255), 1, cv::LINE_AA);

	//// paint detected lines
	//for (size_t i = 0; i < lines.size(); i++)
	//{
	//	cv::Vec4i l = lines[i];
	//	line(res, cv::Point(l[0], l[1]), cv::Point(l[2], l[3]), cv::Scalar(0, 0, 255), 1, cv::LINE_AA);
	//}

	// Mat dst to Qsource result
	result = QImage((uchar *)res.data, res.cols, res.rows, res.step, QImage::Format_RGB888).rgbSwapped();

	elapsed_seconds = end - start;
}

void CudaHoughLinesDetection(const QImage &source, QImage &result, std::chrono::duration<double> &elapsed_seconds)
{
	QImage swappedSource = source.rgbSwapped();
	cv::Mat src = cv::Mat(swappedSource.height(), swappedSource.width(), CV_8UC3, const_cast<uchar*>(swappedSource.bits()), swappedSource.bytesPerLine()); // CV_8UC3 = 8-bit, 3 channel

	QImage swappedResult = result.rgbSwapped();
	cv::Mat res = cv::Mat(swappedResult.height(), swappedResult.width(), CV_8UC3, const_cast<uchar*>(swappedResult.bits()), swappedResult.bytesPerLine());

	cv::Mat mask;
	cv::cvtColor(src, mask, CV_BGR2GRAY);

	cv::cuda::GpuMat d_src(mask);
	cv::cuda::GpuMat d_lines;

	std::chrono::time_point<std::chrono::system_clock> start, end;
	start = std::chrono::system_clock::now();

	cv::Ptr<cv::cuda::HoughLinesDetector> hough = cv::cuda::createHoughLinesDetector(1.0f, (float)(CV_PI / 180.0f), 150);
	hough->detect(d_src, d_lines);

	end = std::chrono::system_clock::now();

	std::vector<cv::Vec2f> lines_gpu;
	hough->downloadResults(d_lines, lines_gpu);

	for (size_t i = 0; i < lines_gpu.size(); ++i)
	{
		float rho = lines_gpu[i][0], theta = lines_gpu[i][1];
		cv::Point pt1, pt2;
		double a = cos(theta), b = sin(theta);
		double x0 = a*rho, y0 = b*rho;
		pt1.x = cvRound(x0 + 1000 * (-b));
		pt1.y = cvRound(y0 + 1000 * (a));
		pt2.x = cvRound(x0 - 1000 * (-b));
		pt2.y = cvRound(y0 - 1000 * (a));
		line(res, pt1, pt2, cv::Scalar(0, 0, 255), 1, CV_AA);
	}

	// Mat dst to Qsource result
	result = QImage((uchar *)res.data, res.cols, res.rows, res.step, QImage::Format_RGB888).rgbSwapped();

	elapsed_seconds = end - start;
}

void CudaProbabilisticHoughLinesDetection(const QImage &source, QImage &result, cv::Vec4i &selectedLine, std::chrono::duration<double> &elapsed_seconds)
{
	QImage swappedSource = source.rgbSwapped();
	cv::Mat src = cv::Mat(swappedSource.height(), swappedSource.width(), CV_8UC3, const_cast<uchar*>(swappedSource.bits()), swappedSource.bytesPerLine()); // CV_8UC3 = 8-bit, 3 channel

	QImage swappedResult = result.rgbSwapped();
	cv::Mat res = cv::Mat(swappedResult.height(), swappedResult.width(), CV_8UC3, const_cast<uchar*>(swappedResult.bits()), swappedResult.bytesPerLine());

	cv::Mat mask;
	cv::cvtColor(src, mask, CV_BGR2GRAY);

	cv::cuda::GpuMat d_src(mask);
	cv::cuda::GpuMat d_lines;

	std::chrono::time_point<std::chrono::system_clock> start, end;
	start = std::chrono::system_clock::now();

	cv::Ptr<cv::cuda::HoughSegmentDetector> hough = cv::cuda::createHoughSegmentDetector(1.0f, (float)(CV_PI / 180.0f), 150, 0);
	hough->detect(d_src, d_lines);

	end = std::chrono::system_clock::now();

	std::vector<cv::Vec4i> lines_gpu;
	if (!d_lines.empty())
	{
		lines_gpu.resize(d_lines.cols);
		cv::Mat h_lines(1, d_lines.cols, CV_32SC4, &lines_gpu[0]);
		d_lines.download(h_lines);
	}

	// find the longest line
	int index = 0;
	double max_len = 0;
	double len;
	for (size_t i = 0; i < lines_gpu.size(); i++)
	{
		cv::Vec4i l = lines_gpu[i];
		len = sqrt((l[2] - l[0]) * (l[2] - l[0]) + (l[3] - l[1]) * (l[3] - l[1]));
		if (len > max_len) {
			max_len = len;
			index = i;
		}

	}

	end = std::chrono::high_resolution_clock::now();

	// paint detected line
	selectedLine = lines_gpu[index];
	line(res, cv::Point(selectedLine[0], selectedLine[1]), cv::Point(selectedLine[2], selectedLine[3]), cv::Scalar(0, 0, 255), 1, cv::LINE_AA);

	//// paint detected lines
	//for (size_t i = 0; i < lines_gpu.size(); i++)
	//{
	//	cv::Vec4i l = lines_gpu[i];
	//	line(res, cv::Point(l[0], l[1]), cv::Point(l[2], l[3]), cv::Scalar(0, 0, 255), 1, cv::LINE_AA);
	//}

	// Mat dst to Qsource result
	result = QImage((uchar *)res.data, res.cols, res.rows, res.step, QImage::Format_RGB888).rgbSwapped();

	elapsed_seconds = end - start;
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

			distance = fabs((double)y0 - a * x0 - b) / sqrt(1.0 + a * a);
			if (row > rowStart && col > colStart && distance < threshold) {
				points.push_back(cv::Vec2i(col, row));
			}
		}
	}
}

void GroundPlane(const QImage &source, std::vector<cv::Vec2i> &points, std::vector<cv::Vec2i> &groundPlanePoints, const double threshold)
{
	int width = source.width();
	int height = source.height();

	// TUI camera
	double f = 141.29; // focal length
	double B = 0.1197; // base line
	double c_u = 163.4;
	double c_v = 119.6;

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