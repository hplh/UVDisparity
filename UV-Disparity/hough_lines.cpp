#include "hough_lines.h"
#include "ground_plane.h"

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

	if (!lines.empty()) {
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
	int index = -1;
	if (!lines.empty()) 
	{
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
	}

	end = std::chrono::high_resolution_clock::now();

	// paint detected line
	if (index != -1)
	{
		selectedLine = lines[index];
		line(res, cv::Point(selectedLine[0], selectedLine[1]), cv::Point(selectedLine[2], selectedLine[3]), cv::Scalar(0, 0, 255), 1, cv::LINE_AA);
	}
	
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

	if (!lines_gpu.empty()) {
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
	}

	// Mat dst to Qsource result
	result = QImage((uchar *)res.data, res.cols, res.rows, res.step, QImage::Format_RGB888).rgbSwapped();

	elapsed_seconds = end - start;
}


unsigned int lineCost(const QImage &source, cv::Vec4i &line, unsigned short int thresholdHoughThickness)
{
	std::vector<cv::Vec2i> points;
	PointsOnLineLine(source, line, points, thresholdHoughThickness);

	unsigned int cost = 0;

	for (auto currentPoint = points.begin(); currentPoint != points.end(); ++currentPoint)
	{
		cost += source.pixel((*currentPoint)[0], (*currentPoint)[1]);
	}

	return cost;
}

void getBestLine(const QImage &source, std::vector<cv::Vec4i> lines, cv::Vec4i &bestLine,unsigned short int thresholdHoughThickness)
{
	//cv::Vec4i bestLine;
	unsigned int bestCost = 0;

	for (auto currentLine = lines.begin(); currentLine != lines.end(); ++currentLine)
	{
		unsigned int currentLineCost = lineCost(source, *currentLine, thresholdHoughThickness);
		if (currentLineCost > bestCost)
		{
			bestCost = currentLineCost;
			bestLine = *currentLine;
		}
	}
}

void CudaProbabilisticHoughLinesDetection(const QImage &source, QImage &result, cv::Vec4i &selectedLine, std::chrono::duration<double> &elapsed_seconds, unsigned short int thresholdHoughThickness)
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

	cv::Ptr<cv::cuda::HoughSegmentDetector> hough = cv::cuda::createHoughSegmentDetector(1.0f, (float)(CV_PI / 180.0f), 100, 0);
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
	int index = -1;
	if (!lines_gpu.empty())
	{
		double max_len = 0;
		double len;
		//for (size_t i = 0; i < lines_gpu.size(); i++)
		//{
		//	cv::Vec4i l = lines_gpu[i];
		//	len = sqrt((l[2] - l[0]) * (l[2] - l[0]) + (l[3] - l[1]) * (l[3] - l[1]));
		//	if (len > max_len) {
		//		max_len = len;
		//		index = i;
		//	}
		//}
		getBestLine(source, lines_gpu, selectedLine, thresholdHoughThickness);

		// paint detected line
		line(res, cv::Point(selectedLine[0], selectedLine[1]), cv::Point(selectedLine[2], selectedLine[3]), cv::Scalar(0, 0, 255), 1, cv::LINE_AA);
	}

	end = std::chrono::high_resolution_clock::now();

	
	// paint detected lines
	//for (size_t i = 0; i < lines_gpu.size(); i++)
	//{
	//	cv::Vec4i l = lines_gpu[i];
	//	line(res, cv::Point(l[0], l[1]), cv::Point(l[2], l[3]), cv::Scalar(0, 0, 255), 1, cv::LINE_AA);
	//}

	// Mat dst to Qsource result
	result = QImage((uchar *)res.data, res.cols, res.rows, res.step, QImage::Format_RGB888).rgbSwapped();

	elapsed_seconds = end - start;
}