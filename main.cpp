#include "Triangle.h"
#include "Line.h"
#include "Point.h"
#include "IndiceTriangle.h"

#include "tgaimage.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <regex>

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);

const float COTE = 500.0;

using namespace std;

// Ouvre le fichier et range la liste des points
TGAImage openfile(string filename, TGAImage &image) {
	const char * name = filename.c_str();

	bool firstTime = true;

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
	vector<IndiceTriangle> lesIndices;
	int cpt;
	int xTemp, yTemp, zTemp;
	int indice1, indice2, indice3;

	string token, token2;
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
					//cout << token.c_str();
					xTemp = (atof(token.c_str()) + 1.) * COTE / 2.;
				}

				// si on est sur le y
				if (cpt == 2) {
					yTemp = (atof(token.c_str()) + 1.) * COTE / 2.;
					//	cout << " " << token.c_str() << endl;
				}

				// si on est sur le z
				if (cpt == 3) {
					zTemp = (atof(token.c_str()) + 1.) * COTE / 2.;
					//	cout << " " << token.c_str() << endl;
				}
				str.erase(0, pos + delimiter.length());
				cpt += 1;
			}

			//cout << xTemp << " " << yTemp << endl;

			Point p(xTemp, yTemp, zTemp);

			//cout << p.getX() << " " << p.getY() << endl;
			image = p.tracer(image, white);
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

			IndiceTriangle iT(indice1, indice2, indice3);
			lesIndices.push_back(iT);

			Triangle t(lesPoints.at(indice1), lesPoints.at(indice2),
					lesPoints.at(indice3));

			image = t.tracer(image, white);
			if (firstTime) {
				//image = t.tracer(image, white);
				firstTime = false;
				cout << indice1 << " " << indice2 << " " << indice3 << endl;
				cout << lesPoints.at(indice1).getX() << " "
						<< lesPoints.at(indice1).getY() << " "
						<< lesPoints.at(indice2).getX() << " "
						<< lesPoints.at(indice2).getY() << " "
						<< lesPoints.at(indice3).getX() << " "
						<< lesPoints.at(indice2).getY() << endl;
			}
		}
	}

	return image;

}

// Fonction alternative (moins bien)
/*TGAImage openfile(string filename, TGAImage &image) {
 vector<Point> lesPoints;
 vector<Triangle> lesTriangles;

 int cpt, cptDeux;
 int xTemp, yTemp, zTemp;
 int premier, deuxieme, troisieme;
 string paquetUn, paquetDeux, paquetTrois;
 char * pch;
 char * pchDeux;

 const char * name = filename.c_str();
 ifstream in(name); // on essaye d'ouvrir le fichier

 // si on ne peut pas ouvrir le fichier
 if (!in)
 cout << "Cannot open input file.\n";

 string str, strDeux;
 char * tabStr;
 char * tabStrDeux;

 bool trouve;
 string temp;

 regex pattern { "^/" };

 // Tant qu'on n'a pas atteint la fin du fichier
 while (!in.eof()) {
 getline(in, str);

 tabStr = new char[str.length() + 1];
 strcpy(tabStr, str.c_str());

 // Si on est sur une ligne v
 if (str[0] == 'v' && str[1] == ' ') {

 // On decoupe la ligne avec les espaces
 pch = strtok(tabStr, " ");
 cpt = 0;
 while (pch != NULL) {
 if (cpt == 1)
 xTemp = (atof(pch) + 1.) * COTE / 2.;
 if (cpt == 2)
 yTemp = (atof(pch) + 1.) * COTE / 2.;
 if (cpt == 3)
 zTemp = (atof(pch) + 1.) * COTE / 2.; // pas sûr de la transformation a faire dessus

 pch = strtok(NULL, " ");
 cpt++;
 }

 // Quand on arrive ici, on a termine de lire la ligne
 Point p(xTemp, yTemp, zTemp);
 lesPoints.push_back(p);
 }

 // si on est sur une ligne f
 if (str[0] == 'f') {
 pch = strtok(tabStr, " ");
 cpt = 0;

 // On decoupe la ligne avec les espaces
 while (pch != NULL) {
 char * taMere = new char[strlen(pch) + 1];
 if (cpt == 1) {
 paquetUn = pch;
 }
 if (cpt == 2) {
 paquetDeux = pch;
 }
 if (cpt == 3) {
 paquetTrois = pch;
 }

 pch = strtok(NULL, " ");
 cpt++;
 }

 cout << paquetUn << " " << paquetDeux << " " << paquetTrois << endl;

 for (int i = 0; i < 3; i++) { // pour s'occuper des trois paquets
 trouve = false;
 cpt = 0;
 temp = "";
 if (i == 0) {
 while (trouve == false && cpt < paquetUn.size()) {
 if (regex_search(&paquetUn[cpt], pattern)) {
 trouve = true;
 } else {
 temp += paquetUn[cpt];
 }
 cpt++;
 }
 premier = atoi(temp.c_str()) -1;
 }
 if (i == 1) {
 while (trouve == false && cpt < paquetDeux.size()) {
 if (regex_search(&paquetDeux[cpt], pattern)) {
 trouve = true;
 } else {
 temp += paquetDeux[cpt];
 }
 cpt++;
 }
 deuxieme = atoi(temp.c_str()) -1;
 }
 if (i == 2) {
 while (trouve == false && cpt < paquetTrois.size()) {
 if (regex_search(&paquetTrois[cpt], pattern)) {
 trouve = true;
 } else {
 temp += paquetTrois[cpt];
 }
 cpt++;
 }
 troisieme = atoi(temp.c_str()) -1;
 }
 }

 cout << premier << " " << deuxieme << " " << troisieme << endl;

 Triangle t(lesPoints[premier], lesPoints[deuxieme], lesPoints[troisieme]);
 lesTriangles.push_back(t);
 }
 }

 // Quand on arrive ici, on a termine de lire le fichier
 //	Point unPoint;
 //	for (int i = 0; i < lesPoints.size(); i++) {
 //		unPoint = lesPoints[i];
 //		image = unPoint.tracer(image, white);
 //		//	cout << unPoint.getX() << " " << unPoint.getY() << endl;
 //	}

 Triangle unTriangle;
 for (int i = 0; i < lesTriangles.size(); i++) {
 unTriangle = lesTriangles[i];
 image = unTriangle.tracer(image, white);
 }

 return image;
 }*/

int main(int argc, char** argv) {
	TGAImage image(COTE, COTE, TGAImage::RGB);
//	image.set(52, 41, red);

//	Line ligneTest(52, 41, 70, 35);
//	image = ligneTest.tracer(image, red);
//
//	Line ligneTest2(52, 41, 35, 70);
//	image = ligneTest2.tracer(image, white);

	image = openfile("african_head.obj", image);

	image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
	image.write_tga_file("output.tga");
	return 0;
}

