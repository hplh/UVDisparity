#pragma once

#include "Image.h"

#include <QPainter>

class GroundPlane
{
private:
	QImage imagePixelsFromHoughLine;
	QImage detectedPlane;
	QImage drownOnSource;

	// TUI Camera;
	const double f = 486.24;
	const double B = 0.119947;
	const double c_u = 320;
	const double c_v = 240;

public:
	GroundPlane();

	QImage& getImagePixelsFromHoughLine();
	QImage& getDetectedPlane();
	QImage& getDrownOnSource();

	void paintImagePixelsFromHoughLine(const QImage &image, std::vector<cv::Vec2i> &points);
	void paintDetectedPlane(const QImage &image, std::vector<cv::Vec2i> &points);
	void paintDrownOnSource(const QImage &image, std::vector<cv::Vec2i> &points);

	void saveMask(QString filename, int width, int height, std::vector<cv::Vec2i> &groundPlanePoints);

	void Detect(Image &image, HoughLinesMethods houghLinesMethod, const double thresholdHoughLineThickness, const double thresholdGroundPlaneThickness = 0.05);

	~GroundPlane();
};

