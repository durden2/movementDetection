#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string>
#include <iostream>
#include <fstream>
#include "TPGM.h"
#include "ppmStructure.h"
#include "labDistance.h"

using namespace std;

int frameSize = 10;
int imagesCount = 276;

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

unsigned char** calcLab(unsigned char **R, unsigned char **G, unsigned char **B, unsigned char **R2, unsigned char **G2, unsigned char **B2, int height, int width) {
	unsigned char **newPositions = new unsigned char *[height];
	newPositions[0] = new unsigned char[height * width];
	for (int i = 1; i < height; i++)
		newPositions[i] = newPositions[i - 1] + width;

	labDistance lab(R, G, B, R2, G2, B2, height, width);
	newPositions = lab.calculateNewPosisions();

	return newPositions;
}

void measureFramesFactor(int height, int width, unsigned char **R, unsigned char **G, unsigned char **B, unsigned int **frames) {
	unsigned int suma = 0;
	unsigned int suma_next = 0;
	int counter = 0;

	for (int x = 0; x < height - frameSize; x++) {
		for (int y = 0; y < width - frameSize; y++) {
			for (int i = x; i < x + frameSize; i++) {
				for (int k = y; k < y + frameSize; k++) {
					suma += R[i][k] * G[i][k] + B[i][k];

				}
			}
			frames[x][y] = suma;
			suma = 0;
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

			unsigned int firstImageFrameValue = first[x][y];
			unsigned int secondImageFrameValue = second[x][y];
			unsigned int firstSecondDiff = 0;
			if (firstImageFrameValue > secondImageFrameValue) {
				firstSecondDiff = firstImageFrameValue - secondImageFrameValue;
			}
			else {
				firstSecondDiff = secondImageFrameValue - firstImageFrameValue;
			}

			unsigned int diffValue = 500000;

			if (firstSecondDiff > diffValue) {

				for (int xx = x; xx < height; xx++) {
					for (int yy = y; yy < width; yy++) {

						if (firstImageFrameValue > second[xx][yy]) {
							firstSecondDiff = firstImageFrameValue - second[xx][yy];
						}
						else {
							firstSecondDiff = second[xx][yy] - firstImageFrameValue;
						}

						unsigned int compareValue = 2;

						if (firstSecondDiff < compareValue) {
							moveFrames[xx][yy] = 1;
						}

					}
				}



			}
		}
	}
}


int main(int argc, char **argv) {

	//INIT
	ppmStructure ppmStruct("..\\pic\\1.ppm");
	ppmStruct.readPPMBHead();
	ppmStruct.initRGBTable();

	//init frames
	unsigned char **moveFrames1 = new unsigned char *[ppmStruct.h_ppm];
	moveFrames1[0] = new unsigned char[ppmStruct.h_ppm * ppmStruct.w_ppm];
	for (int i = 1; i < ppmStruct.h_ppm; i++)
		moveFrames1[i] = moveFrames1[i - 1] + ppmStruct.w_ppm;

	initMovementFrames(ppmStruct.h_ppm, ppmStruct.w_ppm, moveFrames1);

	ppmStruct.readPPMBData();

	drawFrames(ppmStruct.h_ppm, ppmStruct.w_ppm, ppmStruct.R_ppm, ppmStruct.G_ppm, ppmStruct.B_ppm, moveFrames1);

	std::string outfname_ppm = ppmStruct.imageName;
	replace(outfname_ppm, ".ppm", "_simple_with_frames.ppm");

	ppmStruct.writePPMBImage(outfname_ppm);

	delete[] moveFrames1[0];
	delete[] moveFrames1;

	//END INIT
	cout << "END INIT - wcisnij enter" << endl;
	getchar();


	for (int currentImage = 1; currentImage < 100; currentImage++) {
		///////////////////////////////PIERWSZY OBRAZEK
		int nextImage = currentImage + 1;

		string infname_ppm = "..\\pic\\" + std::to_string(currentImage) + ".ppm";

		ppmStructure ppmStructFirst(infname_ppm);
		cout << "\nOdczyt: " << infname_ppm;
		ppmStructFirst.readPPMBHead();
		ppmStructFirst.initRGBTable();

		//init frames
		unsigned char **moveFrames = new unsigned char *[ppmStructFirst.h_ppm];
		moveFrames[0] = new unsigned char[ppmStructFirst.h_ppm * ppmStructFirst.w_ppm];
		for (int i = 1; i < ppmStructFirst.h_ppm; i++)
			moveFrames[i] = moveFrames[i - 1] + ppmStructFirst.w_ppm;

		initMovementFrames(ppmStructFirst.h_ppm, ppmStructFirst.w_ppm, moveFrames);

		ppmStructFirst.readPPMBData();

		unsigned int **factorFramesFirstImage = new unsigned int *[ppmStructFirst.h_ppm];
		factorFramesFirstImage[0] = new unsigned int[ppmStructFirst.h_ppm * ppmStructFirst.w_ppm];
		for (int i = 1; i < ppmStructFirst.h_ppm; i++)
			factorFramesFirstImage[i] = factorFramesFirstImage[i - 1] + ppmStructFirst.w_ppm;

		//measureFramesFactor(ppmStructFirst.h_ppm, ppmStructFirst.w_ppm, ppmStructFirst.R_ppm, ppmStructFirst.G_ppm, ppmStructFirst.B_ppm, factorFramesFirstImage);

		////////////////////////////////DRUGI OBRAZEK ///////////////////////////////////////////////////////////////////////////

		std::string infname_ppm_next = "..\\pic\\" + std::to_string(nextImage) + ".ppm";
		ppmStructure ppmStructSecond(infname_ppm_next);
		cout << "\nOdczyt: " << infname_ppm_next;

		ppmStructSecond.readPPMBHead();
		ppmStructSecond.initRGBTable();

		ppmStructSecond.readPPMBData();

		unsigned int **factorFramesSecondImage = new unsigned int *[ppmStructSecond.h_ppm];
		factorFramesSecondImage[0] = new unsigned int[ppmStructSecond.h_ppm * ppmStructSecond.w_ppm];
		for (int i = 1; i < ppmStructSecond.h_ppm; i++)
			factorFramesSecondImage[i] = factorFramesSecondImage[i - 1] + ppmStructSecond.w_ppm;
		cout << "\nStart measureFramesFactor\n";

		//measureFramesFactor(ppmStructSecond.h_ppm, ppmStructSecond.w_ppm, ppmStructSecond.R_ppm, ppmStructSecond.G_ppm, ppmStructSecond.B_ppm, factorFramesSecondImage);

		cout << "Start compareMeasurements \n";
		//compareMeasurements(ppmStructSecond.h_ppm, ppmStructSecond.w_ppm, factorFramesFirstImage, factorFramesSecondImage, moveFrames);
		cout << "Start drawFrames \n";

		moveFrames = calcLab(ppmStructFirst.R_ppm, ppmStructFirst.G_ppm, ppmStructFirst.B_ppm, ppmStructSecond.R_ppm, ppmStructSecond.G_ppm, ppmStructSecond.B_ppm, ppmStructFirst.h_ppm, ppmStructFirst.w_ppm);
		drawFrames(ppmStructSecond.h_ppm, ppmStructSecond.w_ppm, ppmStructSecond.R_ppm, ppmStructSecond.G_ppm, ppmStructSecond.B_ppm, moveFrames);

		std::string	outfname_ppm = std::to_string(nextImage) + ".ppm";

		replace(outfname_ppm, ".ppm", "_done.ppm");
		cout << "Start writePPMB_image \n";
		ppmStructSecond.writePPMBImage(outfname_ppm);

		delete[] moveFrames[0];
		delete[] moveFrames;

		delete[] factorFramesFirstImage[0];
		delete[] factorFramesFirstImage;

		delete[] factorFramesSecondImage[0];
		delete[] factorFramesSecondImage;

		cout << "Koniec petli \n";
		//getchar();
	}

	cout << "koniec programu";
	getchar();

	return 0;
}