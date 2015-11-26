#pragma once

#include <QImage>
#include <QColor>
#include "opencv2/core.hpp"

void PointsOnLine(const QImage &source, const cv::Vec4i line, std::vector<cv::Vec2i> &points, const double threshold = 1);

void GroundPlane(const QImage &source, std::vector<cv::Vec2i> &points, std::vector<cv::Vec2i> &groundPlanePoints, const double threshold = 0.02);