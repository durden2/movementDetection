#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string>
#include <iostream>
#include <fstream>
#include "TPGM.h"

using namespace std;

int frameSize = 10;
int imagesCount = 5;

bool replace(std::string &str, const std::string &from, const std::string &to) {
	size_t start_pos = str.find(from);
	if (start_pos == std::string::npos)
		return false;
	str.replace(start_pos, from.length(), to);
	return true;
}


void initMovementFrames(int width, int height, unsigned char **moveFrames) {
	int t = 0;

	for (int x = 0; x < width - 1; x++) {
		for (int y = 0; y < height - 1; y++) {
			if (x % frameSize == 0 || y % frameSize == 0) {
				moveFrames[x][y] = 1;
			}
			else {
				moveFrames[x][y] = 0;
			}
		}
	}
}

void drawFrames(int width, int height, unsigned char **R, unsigned char **G, unsigned char **B, unsigned char **frames) {
	for (int x = 0; x < width - 10; x++) {
		for (int y = 0; y < height - 10; y++) {
			if (frames[x][y] != 0) {
				for (int xx = x; xx < x + 10; xx++) {
					for (int yy = y; yy < y + 10; yy++) {
						if (xx % 10 == 0 || yy % 10 == 0) {
							R[xx][yy] = 0;
							G[xx][yy] = 0;
							B[xx][yy] = 255;
						}
					}
				}
			}
		}
	}
}

void measureFramesFactor(int height, int width, unsigned char **R, unsigned char **G, unsigned char **B, unsigned int **frames) {
	int suma = 0;
	unsigned int suma_next = 0;
	// unsigned int *tab_suma = new unsigned int[(height - 10) * (width - 10) * 100];
	// unsigned int *tab_suma_next = new unsigned int[(height - 10) * (width - 10) * 100];
	int counter = 0;

	for (int x = 0; x < height - frameSize; x++) {
		for (int y = 0; y < width - frameSize; y++) {
			for (int i = x; i < x + frameSize; i++) {
				for (int k = y; k < y + frameSize; k++) {
					suma += R[i][k];
					suma += G[i][k];
					suma += B[i][k];
				}
			}

			frames[x][y] = suma;
		}
	}

}

void compareMeasurements(int height, int width, unsigned int **first, unsigned int **second, unsigned char **moveFrames) {
	for (int x = 0; x < height - 1; x++) {
		for (int y = 0; y < width - 1; y++) {
			moveFrames[x][y] = 0;
		}
	}

	for (int x = 0; x < height; x++) {
		for (int y = 0; y < width; y++) {
			if (y >= width) break;
			t: int firstImageFrameValue = first[x][y];

			int difference = 9999999;

			int nextFrameXPosition = 0;
			int nextFrameYPosition = 0;

			for (int xx = 0; xx < height; xx++) {
				for (int yy = 0; yy < width; yy++) {
					long okok = second[xx][yy] - firstImageFrameValue;
					int temp = abs(okok);

					if (temp > difference) {
						nextFrameXPosition = xx;
						nextFrameYPosition = yy;
					}

					if (temp > 20000) {
						moveFrames[nextFrameXPosition][nextFrameYPosition] = 1;
						y++;
						goto t;
					}
				}
			}

			moveFrames[nextFrameXPosition][nextFrameYPosition] = 1;
		}
	}
}


int main(int argc, char **argv) {

	//INIT


	int h_ppm, w_ppm; // height, width rows / cols
	int max_color_ppm;
	int hpos_ppm, i_ppm, j_ppm;

	std::string infname_ppm = "..\\pic\\1.ppm";

	if ((hpos_ppm = readPPMB_header(infname_ppm.c_str(), &h_ppm, &w_ppm, &max_color_ppm)) <= 0) exit(1);

	//Tablica dla skladowych R obrazka
	unsigned char **R_ppm = new unsigned char *[h_ppm];
	R_ppm[0] = new unsigned char[h_ppm * w_ppm];

	for (int i = 1; i < h_ppm; i++)
		R_ppm[i] = R_ppm[i - 1] + w_ppm;

	//Tablica dla skladowych G obrazka
	unsigned char **G_ppm = new unsigned char *[h_ppm];
	G_ppm[0] = new unsigned char[h_ppm * w_ppm];
	for (int i = 1; i < h_ppm; i++)
		G_ppm[i] = G_ppm[i - 1] + w_ppm;

	//Tablica dla skladowych B obrazka
	unsigned char **B_ppm = new unsigned char *[h_ppm];
	B_ppm[0] = new unsigned char[h_ppm * w_ppm];
	for (int i = 1; i < h_ppm; i++)
		B_ppm[i] = B_ppm[i - 1] + w_ppm;

	//init frames
	unsigned char **moveFrames = new unsigned char *[h_ppm];
	moveFrames[0] = new unsigned char[h_ppm * w_ppm];
	for (int i = 1; i < h_ppm; i++)
		moveFrames[i] = moveFrames[i - 1] + w_ppm;

	initMovementFrames(h_ppm, w_ppm, moveFrames);

	if (readPPMB_data(R_ppm[0], G_ppm[0], B_ppm[0], infname_ppm.c_str(), hpos_ppm, h_ppm, w_ppm, max_color_ppm) == 0) exit(1);

	drawFrames(h_ppm, w_ppm, R_ppm, G_ppm, B_ppm, moveFrames);

	std::string outfname_ppm = infname_ppm;
	replace(outfname_ppm, ".ppm", "_simple_sample.ppm");

	if (writePPMB_image(outfname_ppm.c_str(), R_ppm[0], G_ppm[0], B_ppm[0], h_ppm, w_ppm, max_color_ppm) == 0) exit(1);

	//END INIT

	for (int currentImage = 1; currentImage < imagesCount; currentImage++) {
		///////////////////////////////PIERWSZY OBRAZEK
		int max_color_ppm;
		int hpos_ppm, i_ppm, j_ppm;

		std::string infname_ppm = "..\\pic\\" + std::to_string(currentImage) + ".ppm";

		cout << infname_ppm << endl;

		if ((hpos_ppm = readPPMB_header(infname_ppm.c_str(), &h_ppm, &w_ppm, &max_color_ppm)) <= 0) exit(1);

		//Tablica dla skladowych R obrazka
		unsigned char **R_ppm = new unsigned char *[h_ppm];
		R_ppm[0] = new unsigned char[h_ppm * w_ppm];

		for (int i = 1; i < h_ppm; i++)
			R_ppm[i] = R_ppm[i - 1] + w_ppm;

		//Tablica dla skladowych G obrazka
		unsigned char **G_ppm = new unsigned char *[h_ppm];
		G_ppm[0] = new unsigned char[h_ppm * w_ppm];
		for (int i = 1; i < h_ppm; i++)
			G_ppm[i] = G_ppm[i - 1] + w_ppm;

		//Tablica dla skladowych B obrazka
		unsigned char **B_ppm = new unsigned char *[h_ppm];
		B_ppm[0] = new unsigned char[h_ppm * w_ppm];
		for (int i = 1; i < h_ppm; i++)
			B_ppm[i] = B_ppm[i - 1] + w_ppm;

		//init frames
		unsigned char **moveFrames = new unsigned char *[h_ppm];
		moveFrames[0] = new unsigned char[h_ppm * w_ppm];
		for (int i = 1; i < h_ppm; i++)
			moveFrames[i] = moveFrames[i - 1] + w_ppm;

		initMovementFrames(h_ppm, w_ppm, moveFrames);

		if (readPPMB_data(R_ppm[0], G_ppm[0], B_ppm[0], infname_ppm.c_str(), hpos_ppm, h_ppm, w_ppm, max_color_ppm) == 0) exit(1);

		unsigned int **factorFramesFirstImage = new unsigned int *[h_ppm];
		factorFramesFirstImage[0] = new unsigned int[h_ppm * w_ppm];
		for (int i = 1; i < h_ppm; i++)
			factorFramesFirstImage[i] = factorFramesFirstImage[i - 1] + w_ppm;

		measureFramesFactor(h_ppm, w_ppm, R_ppm, G_ppm, B_ppm, factorFramesFirstImage);

		////////////////////////////////DRUGI OBRAZEK
		int max_color_ppm_next;
		int hpos_ppm_next, i_ppm_next, j_ppm_next;

		std::string infname_ppm_next = "..\\pic\\" + std::to_string(currentImage + 1) + ".ppm";

		cout << infname_ppm_next;

		if ((hpos_ppm_next = readPPMB_header(infname_ppm_next.c_str(), &h_ppm, &w_ppm, &max_color_ppm_next)) <= 0) exit(1);

		unsigned char **R_ppm_next = new unsigned char *[h_ppm];
		R_ppm_next[0] = new unsigned char[h_ppm * h_ppm];
		for (int i = 1; i < h_ppm; i++)
			R_ppm_next[i] = R_ppm_next[i - 1] + h_ppm;

		unsigned char **G_ppm_next = new unsigned char *[h_ppm];
		G_ppm_next[0] = new unsigned char[h_ppm * h_ppm];
		for (int i = 1; i < h_ppm; i++)
			G_ppm_next[i] = G_ppm_next[i - 1] + h_ppm;

		unsigned char **B_ppm_next = new unsigned char *[h_ppm];
		B_ppm_next[0] = new unsigned char[h_ppm * h_ppm];
		for (int i = 1; i < h_ppm; i++)
			B_ppm_next[i] = B_ppm_next[i - 1] + h_ppm;

		if (readPPMB_data(R_ppm_next[0], G_ppm_next[0], B_ppm_next[0], infname_ppm_next.c_str(), hpos_ppm_next, h_ppm, h_ppm, max_color_ppm_next) == 0) exit(1);

		unsigned int **factorFramesSecondImage = new unsigned int *[h_ppm];
		factorFramesSecondImage[0] = new unsigned int[h_ppm * w_ppm];
		for (int i = 1; i < h_ppm; i++)
			factorFramesSecondImage[i] = factorFramesSecondImage[i - 1] + w_ppm;

		measureFramesFactor(h_ppm, w_ppm, R_ppm_next, G_ppm_next, B_ppm_next, factorFramesSecondImage);
		
		for (int x = 1; x < 20 - 1; x++) {
			for (int y = 1; y < 20 - 1; y++) {
				cout << factorFramesFirstImage[x][y] - factorFramesSecondImage[x][y] << endl;
			}
		}
		compareMeasurements(h_ppm, w_ppm, factorFramesFirstImage, factorFramesSecondImage, moveFrames);

		drawFrames(h_ppm, w_ppm, R_ppm, G_ppm, B_ppm, moveFrames);

		std::string outfname_ppm = infname_ppm;
		replace(outfname_ppm, ".ppm", "_simple.ppm");

		if (writePPMB_image(outfname_ppm.c_str(), R_ppm[0], G_ppm[0], B_ppm[0], h_ppm, w_ppm, max_color_ppm) == 0) exit(1);

		delete[] R_ppm[0];
		delete[] R_ppm;

		delete[] G_ppm[0];
		delete[] G_ppm;

		delete[] B_ppm[0];
		delete[] B_ppm;


		delete[] R_ppm_next[0];
		delete[] R_ppm_next;

		delete[] G_ppm_next[0];
		delete[] G_ppm_next;

		delete[] B_ppm_next[0];
		delete[] B_ppm_next;

	}


	/*
	std::fstream plik;
	plik.open("roznica.txt", std::ios::in | std::ios::out);
	if (plik.good() == true)
	{
		for (int i = 0; i < (h_ppm - 10)*(w_ppm - 10) * 100; i++) {
			plik << tab_suma[i] << " ; " << tab_suma_next[i] << " ; ";
			if (tab_suma[i] > tab_suma_next[i]) {
				plik << tab_suma[i] - tab_suma_next[i] << endl;
			}
			else {
				plik << "-" << tab_suma_next[i] - tab_suma[i] << endl;
			}
		}


		plik.close();
	}

	*/

	cout << "koniec programu";
	getchar();

	return 0;
}