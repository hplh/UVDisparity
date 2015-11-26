#pragma once

#include <ctime>
#include <chrono>
#include <vector>

#include <QImage>
#include <QColor>

void VDisparity(const QImage &source, QImage &result, std::chrono::duration<double> &elapsed_seconds, const unsigned int threshold = 2);
void VDisparity(const QImage &source, const QImage &mask, QImage &result, std::chrono::duration<double> &elapsed_seconds, const unsigned int threshold = 2);

void VDisparityNormalized(const QImage &source, QImage &result, const unsigned int threshold = 2);
void VDisparityNormalized(const QImage &source, const QImage &mask, QImage &result, const unsigned int threshold = 2);

void UDisparity(const QImage &source, QImage &result, std::chrono::duration<double> &elapsed_seconds);
