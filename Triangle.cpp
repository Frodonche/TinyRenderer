#include "Triangle.h"

#include <iostream>
using namespace std;

// Le constructeur defini les lignes du triangle a partir des points qui lui sont passes en parametres
Triangle::Triangle(Point p, Point pBis, Point pSecond) {
	ligne1 = Line(p.getX(), p.getY(), pBis.getX(), pBis.getY());
	ligne2 = Line(pBis.getX(), pBis.getY(), pSecond.getX(), pSecond.getY());
	ligne3 = Line(p.getX(), p.getY(), pSecond.getX(), pSecond.getY());
}

Line Triangle::getLigne1() {
	return ligne1;
}

Line Triangle::getLigne2() {
	return ligne2;
}

Line Triangle::getLigne3() {
	return ligne3;
}

// On trace successivement les 3 lignes du triangle
TGAImage Triangle::tracer(TGAImage image, TGAColor color){
	image = ligne1.tracer(image, color);
	image = ligne2.tracer(image, color);
	image = ligne3.tracer(image, color);
	return image;
}
