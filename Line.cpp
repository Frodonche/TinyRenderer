#include "Line.h"

#include <iostream>
using namespace std;

Line::Line(){
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

TGAImage Line::tracer(TGAImage image, TGAColor color){
	 bool estRaide = false;
	    if (std::abs(x1-x2) < std::abs(y1-y2)) {
	        std::swap(x1, y1);
	        std::swap(x2, y2);
	        estRaide = true;
	    }

	    if (x1 > x2) {
	        std::swap(x1, x2);
	        std::swap(y1, y2);
	    }

	    int dx = x2-x1;
	    int dy = y2-y1;
	    int derror = std::abs(dy)*2;
	    int error = 0;
	    int y = y1;
	    for (int x = x1; x <= x2; x++) {
	        if (estRaide) {
	            image.set(y, x, color);
	        } else {
	            image.set(x, y, color);
	        }
	        error += derror;
	        if (error > dx) {
	        	if(y2 > y1){
	        		y += 1;
	        	}else{
	        		y -= 1;
	        	}
	            error -= dx*2;
	        }
	    }
	return image;
}
