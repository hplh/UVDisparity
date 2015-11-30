#pragma once

#include <vector>

#include <QImage>

#include "opencv2/core.hpp"

class Utils
{
public:
	Utils();

	static std::vector<cv::Vec2i> HoughLineToImagePixels(const QImage &image, const cv::Vec4i &line, const double thresholdLineThickness);
	static unsigned int LineConsistency(const QImage &image, const cv::Vec4i &line, const double thresholdLineThickness);
	static cv::Vec4i BestHoughLine(const QImage &image, const std::vector<cv::Vec4i> &lines, const double thresholdLineThickness);

	~Utils();
};

