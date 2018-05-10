#ifndef POINT_H_
#define POINT_H_

#include "tgaimage.h"

class Point{
	int x;
	int y;
	int z;

public:
  Point();
  Point(int xParam,int yParam, int zParam);
  int getX();
  int getY();
  int getZ();
  void tracer(TGAImage &image, TGAColor color);
};

#endif
