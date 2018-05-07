#include "Line.h"

#include <iostream>
using namespace std;

Line::Line() {
	x1 = 0;
	y1 = 0;
	x2 = 0;
	y2 = 0;
}

Line::Line(int x, int y, int xBis, int yBis) {
	x1 = x;
	y1 = y;
	x2 = xBis;
	y2 = xBis;
}

int Line::getX1() {
	return x1;
}

int Line::getY1() {
	return y1;
}

int Line::getX2() {
	return x2;
}

int Line::getY2() {
	return y2;
}

TGAImage Line::tracer(TGAImage &image, TGAColor color) {
	bool steep = false;
	if (std::abs(x1 - x2) < std::abs(y1 - y2)) { // if the line is steep, we transpose the image
		std::swap(x1, y1);
		std::swap(x2, y2);
		steep = true;
	}
	if (x1 > x2) { // make it left−to−right
		std::swap(x1, x2);
		std::swap(y1, y2);
	}
	for (int x = x1; x <= x2; x++) {
		float t = (float) (x - x1) / (float) (x2 - x1);
		int y = y1 * (1. - t) + y2 * t;
		if (steep) {
			image.set(y, x, color); // if transposed, de−transpose
		} else {
			image.set(x, y, color);
		}
	}
	return image;
}

