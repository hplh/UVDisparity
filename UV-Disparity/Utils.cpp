#include "Utils.h"


Utils::Utils()
{
}

std::vector<cv::Vec2i> Utils::HoughLineToImagePixels(const QImage &image, const cv::Vec4i &line, const double thresholdLineThickness)
{
	std::vector<cv::Vec2i> pixels;

	int width = image.width();
	int height = image.height();

	double a = (double)(line[3] - line[1]) / (line[2] - line[0]);
	double b = (double)line[1] - a * line[0];

	double distance;

	int rowStart = (line[1] < line[3]) ? line[1] : line[3];
	int colStart = (line[0] < line[2]) ? line[0] : line[2];

	for (int row = 0; row < height; ++row)
	{
		for (int col = 0; col < width; ++col)
		{
			double x0 = qGray(image.pixel(col, row));
			double y0 = row;

			if (row > rowStart && col > colStart)
			{
				distance = fabs((double)y0 - a * x0 - b) / sqrt(1.0 + a * a);
				if (distance <= thresholdLineThickness) {
					pixels.push_back(cv::Vec2i(col, row));
				}
			}
		}
	}

	return pixels;
}

unsigned int Utils::LineConsistency(const QImage &image, const cv::Vec4i &line, const double thresholdLineThickness)
{
	int width = image.width();
	int height = image.height();

	double a = (double)(line[3] - line[1]) / (line[2] - line[0]);
	
	//double slope = atan(a) * 180 / CV_PI;
	//if (slope > 150 || slope < 110) {
	//	return 0;
	//}
	
	double b = (double)line[1] - a * line[0];

	double distance;

	int rowStart = (line[1] < line[3]) ? line[1] : line[3];
	int colStart = (line[0] < line[2]) ? line[0] : line[2];

	unsigned int pixelsOnLine = 0;

	for (int row = 0; row < height; ++row)
	{
		for (int col = 0; col < width; ++col)
		{
			double x0 = col;
			double y0 = row;

			if (row > rowStart && col > colStart)
			{
				distance = fabs((double)y0 - a * x0 - b) / sqrt(1.0 + a * a);
				if (distance <= thresholdLineThickness) {
					++pixelsOnLine;
				}
			}
		}
	}

	return pixelsOnLine;
}

cv::Vec4i Utils::BestHoughLine(const QImage &image, const std::vector<cv::Vec4i> &lines, const double thresholdLineThickness)
{
	unsigned int bestLineConsistency = 0;
	cv::Vec4i bestLine;

	for (auto currentLine = lines.begin(); currentLine != lines.end(); ++currentLine)
	{
		unsigned int currentLineConsistency = LineConsistency(image, *currentLine, thresholdLineThickness);
		if (currentLineConsistency > bestLineConsistency)
		{
			bestLineConsistency = currentLineConsistency;
			bestLine = *currentLine;
		}
	}

	return bestLine;
}

Utils::~Utils()
{
}
