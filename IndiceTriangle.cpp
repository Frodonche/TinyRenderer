#include "IndiceTriangle.h"

#include <iostream>
using namespace std;

IndiceTriangle::IndiceTriangle(){
	premierIndice = 0;
	deuxiemeIndice = 0;
	troisiemeIndice = 0;
}

IndiceTriangle::IndiceTriangle(int premier, int deuxieme, int troisieme){
	premierIndice = premier;
	deuxiemeIndice = deuxieme;
	troisiemeIndice = troisieme;
}

int IndiceTriangle::getPremierIndice(){
	return premierIndice;
}

int IndiceTriangle::getDeuxiemeIndice(){
	return deuxiemeIndice;
}

int IndiceTriangle::getTroisiemeIndice(){
	return troisiemeIndice;
}
