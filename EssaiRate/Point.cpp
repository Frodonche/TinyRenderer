#include "Point.h"

#include <iostream>
using namespace std;

Point::Point(){
	x = 0;
	y = 0;
	z = 0;
}

Point::Point(int xParam, int yParam, int zParam) {
	x = xParam;
	y = yParam;
	z = zParam;
}

int Point::getX() {
	return x;
}

int Point::getY() {
	return y;
}

int Point::getZ(){
	return z;
}

void Point::tracer(TGAImage &image, TGAColor color){
	image.set(x, y, color);
	//return image;
}
