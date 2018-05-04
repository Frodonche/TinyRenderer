#ifndef POINT_H_
#define POINT_H_

#include "tgaimage.h"

class Point{
	int x;
	int y;

public:
  Point();
  Point(int xParam,int yParam);
  int getX();
  int getY();
  TGAImage tracer(TGAImage image, TGAColor color);
};

#endif
