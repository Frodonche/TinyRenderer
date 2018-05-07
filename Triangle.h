#ifndef TRIANGLE_H_
#define TRIANGLE_H_

#include "tgaimage.h"
#include "Point.h"
#include "Line.h"

class Triangle
{
	Line ligne1;
	Line ligne2;
	Line ligne3;

public:
  Triangle();
  Triangle(Point p, Point pBis, Point pSecond);
  Line getLigne1();
  Line getLigne2();
  Line getLigne3();
  TGAImage tracer(TGAImage image, TGAColor color);
};

#endif
