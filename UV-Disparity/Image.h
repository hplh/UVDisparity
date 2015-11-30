#pragma once

#include <iostream>
#include <ctime>
#include <chrono>
#include <vector>

#include <QImage>
#include <QColor>

#include "opencv2/core.hpp"
#include <opencv2/core/utility.hpp>
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/cudaimgproc.hpp"

#include "Utils.h"

typedef enum {
	houghLines,
	probabilisticHoughLines,
	cudaHoughLines,
	cudaProbabilisticHoughLines
} HoughLinesMethods;

class Image
{
private:
	QImage source;
	QImage disparity;
	QImage mask;

	QImage vDisparity;
	QImage vDisparityLog;
	QImage vDisparityLogCopy;
	QImage uDisparity;
	QImage uDisparityLog;

	unsigned int startRow = 0;

	unsigned int uvDisparityThreshold = 0;

public:
	Image();
	
	void Load(QString sourceFile, QString disparityFile, QString maskFile);

	QImage& getSource();
	QImage& getDisparity();
	QImage& getMask();
	QImage& getVDisparity();
	QImage& getVDisparityLog();
	QImage& getUDisparity();
	QImage& getUDisparityLog();

	void setUVDisparityThreshold(int value);

	void VDisparity();
	void UDisparity();

	void HoughLinesDetection(cv::Vec4i &line, const double thresholdLineThickness);
	void ProbabilisticHoughLinesDetection(cv::Vec4i &line, const double thresholdLineThickness);
	void CudaHoughLinesDetection(cv::Vec4i &line, const double thresholdLineThickness);
	void CudaProbabilisticHoughLinesDetection(cv::Vec4i &line, const double thresholdLineThickness);

	~Image();
};

