#include "uv_disparity.h"

void v_disparity(const QImage &source, QImage &result, std::chrono::duration<double> &elapsed_seconds)
{
	int width = source.width();
	int height = source.height();
	int startRow = height / 2;

	std::chrono::time_point<std::chrono::system_clock> start, end;
	start = std::chrono::system_clock::now();

	for (int row = startRow; row < height; ++row)
	{
		unsigned char rowHistogram[256] = { 0 };
		for (int col = 0; col < width; ++col)
		{
			int gray = qGray(source.pixel(col, row));
			if (gray != 255 && gray > 5)
			{
				++rowHistogram[qGray(source.pixel(col, row))]; 
			}
		}

		for (int col = 0; col < 256; ++col)
		{
			QColor color(rowHistogram[col], rowHistogram[col], rowHistogram[col]);
			result.setPixel(col, row, color.rgb());
		}
	}

	end = std::chrono::system_clock::now();
	elapsed_seconds = end - start;
}

void v_disparity(const QImage &source, const QImage &mask, QImage &result, std::chrono::duration<double> &elapsed_seconds)
{
	int width = source.width();
	int height = source.height();

	std::chrono::time_point<std::chrono::system_clock> start, end;
	start = std::chrono::system_clock::now();

	for (int row = 0; row < height; ++row)
	{
		unsigned char rowHistogram[256] = { 0 };
		for (int col = 0; col < width; ++col)
		{
			int grayM = qGray(mask.pixel(col, row));
			int grayS = qGray(source.pixel(col, row));
			if (grayM != 255 && grayS > 5)
			{
				++rowHistogram[qGray(source.pixel(col, row))];
			}
		}

		for (int col = 0; col < 256; ++col)
		{
			QColor color(rowHistogram[col], rowHistogram[col], rowHistogram[col]);
			result.setPixel(col, row, color.rgb());
		}
	}

	end = std::chrono::system_clock::now();
	elapsed_seconds = end - start;
}

void v_disparity_N(const QImage &source, QImage &result, std::chrono::duration<double> &elapsed_seconds)
{
	int width = source.width();
	int height = source.height();
	int startRow = height / 2;

	int max = 0;

	std::chrono::time_point<std::chrono::system_clock> start, end;
	start = std::chrono::system_clock::now();

	for (int row = startRow; row < height; ++row)
	{
		for (int col = 0; col < width; ++col)
		{
			int gray = qGray(source.pixel(col, row));
			if (gray > max)
			{
				max = gray;
			}
		}
	}

	for (int row = startRow; row < height; ++row)
	{
		unsigned char rowHistogram[256] = { 0 };
		for (int col = 0; col < width; ++col)
		{
			int gray = qGray(source.pixel(col, row));
			if (gray != 255 && gray > 5)
			{
				++rowHistogram[qGray(source.pixel(col, row))];
			}
		}

		for (int col = 0; col < 256; ++col)
		{
			double colorVal = log(rowHistogram[col] + 1) / log(max) * 255;
			QColor color(colorVal, colorVal, colorVal);
			result.setPixel(col, row, color.rgb());
		}
	}

	end = std::chrono::system_clock::now();
	elapsed_seconds = end - start;
}

void v_disparity_N(const QImage &source, const QImage &mask, QImage &result, std::chrono::duration<double> &elapsed_seconds)
{
	int width = source.width();
	int height = source.height();

	int max = 0;

	std::chrono::time_point<std::chrono::system_clock> start, end;
	start = std::chrono::system_clock::now();

	for (int row = 0; row < height; ++row)
	{
		for (int col = 0; col < width; ++col)
		{
			int gray = qGray(source.pixel(col, row));
			if (gray > max)
			{
				max = gray;
			}
		}
	}

	for (int row = 0; row < height; ++row)
	{
		unsigned char rowHistogram[256] = { 0 };
		for (int col = 0; col < width; ++col)
		{
			int grayM = qGray(mask.pixel(col, row));
			int grayS = qGray(source.pixel(col, row));
			if (grayM != 255 && grayS > 5)
			{
				++rowHistogram[qGray(source.pixel(col, row))];
			}
		}

		for (int col = 0; col < 256; ++col)
		{
			double colorVal = log(rowHistogram[col] + 1) / log(max) * 255;
			QColor color(colorVal, colorVal, colorVal);
			result.setPixel(col, row, color.rgb());
		}
	}

	end = std::chrono::system_clock::now();
	elapsed_seconds = end - start;
}

void u_disparity(const QImage &source, QImage &result, std::chrono::duration<double> &elapsed_seconds) {
	int width = source.width();
	int height = source.height();

	std::chrono::time_point<std::chrono::system_clock> start, end;
	start = std::chrono::system_clock::now();

	for (int col = 0; col < width; ++col)
	{
		unsigned char colHistogram[256] = { 0 };
		for (int row = 0; row < height; ++row)
		{
			++colHistogram[qGray(source.pixel(col, row))];
		}

		for (int row = 0; row < 256; ++row)
		{
			QColor color(colHistogram[row], colHistogram[row], colHistogram[row]);
			result.setPixel(col, row, color.rgb());
		}
	}

	end = std::chrono::system_clock::now();

	elapsed_seconds = end - start;
}