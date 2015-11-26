#pragma once

#include <ctime>
#include <chrono>
#include <cmath>

#include <QImage>

#include "opencv2/core.hpp"
#include <opencv2/core/utility.hpp>
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/cudaimgproc.hpp"

void HoughLinesDetection(const QImage &source, QImage &result, std::chrono::duration<double> &elapsed_seconds);

void ProbabilisticHoughLinesDetection(const QImage &source, QImage &result, cv::Vec4i &selectedLine, std::chrono::duration<double> &elapsed_seconds);

void CudaHoughLinesDetection(const QImage &source, QImage &result, std::chrono::duration<double> &elapsed_seconds);

void CudaProbabilisticHoughLinesDetection(const QImage &source, QImage &result, cv::Vec4i &selectedLine, std::chrono::duration<double> &elapsed_seconds);