#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <regex>
#include <vector>

#include "tgaimage.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
const TGAColor green = TGAColor(0, 255, 0, 255);

const float COTE = 500.0;

using namespace std;

typedef struct vector3is vector3i;
struct vector3is {
  int premier; // x si c'est un point
  int deuxieme; // y si c'est un point
  int troisieme; // z si c'est un point
};

typedef struct vector3fs vector3f;
struct vector3fs {
	float x;
	float y;
	float z;
};


void line(int x1, int y1, int x2, int y2, TGAImage &image, TGAColor color){
	bool steep = false;
		if (abs(x1 - x2) < abs(y1 - y2)) { // if the line is steep, we transpose the image
			swap(x1, y1);
			swap(x2, y2);
			steep = true;
		}
		if (x1 > x2) { // make it left−to−right
			swap(x1, x2);
			swap(y1, y2);
		}
		for (int x = x1; x <= x2; x++) {
			float t = (float) (x - x1) / (float) (x2 - x1);
			int y = y1 * (1. - t) + y2 * t;
			if (steep) {
				image.set(y, x, color); // if transposed, de−transpose
			} else {
				image.set(x, y, color);
			}
		}
}

// Trace un triangle reliant les trois points d'indice indice1, 2 et 3
void tracer_triangle(vector3i point1, vector3i point2, vector3i point3, TGAImage &image, TGAColor color){
	line(point1.premier, point1.deuxieme, point2.premier, point2.deuxieme, image, color);
	line(point2.premier, point2.deuxieme, point3.premier, point3.deuxieme, image, color);
	line(point3.premier, point3.deuxieme, point1.premier, point1.deuxieme, image, color);
}


//Calcul du barycentre pour definir si le point pTest est dans le triangle defini par les points point1, 2 et 3
vector3f barycentre(vector3i * triangle, vector3i P) {
	vector3f result;
	vector3f v1 = {(float)triangle[2].premier - (float)triangle[0].premier, (float)triangle[1].premier - (float)triangle[0].premier, (float)triangle[0].premier - (float)P.premier};
	vector3f v2 = {(float)triangle[2].deuxieme - (float)triangle[0].deuxieme, (float)triangle[1].deuxieme - (float)triangle[0].deuxieme, (float)triangle[0].deuxieme - (float)P.deuxieme};

	// cross product de v1 et v2
	vector3f crossProduct = {(v1.y * v2.z) - (v1.z * v2.y), (v1.z * v2.x) - (v1.x * v2.z), (v1.x * v2.y) - (v1.y * v2.x)};

	// si on est hors du triangle, on rend une valeur négative
    if (abs(crossProduct.z)<1){
    	result = {-1,-1,-1};
    }else{
    	result = {1.f - (crossProduct.x + crossProduct.y) / crossProduct.z, crossProduct.y / crossProduct.z, crossProduct.x / crossProduct.z};
    }

    return result;
}

// Remplit l'intérieur du triangle
void remplir_triangle(vector3i point1, vector3i point2, vector3i point3, TGAImage &image, TGAColor color){
	vector3i triangle[3] = {point1, point2, point3};

	// On trie les points par ordre croissant des y (petit tri a bulles)
	if(triangle[0].deuxieme > triangle[1].deuxieme)
		swap(triangle[0], triangle[1]);
	if(triangle[0].deuxieme > triangle[2].deuxieme)
		swap(triangle[0], triangle[2]);
	if(triangle[1].deuxieme > triangle[2].deuxieme)
		swap(triangle[1], triangle[2]);

	// On initialise les bords de la boite
	 vector3i bordMax = {(int)COTE-1, (int)COTE-1, 0};
	 vector3i bordMin = {0, 0, 0};

	 vector3i constMax = {(int)COTE-1, (int)COTE-1, 0};
	 vector3i constMin = {0, 0, 0};

	for (int i=0; i<3; i++) {
		bordMax.premier = max((int)constMin.premier, min((int)bordMax.premier, (int)triangle[i].premier));
		bordMin.premier = min((int)constMax.premier, max((int)bordMin.premier, (int)triangle[i].premier));
		bordMax.deuxieme = max((int)constMin.deuxieme, min((int)bordMax.deuxieme, (int)triangle[i].deuxieme));
		bordMin.deuxieme = min((int)constMax.deuxieme, max((int)bordMin.deuxieme, (int)triangle[i].deuxieme));
	}

	vector3i P;
	for (P.premier = bordMax.premier; P.premier <= bordMin.premier; P.premier++) {
		for (P.deuxieme = bordMax.deuxieme; P.deuxieme <= bordMin.deuxieme; P.deuxieme++) {
			vector3f bary  = barycentre(triangle, P);
			if (bary.x < 0 || bary.y < 0 || bary.z < 0) continue; // On prevoie les cas ou les coordonnes sortiraient de l'ecran
			image.set(P.premier, P.deuxieme, color);
		}
	}

}

// Ouvre le fichier et range la liste des points
void openfile(string filename, TGAImage &image) {
	vector<vector3i> lesPoints;

	const char * name = filename.c_str();

	ifstream in(name); // on essaye d'ouvrir le fichier

	// si on ne peut pas ouvrir le fichier
	if (!in)
		cout << "Cannot open input file.\n";

	regex patternV { "^v " };

	regex patternF { "^f " };

	string str;
	string delimiter = " ";
	string delimiter2 = "/";
	bool matchV, matchF;

	int cpt;
	int xTemp, yTemp, zTemp;
	int indice1, indice2, indice3;

	string token, token2;
	size_t pos, pos2;

	vector3i tempPoint;

	while (getline(in, str)) {
		// A chaque ligne, on regarde de quel type il s'agit, et on les range dans les tableaux appropries
		matchV = regex_search(str, patternV);

		matchF = regex_search(str, patternF);

		// Si c'est une ligne avec un point, on cree un point qu'on stocke dans la liste
		if (matchV) {
			//pos = 0;
			cpt = 0;

			while ((pos = str.find(delimiter)) != std::string::npos) {
				token = str.substr(0, pos);
				// si on est sur le x
				if (cpt == 1) {
					//cout << token.c_str();
					xTemp = (int)((atof(token.c_str()) + 1.) * COTE / 2.);
				}

				// si on est sur le y
				if (cpt == 2) {
					yTemp = (int)((atof(token.c_str()) + 1.) * COTE / 2.);
					//	cout << " " << token.c_str() << endl;
				}

				// si on est sur le z
				if (cpt == 3) {
					zTemp = (int)((atof(token.c_str()) + 1.) * COTE / 2.);
					//	cout << " " << token.c_str() << endl;
				}
				str.erase(0, pos + delimiter.length());
				cpt += 1;
			}

			//cout << xTemp << " " << yTemp << endl;

			tempPoint.premier = xTemp;
			tempPoint.deuxieme = yTemp;
			tempPoint.troisieme = zTemp;

			lesPoints.push_back(tempPoint);
			image.set(tempPoint.premier, tempPoint.deuxieme, white);

		}

		// si on trouve une ligne qui symbolise un triangle
		if (matchF == true) {
			pos = 0;
			cpt = 0;

			// on delimite les 3 parametres
			while ((pos = str.find(delimiter)) != std::string::npos) {
				token = str.substr(0, pos);
				pos2 = 0;

				// si on a depasse le caractere f
				if (cpt > 0) {
					pos2 = token.find(delimiter2);
					token2 = token.substr(0, pos2);

					// si on est sur le premier groupe de X/X/X
					if (cpt == 1) {
						indice1 = atoi(token2.c_str()) - 1;
					}

					// si on est sur le 2eme groupe de X/X/X
					if (cpt == 2) {
						indice2 = atoi(token2.c_str()) - 1;
					}

				}
				str.erase(0, pos + delimiter.length());
				cpt += 1;
			}
			// reste le 3eme groupe de X/X/X
			pos2 = str.find(delimiter2);
			token2 = str.substr(0, pos2);
			indice3 = atoi(str.c_str()) - 1;

			tracer_triangle(lesPoints.at(indice1), lesPoints.at(indice2), lesPoints.at(indice3), image, white);

		}
	}

}

int main(int argc, char** argv) {
	TGAImage image(COTE, COTE, TGAImage::RGB);

	vector3i point1 = {10, 10, 1};
	vector3i point2 = {100, 30, 1};
	vector3i point3 = {190, 160, 1};
	remplir_triangle(point1, point2, point3, image, white);

	//openfile("african_head.obj", image);

	image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
	image.write_tga_file("output.tga");
	return 0;
}

