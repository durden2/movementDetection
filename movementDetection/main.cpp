#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string>
#include <iostream>
#include <fstream>
#include "TPGM.h"
#include "histogram.h"
#include "histogram.cpp"
#include "Image.cpp"

using namespace std;

int frameSize = 10;
int imagesCount = 5;


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
			t: if (y >= width) break;
			int firstImageFrameValue = first[x][y];

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
	

	std::string infname_ppm = "..\\pic\\1.ppm";

	Image i1 = Image(infname_ppm);

	int h_ppm = i1.h;
	int w_ppm = i1.w;

	//init frames
	unsigned char **moveFrames = new unsigned char *[h_ppm];
	moveFrames[0] = new unsigned char[h_ppm * w_ppm];
	for (int i = 1; i < h_ppm; i++)
		moveFrames[i] = moveFrames[i - 1] + w_ppm;

	initMovementFrames(h_ppm, w_ppm, moveFrames);

	i1.drawFrames(moveFrames);

	i1.writeImage(infname_ppm);

	//END INIT

	for (int currentImage = 1; currentImage < imagesCount; currentImage++) {
		///////////////////////////////PIERWSZY OBRAZEK
		int max_color_ppm;
		int hpos_ppm, i_ppm, j_ppm;

		std::string infname_ppm = "..\\pic\\" + std::to_string(currentImage) + ".ppm";

		Image img1 = Image(infname_ppm);

		//init frames
		unsigned char **moveFrames = new unsigned char *[h_ppm];
		moveFrames[0] = new unsigned char[h_ppm * w_ppm];
		for (int i = 1; i < h_ppm; i++)
			moveFrames[i] = moveFrames[i - 1] + w_ppm;

		initMovementFrames(h_ppm, w_ppm, moveFrames);

		unsigned int **factorFramesFirstImage = new unsigned int *[h_ppm];
		factorFramesFirstImage[0] = new unsigned int[h_ppm * w_ppm];
		for (int i = 1; i < h_ppm; i++)
			factorFramesFirstImage[i] = factorFramesFirstImage[i - 1] + w_ppm;

		//measureFramesFactor(h_ppm, w_ppm, R_ppm, G_ppm, B_ppm, factorFramesFirstImage);


		////////////////////////////////DRUGI OBRAZEK
		int max_color_ppm_next;
		int hpos_ppm_next, i_ppm_next, j_ppm_next;

		std::string infname_ppm_next = "..\\pic\\" + std::to_string(currentImage + 1) + ".ppm";

		Image img2 = Image(infname_ppm_next);

		unsigned int **factorFramesSecondImage = new unsigned int *[h_ppm];
		factorFramesSecondImage[0] = new unsigned int[h_ppm * w_ppm];
		for (int i = 1; i < h_ppm; i++)
			factorFramesSecondImage[i] = factorFramesSecondImage[i - 1] + w_ppm;

		//measureFramesFactor(h_ppm, w_ppm, R_ppm_next, G_ppm_next, B_ppm_next, factorFramesSecondImage);

		//compareMeasurements(h_ppm, w_ppm, factorFramesFirstImage, factorFramesSecondImage, moveFrames);


		//INIT HISTOGRAMS

		img2.drawFrames(moveFrames);
		cout << img2.compareImages(img1) << endl;
		img2.writeImage(infname_ppm_next);

	}

	cout << "koniec programu";
	getchar();

	return 0;
}