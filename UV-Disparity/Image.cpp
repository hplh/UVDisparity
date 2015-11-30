#include "Image.h"


Image::Image()
{
}

void Image::Load(QString sourceFile, QString disparityFile, QString maskFile)
{
	source.load(sourceFile);
	disparity.load(disparityFile);
	mask.load(maskFile);

	if (!disparity.isNull())
	{
		startRow = disparity.height() / 2;
	}
}

QImage& Image::getSource()
{
	return source;
}

QImage& Image::getDisparity()
{
	return disparity;
}

QImage& Image::getMask()
{
	return mask;
}

QImage& Image::getVDisparity()
{
	return vDisparity;
}

QImage& Image::getVDisparityLog()
{
	return vDisparityLog;
}

QImage& Image::getUDisparity()
{
	return uDisparity;
}

QImage& Image::getUDisparityLog()
{
	return uDisparityLog;
}

void Image::setUVDisparityThreshold(int value)
{
	uvDisparityThreshold = value;
}

void Image::VDisparity()
{
	int width = disparity.width();
	int height = disparity.height();

	vDisparity = QImage(256, height, QImage::Format_RGB888);
	vDisparity.fill(Qt::black);

	vDisparityLog = QImage(256, height, QImage::Format_RGB888);
	vDisparityLog.fill(Qt::black);

	// find the maximum intensity in the disparity image
	int max = 0;
	for (int row = 0; row < height; ++row)
	{
		for (int col = 0; col < width; ++col)
		{
			int gray = qGray(disparity.pixel(col, row));
			if (gray > max)
			{
				max = gray;
			}
		}
	}

	// calculate VDisparity
	if (mask.isNull())
	{
		
		for (int row = startRow; row < height; ++row)
		{
			unsigned char rowHistogram[256] = { 0 };
			for (int col = 0; col < width; ++col)
			{
				int gray = qGray(disparity.pixel(col, row));

				if (gray != 255 && gray > 5)
				{
					++rowHistogram[gray];
				}
			}

			for (int col = 0; col < 256; ++col)
			{
				if (rowHistogram[col] < uvDisparityThreshold)
				{
					rowHistogram[col] = 0;
				}
			
				vDisparity.setPixel(col, row, QColor(rowHistogram[col], rowHistogram[col], rowHistogram[col]).rgb());

				double colorVal = log(rowHistogram[col] + 1) / log(max) * 255;
				vDisparityLog.setPixel(col, row, QColor(colorVal, colorVal, colorVal).rgb());
			}
		}
	}
	else
	{
		for (int row = 0; row < height; ++row)
		{
			unsigned char rowHistogram[256] = { 0 };
			for (int col = 0; col < width; ++col)
			{
				int grayM = qGray(mask.pixel(col, row));
				int grayD = qGray(disparity.pixel(col, row));
				if (grayM != 255 && grayD != 255 && grayD > 5)
				{
					++rowHistogram[grayD];
				}
			}

			for (int col = 0; col < 256; ++col)
			{
				if (rowHistogram[col] < uvDisparityThreshold)
				{
					rowHistogram[col] = 0;
				}
				
				vDisparity.setPixel(col, row, QColor(rowHistogram[col], rowHistogram[col], rowHistogram[col]).rgb());

				double colorVal = log(rowHistogram[col] + 1) / log(max) * 255;
				vDisparityLog.setPixel(col, row, QColor(colorVal, colorVal, colorVal).rgb());
			}
		}
	}
	vDisparityLogCopy = QImage(vDisparityLog);
}

void Image::UDisparity()
{
	int width = disparity.width();
	int height = disparity.height();

	uDisparity = QImage(width, 256, QImage::Format_RGB888);
	uDisparity.fill(Qt::black);

	uDisparityLog = QImage(width, 256, QImage::Format_RGB888);
	uDisparityLog.fill(Qt::black);

	// find the maximum intensity in the disparity image
	int max = 0;
	for (int row = 0; row < height; ++row)
	{
		for (int col = 0; col < width; ++col)
		{
			int gray = qGray(disparity.pixel(col, row));
			if (gray > max)
			{
				max = gray;
			}
		}
	}

	// calculate UDisparity
	for (int col = 0; col < width; ++col)
	{
		unsigned char colHistogram[256] = { 0 };
		for (int row = 0; row < height; ++row)
		{
			int gray = qGray(disparity.pixel(col, row));

			if (gray != 255 && gray > 5)
			{
				++colHistogram[gray];
			}
		}

		for (int row = 0; row < 256; ++row)
		{
			if (colHistogram[row] < uvDisparityThreshold)
			{
				colHistogram[row] = 0;
			}
			
			uDisparity.setPixel(col, row, QColor(colHistogram[row], colHistogram[row], colHistogram[row]).rgb());

			double colorVal = log(colHistogram[row] + 1) / log(max) * 255;
			uDisparityLog.setPixel(col, row, QColor(colorVal, colorVal, colorVal).rgb());
		}
	}
}

void Image::HoughLinesDetection(cv::Vec4i &selectedLine, const double thresholdLineThickness)
{
	// QImage vDisparity to Mat src considering QImage::Format_RGB888
	QImage swappedSource = vDisparity.rgbSwapped();
	cv::Mat src = cv::Mat(swappedSource.height(), swappedSource.width(), CV_8UC3, const_cast<uchar*>(swappedSource.bits()), swappedSource.bytesPerLine()); // CV_8UC3 = 8-bit, 3 channel

	QImage swappedResult = vDisparityLogCopy.rgbSwapped();
	cv::Mat res = cv::Mat(swappedResult.height(), swappedResult.width(), CV_8UC3, const_cast<uchar*>(swappedResult.bits()), swappedResult.bytesPerLine());

	std::vector<cv::Vec2f> houghLines;

	cv::Mat houghLinesSrc;
	cv::cvtColor(src, houghLinesSrc, CV_BGR2GRAY);

	HoughLines(houghLinesSrc, houghLines, 1, CV_PI / 180, 150);

	if (houghLines.size() > 0)
	{
		// hough coordinates to xy coordinates
		std::vector<cv::Vec4i> lines;
		for (size_t i = 0; i < houghLines.size(); i++)
		{
			float rho = houghLines[i][0], theta = houghLines[i][1];
			cv::Point pt1, pt2;
			double a = cos(theta), b = sin(theta);
			double x0 = a*rho, y0 = b*rho;
			pt1.x = cvRound(x0 + 1000 * (-b));
			pt1.y = cvRound(y0 + 1000 * (a));
			pt2.x = cvRound(x0 - 1000 * (-b));
			pt2.y = cvRound(y0 - 1000 * (a));

			lines.push_back(cv::Vec4i(pt1.x, pt1.y, pt2.x, pt2.y));
		}

		// find the best line
		selectedLine = Utils::BestHoughLine(disparity, lines, thresholdLineThickness);

		// paint detected line
		line(res, cv::Point(selectedLine[0], selectedLine[1]), cv::Point(selectedLine[2], selectedLine[3]), cv::Scalar(0, 0, 255), 1, cv::LINE_AA);

		//// paint detected lines
		//for (size_t i = 0; i < lines.size(); i++)
		//{
		//	cv::Vec4i l = lines[i];
		//	line(res, cv::Point(l[0], l[1]), cv::Point(l[2], l[3]), cv::Scalar(0, 0, 255), 1, cv::LINE_AA);
		//}
	}

	// Mat dst to QImage vDisparityLog
	vDisparityLog = QImage((uchar *)res.data, res.cols, res.rows, res.step, QImage::Format_RGB888).rgbSwapped();
}

void Image::ProbabilisticHoughLinesDetection(cv::Vec4i &selectedLine, const double thresholdLineThickness)
{
	// QImage vDisparity to Mat src considering QImage::Format_RGB888
	QImage swappedSource = vDisparity.rgbSwapped();
	cv::Mat src = cv::Mat(swappedSource.height(), swappedSource.width(), CV_8UC3, const_cast<uchar*>(swappedSource.bits()), swappedSource.bytesPerLine()); // CV_8UC3 = 8-bit, 3 channel

	QImage swappedResult = vDisparityLogCopy.rgbSwapped();
	cv::Mat res = cv::Mat(swappedResult.height(), swappedResult.width(), CV_8UC3, const_cast<uchar*>(swappedResult.bits()), swappedResult.bytesPerLine());

	std::vector<cv::Vec4i> lines;

	cv::Mat houghLinesSrc;
	cv::cvtColor(src, houghLinesSrc, CV_BGR2GRAY);

	HoughLinesP(houghLinesSrc, lines, 1, CV_PI / 180, 150, 50, 0);

	if (lines.size() > 0)
	{
		// find the best line
		selectedLine = Utils::BestHoughLine(disparity, lines, thresholdLineThickness);

		// paint detected line
		line(res, cv::Point(selectedLine[0], selectedLine[1]), cv::Point(selectedLine[2], selectedLine[3]), cv::Scalar(0, 0, 255), 1, cv::LINE_AA);
		
		//// paint detected lines
		//for (size_t i = 0; i < lines.size(); i++)
		//{
		//	cv::Vec4i l = lines[i];
		//	line(res, cv::Point(l[0], l[1]), cv::Point(l[2], l[3]), cv::Scalar(0, 0, 255), 1, cv::LINE_AA);
		//}
	}

	// Mat dst to QImage vDisparityLog
	vDisparityLog = QImage((uchar *)res.data, res.cols, res.rows, res.step, QImage::Format_RGB888).rgbSwapped();
}

void Image::CudaHoughLinesDetection(cv::Vec4i &selectedLine, const double thresholdLineThickness)
{
	// QImage vDisparity to Mat src considering QImage::Format_RGB888
	QImage swappedSource = vDisparity.rgbSwapped();
	cv::Mat src = cv::Mat(swappedSource.height(), swappedSource.width(), CV_8UC3, const_cast<uchar*>(swappedSource.bits()), swappedSource.bytesPerLine()); // CV_8UC3 = 8-bit, 3 channel

	QImage swappedResult = vDisparityLogCopy.rgbSwapped();
	cv::Mat res = cv::Mat(swappedResult.height(), swappedResult.width(), CV_8UC3, const_cast<uchar*>(swappedResult.bits()), swappedResult.bytesPerLine());

	cv::Mat mask;
	cv::cvtColor(src, mask, CV_BGR2GRAY);

	cv::cuda::GpuMat d_src(mask);
	cv::cuda::GpuMat d_lines;

	cv::Ptr<cv::cuda::HoughLinesDetector> hough = cv::cuda::createHoughLinesDetector(1.0f, (float)(CV_PI / 180.0f), 150);
	hough->detect(d_src, d_lines);

	std::vector<cv::Vec2f> lines_gpu;
	hough->downloadResults(d_lines, lines_gpu);

	if (!lines_gpu.empty()) {
		// hough coordinates to xy coordinates
		std::vector<cv::Vec4i> lines;
		for (size_t i = 0; i < lines_gpu.size(); i++)
		{
			float rho = lines_gpu[i][0], theta = lines_gpu[i][1];
			cv::Point pt1, pt2;
			double a = cos(theta), b = sin(theta);
			double x0 = a*rho, y0 = b*rho;
			pt1.x = cvRound(x0 + 1000 * (-b));
			pt1.y = cvRound(y0 + 1000 * (a));
			pt2.x = cvRound(x0 - 1000 * (-b));
			pt2.y = cvRound(y0 - 1000 * (a));

			lines.push_back(cv::Vec4i(pt1.x, pt1.y, pt2.x, pt2.y));
		}

		// find the best line
		selectedLine = Utils::BestHoughLine(disparity, lines, thresholdLineThickness);

		// paint detected line
		line(res, cv::Point(selectedLine[0], selectedLine[1]), cv::Point(selectedLine[2], selectedLine[3]), cv::Scalar(0, 0, 255), 1, cv::LINE_AA);

		// paint detected lines
		//for (size_t i = 0; i < lines_gpu.size(); i++)
		//{
		//	cv::Vec4i l = lines_gpu[i];
		//	line(res, cv::Point(l[0], l[1]), cv::Point(l[2], l[3]), cv::Scalar(0, 0, 255), 1, cv::LINE_AA);
		//}
	}

	// Mat dst to Qsource result
	vDisparityLog = QImage((uchar *)res.data, res.cols, res.rows, res.step, QImage::Format_RGB888).rgbSwapped();
}

void Image::CudaProbabilisticHoughLinesDetection(cv::Vec4i &selectedLine, const double thresholdLineThickness)
{
	// QImage vDisparity to Mat src considering QImage::Format_RGB888
	QImage swappedSource = vDisparity.rgbSwapped();
	cv::Mat src = cv::Mat(swappedSource.height(), swappedSource.width(), CV_8UC3, const_cast<uchar*>(swappedSource.bits()), swappedSource.bytesPerLine()); // CV_8UC3 = 8-bit, 3 channel

	QImage swappedResult = vDisparityLogCopy.rgbSwapped();
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

	if (!lines_gpu.empty()) {
		// find the best line
		selectedLine = Utils::BestHoughLine(disparity, lines_gpu, thresholdLineThickness);

		// paint detected line
		line(res, cv::Point(selectedLine[0], selectedLine[1]), cv::Point(selectedLine[2], selectedLine[3]), cv::Scalar(0, 0, 255), 1, cv::LINE_AA);

		// paint detected lines
		//for (size_t i = 0; i < lines_gpu.size(); i++)
		//{
		//	cv::Vec4i l = lines_gpu[i];
		//	line(res, cv::Point(l[0], l[1]), cv::Point(l[2], l[3]), cv::Scalar(0, 0, 255), 1, cv::LINE_AA);
		//}
	}

	// Mat dst to Qsource result
	vDisparityLog = QImage((uchar *)res.data, res.cols, res.rows, res.step, QImage::Format_RGB888).rgbSwapped();
}

Image::~Image()
{
}
