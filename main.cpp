#include "Triangle.h"
#include "Line.h"
#include "Point.h"
#include "tgaimage.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <regex>

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);

const float LARGEUR = 100.0;
const float HAUTEUR = 100.0;

using namespace std;

// Ouvre le fichier et range la liste des points
TGAImage openfile(string filename, TGAImage image) {
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

	vector<Point> lesPoints;
	int cpt;
	int xTemp, yTemp;
	int indice1, indice2, indice3;
	Point p1, p2, p3;

	string token, token2;
	string::size_type sz;
	size_t pos, pos2;

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
					xTemp = (atof(token.c_str()) + 1.) * LARGEUR / 2.;
					//xTemp = ((atof(token.c_str)) + 1.) * LARGEUR / 2.;
				}

				// si on est sur le y
				if (cpt == 2) {
					yTemp = (atof(token.c_str()) + 1.) * LARGEUR / 2.;
				}
				str.erase(0, pos + delimiter.length());
				cpt += 1;
			}

			//cout << xTemp << " " << yTemp << endl;

			Point p(xTemp, yTemp);
			lesPoints.push_back(p);

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
						indice1 = stoi(token2, &sz);
					}

					// si on est sur le 2eme groupe de X/X/X
					if (cpt == 2) {
						indice2 = stoi(token2, &sz);
					}

				}
				str.erase(0, pos + delimiter.length());
				cpt += 1;
			}
			// reste le 3eme groupe de X/X/X
			pos2 = str.find(delimiter2);
			token2 = str.substr(0, pos2);
			indice3 = stoi(str, &sz);

			if (indice1 == 1258) {
				indice1 = indice2;
			}
			if (indice2 == 1258) {
				indice2 = indice3;
			}
			if (indice3 == 1258) {
				indice3 = indice1;
			}

			cout << indice1 << " " << indice2 << " " << indice3 << endl;
			Triangle t(lesPoints.at(indice1), lesPoints.at(indice2),
					lesPoints.at(indice3));

			image = t.tracer(image, white);

		}
	}
	return image;

}

int main(int argc, char** argv) {
	TGAImage image(LARGEUR, HAUTEUR, TGAImage::RGB);
//	image.set(52, 41, red);
	image.flip_vertically(); // i want to have the origin at the left bottom corner of the image

//	Line ligneTest(52, 41, 70, 35);
//	image = ligneTest.tracer(image, red);
//
//	Line ligneTest2(52, 41, 35, 70);
//	image = ligneTest2.tracer(image, white);

	image = openfile("africian_head.obj", image);

	image.write_tga_file("output.tga");
	return 0;
}

