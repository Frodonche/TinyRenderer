#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <regex>
#include <vector>
#include <cmath>

#include "tgaimage.h"

using namespace std;

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
const TGAColor green = TGAColor(0, 255, 0, 255);

const float COTE = 1024.0;

const float PLUS_INFINI = numeric_limits<float>::max();
const float MOINS_INFINI = - numeric_limits<float>::max(); // on ne prend pas ::min() pour s'assurer qu'on a la meme valeur que PLUS_INFINI mais en negatif

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

typedef struct vector3ds vector3d;
struct vector3ds {
	long double x;
	long double y;
	long double z;
};

void line(int x1, int y1, int x2, int y2, TGAImage &image, TGAColor color){
	bool steep = false;
		if (abs(x1 - x2) < abs(y1 - y2)) {
			swap(x1, y1);
			swap(x2, y2);
			steep = true;
		}
		if (x1 > x2) {
			swap(x1, x2);
			swap(y1, y2);
		}
		for (int x = x1; x <= x2; x++) {
			float t = (float) (x - x1) / (float) (x2 - x1);
			int y = y1 * (1. - t) + y2 * t;
			if (steep) {
				image.set(y, x, color);
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
vector3f barycentre(vector3i * triangle, vector3f P) {
	vector3f result;
	vector3f v1 = {(float)triangle[2].premier - (float)triangle[0].premier, (float)triangle[1].premier - (float)triangle[0].premier, (float)triangle[0].premier - P.x};
	vector3f v2 = {(float)triangle[2].deuxieme - (float)triangle[0].deuxieme, (float)triangle[1].deuxieme - (float)triangle[0].deuxieme, (float)triangle[0].deuxieme - P.y};

	// cross product de v1 et v2
	vector3f crossProduct = {(v1.y * v2.z) - (v1.z * v2.y), (v1.z * v2.x) - (v1.x * v2.z), (v1.x * v2.y) - (v1.y * v2.x)};

	// si on est hors du triangle, on rend une valeur négative
    if (abs(crossProduct.z) < 1){
    	result = {-1,-1,-1};
    }else{
    	result = {1.f - (crossProduct.x + crossProduct.y) / crossProduct.z, crossProduct.y / crossProduct.z, crossProduct.x / crossProduct.z};
    }

    return result;
}

// Remplit l'intérieur du triangle
void remplir_triangle(float * zbuffer, vector3i point1, vector3i point2, vector3i point3, vector3f pointTexture1, vector3f pointTexture2, vector3f pointTexture3,  TGAImage &image, float luminosite, TGAImage &texture){
	vector3i triangle[3] = {point1, point2, point3};
	vector3f pointsTexture[3] = {pointTexture1, pointTexture2, pointTexture3};

	// On trie les points par ordre croissant des y (petit tri a bulles)
	if(triangle[0].deuxieme > triangle[1].deuxieme)
		swap(triangle[0], triangle[1]);
	if(triangle[0].deuxieme > triangle[2].deuxieme)
		swap(triangle[0], triangle[2]);
	if(triangle[1].deuxieme > triangle[2].deuxieme)
		swap(triangle[1], triangle[2]);

	if(pointsTexture[0].y > pointsTexture[1].y)
		swap(pointsTexture[0], pointsTexture[1]);
	if(pointsTexture[0].y > pointsTexture[2].y)
		swap(pointsTexture[0], pointsTexture[2]);
	if(pointsTexture[1].y > pointsTexture[2].y)
		swap(pointsTexture[1], pointsTexture[2]);

	// On initialise les bords de la boite
	 vector3i bordMax = {(int)PLUS_INFINI, (int)PLUS_INFINI, 1};
	 vector3i bordMin = {(int)MOINS_INFINI, (int)MOINS_INFINI, 1};

	 vector3i constMax = {(int)COTE-1, (int)COTE-1, 1};
	 vector3i constMin = {0, 0, 0};

	for (int i = 0; i < 3; i++) {
		bordMax.premier = max((int)constMin.premier, min((int)bordMax.premier, (int)triangle[i].premier));
		bordMin.premier = min((int)constMax.premier, max((int)bordMin.premier, (int)triangle[i].premier));
		bordMax.deuxieme = max((int)constMin.deuxieme, min((int)bordMax.deuxieme, (int)triangle[i].deuxieme));
		bordMin.deuxieme = min((int)constMax.deuxieme, max((int)bordMin.deuxieme, (int)triangle[i].deuxieme));
	}

	vector3f P, pT;
	TGAColor couleur;
	for (P.x = bordMax.premier; P.x <= bordMin.premier; P.x += 1) {
		for (P.y = bordMax.deuxieme; P.y <= bordMin.deuxieme; P.y += 1) {
			vector3f bary  = barycentre(triangle, P);
			if (bary.x < 0 || bary.y < 0 || bary.z < 0) continue; // On prevoie les cas ou les coordonnes sortiraient de l'ecran

			P.z = (triangle[0].troisieme * bary.x) + (triangle[1].troisieme * bary.y) + (triangle[2].troisieme * bary.z);

			pT.x = (int)(bary.x * pointsTexture[0].x + bary.y * pointsTexture[1].x + bary.z * pointsTexture[2].x);
			if(((int)(pT.x))%2 == 0)
				pT.x += 1;
			pT.y = (int)(bary.x * pointsTexture[0].y + bary.y * pointsTexture[1].y + bary.z * pointsTexture[2].y);
			if(((int)(pT.y))%2 == 0)
							pT.y += 1;

			//cout << P.x << " " << P.y << endl;
			if (zbuffer[(int)(P.x + P.y * COTE)] < P.z) {
				zbuffer[(int)(P.x + P.y * COTE)] = P.z;
				couleur = texture.get((int)(P.x)+5, (int)(P.y)-75);
				//couleur = texture.get((int)(pT.x), (int)(pT.y)); Ceci est la ligne qui est censee etre utilisee, l'autre c'est du bricolage
				image.set(P.x, P.y, TGAColor(couleur.r * luminosite, couleur.g * luminosite, couleur.b * luminosite, 255));
			}
		}
	}

}

void normalize(vector3d &n){
	long double value = sqrt((n.x * n.x) + (n.y * n.y) + (n.z * n.z));
	n.x = n.x / value;
	n.y = n.y / value;
	n.z = n.z / value;
}

// Ouvre le fichier et range la liste des points
void openfile(string filename, TGAImage &image, TGAImage &texture) {
	vector<vector3i> lesPointsAff;
	vector<vector3f> lesPointsReels;

	const char * name = filename.c_str();

	ifstream in(name); // on essaye d'ouvrir le fichier

	// si on ne peut pas ouvrir le fichier
	if (!in)
		cout << "Cannot open input file.\n";

	regex patternV { "^v " };
	regex patternF { "^f " };
	regex patternVT {"^vt "};

	string str;
	string delimiter = " ";
	string delimiter2 = "/";
	bool matchV, matchF, matchVT;

	int cpt, cpt2;
	int xTempA, yTempA, zTempA;
	float xTempR, yTempR, zTempR;
	int indice1F, indice2F, indice3F;

	string token, token2;
	size_t pos, pos2;

	vector3f lesTemps[3];
	vector3f tempPointReel, tempNormale1, tempNormale2;
	vector3i tempPointAff;
	vector3d normale;

	// Partie eclairage
	vector3f directionLumiere = {0, 0, -1};
	float luminosite;

	// Partie textures
	vector<vector3f>lesTextures;
	float xTempT, yTempT;
	int indice1T, indice2T, indice3T;

	 // On initialise le zbuffer à -Infini
	float zbuffer[(int)(COTE) * (int)(COTE)] = {MOINS_INFINI};

	while (getline(in, str)) {
		// A chaque ligne, on regarde de quel type il s'agit, et on les range dans les tableaux appropries
		matchV = regex_search(str, patternV);
		matchF = regex_search(str, patternF);
		matchVT = regex_search(str,patternVT);

		// Si c'est une ligne avec un point, on cree un point qu'on stocke dans la liste
		if (matchV) {
			//pos = 0;
			cpt = 0;

			while ((pos = str.find(delimiter)) != std::string::npos) {
				token = str.substr(0, pos);
				// si on est sur le x
				if (cpt == 1) {
					//cout << token.c_str();
					xTempA = (int)((atof(token.c_str()) + 1.) * COTE / 2.);
					xTempR = atof(token.c_str());
				}

				// si on est sur le y
				if (cpt == 2) {
					yTempA = (int)((atof(token.c_str()) + 1.) * COTE / 2.);
					yTempR = atof(token.c_str());
					//	cout << " " << token.c_str() << endl;
				}

				str.erase(0, pos + delimiter.length());
				cpt += 1;
			}
			// si on est sur le z
			pos = str.find(delimiter);
			token = str.substr(0, pos);
			zTempA = (int)((atof(token.c_str()) + 1.) * COTE / 2.);
			zTempR = atof(token.c_str());

			tempPointReel.x = xTempR;
			tempPointReel.y = yTempR;
			tempPointReel.z = zTempR;

			tempPointAff.premier = xTempA;
			tempPointAff.deuxieme = yTempA;
			tempPointAff.troisieme = zTempA;

			lesPointsAff.push_back(tempPointAff);
			lesPointsReels.push_back(tempPointReel);

			// Pour afficher les points
//			image.set(tempPointAff.premier, tempPointAff.deuxieme, white);

		}

		// Si c'est une ligne vt avec les indices de textures
		if (matchVT == true){
			cpt = 0;
			while((pos = str.find(delimiter)) != std::string::npos){
				token = str.substr(0, pos);
				if(cpt == 2){
					xTempT = (atof(token.c_str()) + 1.) * 1024 / 3.;
					//xTempT = atof(token.c_str())+1;
				}
				if(cpt == 3){
					yTempT = (atof(token.c_str()) + 1.) * 1024 / 3.;
					//yTempT = atof(token.c_str())+1;
				}
				str.erase(0, pos + delimiter.length());
				cpt++;
			}

			vector3f temp = {xTempT, yTempT, 0};
			lesTextures.push_back(temp);
		}

		// si on trouve une ligne qui symbolise un triangle
		if (matchF == true) {
			pos = 0;
			cpt = 0;

			// on delimite les 3 parametres
			while ((pos = str.find(delimiter)) != std::string::npos) {
				token = str.substr(0, pos);
				pos2 = 0;
				cpt2 = 0;

				// si on a depasse le caractere f
				if (cpt > 0) {

					while((pos2 = token.find(delimiter2)) != std::string::npos){
						token2 = token.substr(0, pos2);

						// si on est sur le premier groupe de X/X/X
						if (cpt == 1) {
							if(cpt2 == 0) // si on est sur le premier X
								indice1F = atoi(token2.c_str()) - 1;
							if(cpt2 == 1) // si on est sur le deuxieme X
								indice1T = atoi(token2.c_str()) - 1;
						}

						// si on est sur le 2eme groupe de X/X/X
						if (cpt == 2) {
							if(cpt2 == 0) // si on est sur le premier X
								indice2F = atoi(token2.c_str()) - 1;
							if(cpt2 == 1) // si on est sur le deuxieme X
								indice2T = atoi(token2.c_str()) - 1;
						}
						token.erase(0, pos2 + delimiter2.length());
						cpt2++;
					}

				}
				str.erase(0, pos + delimiter.length());
				cpt += 1;
			}
			// reste le 3eme groupe de X/X/X
			pos2 = str.find(delimiter2);
			token2 = str.substr(0, pos2);
			indice3F = atoi(token2.c_str()) - 1; // on recupere l'indice du point

			str.erase(0, pos2 + delimiter2.length());
			pos2 = str.find(delimiter2);
			token2 = str.substr(0, pos2);
			indice3T = atoi(token2.c_str()) - 1; // on recupere l'indice de la texture

			lesTemps[0] = lesPointsReels.at(indice1F);
			lesTemps[1] = lesPointsReels.at(indice2F);
			lesTemps[2] = lesPointsReels.at(indice3F);

			tempNormale1 = {lesTemps[2].x - lesTemps[0].x, lesTemps[2].y - lesTemps[0].y, lesTemps[2].z -lesTemps[0].z};
			tempNormale2 = {lesTemps[1].x - lesTemps[0].x, lesTemps[1].y - lesTemps[0].y, lesTemps[1].z - lesTemps[0].z};

			// On calcul le vecteur avant de le normaliser
			normale = {(tempNormale1.y * tempNormale2.z) - (tempNormale1.z * tempNormale2.y),
								(tempNormale1.z * tempNormale2.x) - (tempNormale1.x * tempNormale2.z),
								(tempNormale1.x * tempNormale2.y) - (tempNormale1.y * tempNormale2.x)};

			normalize(normale);

			luminosite = (normale.x * directionLumiere.x) + (normale.y * directionLumiere.y) + (normale.z * directionLumiere.z);
			//cout << indice1T << " " << indice2T << " " << indice3T << endl;
			if (luminosite > 0) {
				remplir_triangle(zbuffer, lesPointsAff.at(indice1F), lesPointsAff.at(indice2F), lesPointsAff.at(indice3F), lesTextures.at(indice1T), lesTextures.at(indice2T), lesTextures.at(indice3T), image, luminosite, texture);
			}

			// Pour tracer les aretes des triangles
			//tracer_triangle(lesPoints.at(indice1), lesPoints.at(indice2), lesPoints.at(indice3), image, white);

		}
	}

}

int main(int argc, char** argv) {
	TGAImage image(COTE, COTE, TGAImage::RGB);
	TGAImage texture(1024, 1024, TGAImage::RGB);

	texture.read_tga_file("african_head_diffuse.tga");
	texture.flip_vertically();

	openfile("african_head.obj", image, texture);

	image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
	image.write_tga_file("output.tga");
	return 0;
}

