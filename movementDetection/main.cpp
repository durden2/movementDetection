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
#include <vector>

using namespace std;

int frameSize = 4;
int imagesCount = 5;

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

double singleCorrelation(int *input, int *input2) {
	double sum = 0;

	for (int i = 0; i < 255; i++) {
		double t = pow(input[i] - input2[i], 2);
		double t2 = input[i] + input2[i];

		if (t2 == 0) continue;

		sum += (t / t2);
	}

	return sum;
}

double compareHistograms(histogram *hist, histogram *hToCompare) {
	double corrR = singleCorrelation(hist->R, hToCompare->R);
	double corrG = singleCorrelation(hist->G, hToCompare->G);
	double corrB = singleCorrelation(hist->B, hToCompare->B);

	double corrValue = (corrR + corrB + corrG) / 3;

	return corrValue;
};

void calculateMoveHistograms(Image *img1, Image *img2) {
	vector<histogram> mainHist;
	vector<histogram> secondHistogram;

	int w = img1->w;
	int h = img1->h;

	histogram tempHistogram;
	histogram tempHistogram2;
	for (int i = 0; i < h; i++) {
		for (int j = 0; j < w; j++) {

			tempHistogram.setHistogramValues(img1->R, img1->G, img1->B, frameSize, frameSize, i, j);
			mainHist.push_back(tempHistogram);

			tempHistogram2.setHistogramValues(img2->R, img2->G, img2->B, frameSize, frameSize, i, j);
			secondHistogram.push_back(tempHistogram2);
		}
	}


	unsigned char **moveFrames = new unsigned char *[h];
	moveFrames[0] = new unsigned char[h * w];
	for (int i = 1; i < h; i++)
		moveFrames[i] = moveFrames[i - 1] + w;

	int t = 0;

	for (int x = 0; x < h; x++) {
		for (int y = 0; y < w; y++) {
			moveFrames[x][y] = 0;
		}
	}

	for (int i = 0; i < img1->h; i ++) {
		cout << i << endl;
		for (int j = 0; j < img1->w; j ++) {
			if (img1->moveFrames[i][j] == 1) {
				double maxFactor = 0.0;
				int newX = 0;
				int newY = 0;
				int distance = 10000;
				int scope = 50;

				for (int ii = i - scope; ii < i + scope; ii++) {
					for (int jj = j - scope; jj < j + scope; jj++) {
						if (ii > 0 && jj > 0 && ii < img1->h && jj < img1->w) {

							double factor = compareHistograms(&secondHistogram.at(ii + jj), &mainHist.at(ii + jj));

							if (factor == maxFactor) {
								int tempDistance = sqrt(pow(ii - i, 2) + pow(jj - j, 2));
								if (tempDistance < distance) {
									maxFactor = factor;
									newX = ii;
									newY = jj;
								}
							}
							else if (factor > maxFactor) {
								maxFactor = factor;
								newX = ii;
								newY = jj;
							}

						}
					}
				}

				moveFrames[newX][newY] = 1;
			}

		}
	}

	img2->moveFrames = moveFrames;
}

void calculateMoveSum(Image *img1, Image *img2) {
	vector<double> mainHist;
	vector<double> secondHistogram;

	int w = img1->w;
	int h = img1->h;

	unsigned char **moveFrames = new unsigned char *[h];
	moveFrames[0] = new unsigned char[h * w];
	for (int i = 1; i < h; i++)
		moveFrames[i] = moveFrames[i - 1] + w;


	for (int i = 0; i < h; i++) {
		for (int j = 0; j < w; j++) {
			if (img1->moveFrames[i][j] == 1) {
				
				double factorMax = 0.0;
				int newX = 0;
				int newY = 0;

				for (int ii = i - 200; ii < i + 200; ii ++) {
					for (int jj = j - 200; jj < j + 200; jj ++) {
						double difference = 0.0;

						int pCount = 0;
						if (ii > 0 && jj > 0 && ii < img1->h && jj < img1->w) {
							for (int y = i; y < i + frameSize; y++) {
								for (int x = j; x < i + frameSize ; x++) {
									if (x > 0 && y > 0 && x < img1->h && y < img1->w) {
										pCount++;
										unsigned char redA = img1->R[x][y];
										unsigned char greenA = img1->G[x][y];
										unsigned char blueA = img1->B[x][y];
										unsigned char redB = img2->R[x][y];
										unsigned char greenB = img2->G[x][y];
										unsigned char blueB = img2->B[x][y];
										difference += abs(redA - redB);
										difference += abs(greenA - greenB);
										difference += abs(blueA - blueB);
									}
									
								}
							}

							double total_pixels = pCount * 3;
							double avg_different_pixels = difference /
								total_pixels;

							// There are 255 values of pixels in total
							double percentage = (avg_different_pixels /
								255) * 100;

							if (percentage > factorMax) {
								factorMax = percentage;
								newX = ii;
								newY = jj;
							}
						}
					}
				}

				moveFrames[newX][newY] = 1;
			}
			j += frameSize;
		}
		i += frameSize;
	}
	img2->moveFrames = moveFrames;
}

int main(int argc, char **argv) {

	//INIT
	std::string infname_ppm = "..\\pic\\1.ppm";

	Image i1(infname_ppm);

	i1.initMovementFrames();
	i1.drawFrames();
	i1.writeImage(infname_ppm);

	unsigned char **moveFrames = new unsigned char *[i1.h];
	moveFrames[0] = new unsigned char[i1.h * i1.w];
	for (int i = 1; i < i1.h; i++)
		moveFrames[i] = moveFrames[i - 1] + i1.w;

	moveFrames = i1.moveFrames;
	//END INIT

	for (int currentImage = 1; currentImage < 15; currentImage++) {
		///////////////////////////////PIERWSZY OBRAZEK

		std::string infname_ppm = "..\\pic\\" + std::to_string(currentImage) + ".ppm";

		Image img1(infname_ppm);
		img1.moveFrames = moveFrames;

		////////////////////////////////DRUGI OBRAZEK
		std::string infname_ppm_next = "..\\pic\\" + std::to_string(currentImage + 1) + ".ppm";

		Image img2(infname_ppm_next);

		calculateMoveHistograms(&img1, &img2);

		cout << "DONE";

		img2.drawFrames();
		img2.writeImage(infname_ppm_next);

		for (int i = 1; i < i1.h; i++)
			for (int ii = 1; ii < i1.h; ii++)
			moveFrames[i][ii] = img2.moveFrames[i][ii];
	}

	cout << "koniec programu";
	getchar();

	return 0;
}