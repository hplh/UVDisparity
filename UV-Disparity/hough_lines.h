#pragma once

#include <ctime>
#include <chrono>
#include <cmath>

#include <QImage>
#include <QColor>

#include "opencv2/core.hpp"
#include <opencv2/core/utility.hpp>
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/cudaimgproc.hpp"

void HoughLinesDetection(const QImage &source, QImage &result, std::chrono::duration<double> &elapsed_seconds);

void ProbabilisticHoughLinesDetection(const QImage &source, QImage &result, cv::Vec4i &selectedLine, std::chrono::duration<double> &elapsed_seconds);

void CudaHoughLinesDetection(const QImage &source, QImage &result, std::chrono::duration<double> &elapsed_seconds);

void CudaProbabilisticHoughLinesDetection(const QImage &source, QImage &result, cv::Vec4i &selectedLine, std::chrono::duration<double> &elapsed_seconds);

void PointsOnLine(const QImage &source, const cv::Vec4i line, std::vector<cv::Vec2i> &points, const double threshold = 1);

void GroundPlane(const QImage &source, std::vector<cv::Vec2i> &points, std::vector<cv::Vec2i> &groundPlanePoints, const double threshold = 0.02);