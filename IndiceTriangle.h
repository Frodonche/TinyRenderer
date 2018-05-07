#ifndef INDICETRIANGLE_H_
#define INDICETRIANGLE_H_

class IndiceTriangle{
	int premierIndice;
	int deuxiemeIndice;
	int troisiemeIndice;

public:
  IndiceTriangle();
  IndiceTriangle(int premier,int deuxieme, int troisieme);
  int getPremierIndice();
  int getDeuxiemeIndice();
  int getTroisiemeIndice();
};

#endif
