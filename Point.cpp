#include "Point.h"

#include <iostream>
using namespace std;

Point::Point(int xParam, int yParam) {
	x = xParam;
	y = yParam;
}

int Point::getX() {
	return x;
}

int Point::getY() {
	return y;
}
