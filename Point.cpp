#include "Point.h"

#include <iostream>
using namespace std;

Point::Point(){
	x = 0;
	y = 0;
}

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

TGAImage Point::tracer(TGAImage image, TGAColor color){
	image.set(x, y, color);
	return image;
}
