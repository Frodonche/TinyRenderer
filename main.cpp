#include "Line.h"
#include "tgaimage.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <string.h>
#include <regex>

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0,   0,   255);

using namespace std;


// Ouvre le fichier et range des infos sur les lignes dans des tableaux
void openfile(string filename){
	const char * name = filename.c_str();
	cout << "string : "; cout << filename; cout << "\n";
	cout << "char * : "; cout << name; cout << "\n";

	ifstream in(name); // on essaye d'ouvrir le fichier

	// si on ne peut pas ouvrir le fichier
	if(!in)
		cout << "Cannot open input file.\n";

	regex patternV { "^v " };
	regex patternF { "^f " };

	string str;
	bool matchV, matchF;

	while (getline(in, str)) {
	    // A chaque ligne, on regarde de quel type il s'agit, et on les range dans les tableaux appropries
		matchV = regex_search(str, patternV);
		matchF = regex_search(str, patternF);

		//if(matchF == true)
			//cout << str << endl;
	}

}



int main(int argc, char** argv) {
	TGAImage image(100, 100, TGAImage::RGB);
//	image.set(52, 41, red);
	image.flip_vertically(); // i want to have the origin at the left bottom corner of the image

//	Line ligneTest(52, 41, 70, 35);
//	image = ligneTest.tracer(image, red);
//
//	Line ligneTest2(52, 41, 35, 70);
//	image = ligneTest2.tracer(image, white);


	openfile("africian_head.obj");

	image.write_tga_file("output.tga");
	return 0;
}


