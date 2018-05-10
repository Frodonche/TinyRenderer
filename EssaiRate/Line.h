#ifndef LINE_H_
#define LINE_H_

#include "tgaimage.h"

class Line
{
	int x1;
	int y1;
	int x2;
	int y2;

public:
  Line();
  Line(int x,int y, int xBis, int yBis);
  int getX1();
  int getY1();
  int getX2();
  int getY2();
  void tracer(TGAImage &image, TGAColor color);
};

#endif // LINE_H
