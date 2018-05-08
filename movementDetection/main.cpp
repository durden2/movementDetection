#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string>

#include "TPGM.h"


bool replace(std::string &str, const std::string &from, const std::string &to) {
	size_t start_pos = str.find(from);
	if (start_pos == std::string::npos)
		return false;
	str.replace(start_pos, from.length(), to);
	return true;
}


int main(int argc, char **argv) {
	int h, w; // height, width
	int max_color;
	int hpos, i, j;

	std::string infname = "maly.pgm";

	if ((hpos = readPGMB_header(infname.c_str(), &h, &w, &max_color)) <= 0) exit(1);

	unsigned char **a = new unsigned char *[h];
	a[0] = new unsigned char[h * w];
	for (int i = 1; i < h; i++)
		a[i] = a[i - 1] + w;

	if (readPGMB_data(a[0], infname.c_str(), hpos, h, w, max_color) == 0) exit(1);

	//przygotowanie czarno-bialej tablicy wyjsciowej

	unsigned char **b = new unsigned char *[h];
	b[0] = new unsigned char[h * w];
	for (int i = 1; i < h; i++)
		b[i] = b[i - 1] + w;

	
	memset(b[0], 255, w * h);	

	std::string outfname = infname;
	replace(outfname, ".pgm", "_simple.pgm");

	if (writePGMB_image(outfname.c_str(), b[0], h, w, 255) == 0) exit(1);

	delete[] a[0];
	delete[] a;
	delete[] b[0];
	delete[] b;

	getchar();

	return 0;
}