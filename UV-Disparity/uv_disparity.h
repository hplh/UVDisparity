#pragma once

#include <ctime>
#include <chrono>
#include <vector>

#include <QImage>
#include <QColor>

void v_disparity(const QImage &source, QImage &result, std::chrono::duration<double> &elapsed_seconds);
void v_disparity(const QImage &source, const QImage &mask, QImage &result, std::chrono::duration<double> &elapsed_seconds);

void v_disparity_N(const QImage &source, QImage &result, std::chrono::duration<double> &elapsed_seconds);
void v_disparity_N(const QImage &source, const QImage &mask, QImage &result, std::chrono::duration<double> &elapsed_seconds);

void u_disparity(const QImage &source, QImage &result, std::chrono::duration<double> &elapsed_seconds);
