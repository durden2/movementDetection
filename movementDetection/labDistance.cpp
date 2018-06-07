#include "labDistance.h"
#include <vector>

labDistance::labDistance(unsigned char **R, unsigned char **G, unsigned char **B, unsigned char **R2, unsigned char **G2, unsigned char **B2, int _height, int _width) {
	vector<vector<vector<float>>> firstMean;
	vector<vector<vector<float>>> secondMean;

	firstMean = calculateMeanValues(R, G, B, height, width);
	secondMean = calculateMeanValues(R2, G2, B2, height, width);
}

vector<float> labDistance::convertToLab(unsigned char R, unsigned char G, unsigned char B) {
	float r = (float) R / 255;
	float g = (float) G / 255;
	float b = (float) B / 255;
	float x, y, z;

	r = (r > 0.04045) ? pow((r + 0.055) / 1.055, 2.4) : r / 12.92;
	g = (g > 0.04045) ? pow((g + 0.055) / 1.055, 2.4) : g / 12.92;
	b = (b > 0.04045) ? pow((b + 0.055) / 1.055, 2.4) : b / 12.92;

	x = (r * 0.4124 + g * 0.3576 + b * 0.1805) / 0.95047;
	y = (r * 0.2126 + g * 0.7152 + b * 0.0722) / 1.00000;
	z = (r * 0.0193 + g * 0.1192 + b * 0.9505) / 1.08883;

	x = (x > 0.008856) ? pow(x, 0.33) : (7.787 * x) + (16 / 116);
	y = (y > 0.008856) ? pow(y, 0.33) : (7.787 * y) + (16 / 116);
	z = (z > 0.008856) ? pow(z, 0.33) : (7.787 * z) + (16 / 116);

	float a = (116 * y) - 16;
	float d = 500 * (x - y);
	float c = 200 * (y - z);

	vector<float> arr;
	arr.push_back(a);
	arr.push_back(d);
	arr.push_back(c);

	return arr;
}

//void labDistance::measureFactors(int height, int width, unsigned char **R, unsigned char **G, unsigned char **B, unsigned int **frames) {}

vector<vector<vector<float>>> labDistance::calculateMeanValues(unsigned char **R, unsigned char **G, unsigned char **B, int height, int width) {
	vector<vector<vector<float>>> meanFrames(width);

	for (int i = 0; i < width; i++) {
		vector<vector<float>> widthVec(height);

		for (int j = 0; j < height; j++) {
			int meanR = 0;
			int meanG = 0;
			int meanB = 0;

			for (int a = 0; a < 10; a++)
				for (int b = 0; b < 10; b++) {
					meanR += R[a][b];
					meanG += G[a][b];
					meanB += B[a][b];
				}

			meanR /= 100;
			meanG /= 100;
			meanB /= 100;


			vector<float> col = labDistance::convertToLab(meanR, meanG, meanB);

			widthVec.at(j) = col;
		}

		meanFrames.at(i) = widthVec;
	}

	return meanFrames;
}

unsigned char **labDistance::calculateNewPosisions() {
	//init frames
	unsigned char **newPositions = new unsigned char *[labDistance::height];
	newPositions[0] = new unsigned char[labDistance::height * labDistance::width];
	for (int i = 1; i < labDistance::height; i++)
		newPositions[i] = newPositions[i - 1] + labDistance::width;

	for (int i = 0; i < labDistance::width; i++)
		for (int j = 0; j < labDistance::height; j++) {
		
			vector <float> firstFrame = labDistance::firstMean[i][j];
			float myFactor = 0;
			int framrX, frameY;

			for (int a = 0; a < labDistance::width; a++)
				for (int b = 0; b < labDistance::height; b++) {
					vector<float> secondFrame = labDistance::secondMean[a][b];

					float currentFactor = labDistance::calculateDelta(firstFrame, secondFrame);

					if (currentFactor < myFactor) {
						myFactor = currentFactor;
						frameY = b;
						framrX = a;
					}
				}

			newPositions[framrX][frameY] = 1;
		}

	return newPositions;
}

float labDistance::calculateDelta(vector<float> labA, vector<float> labB) {
	float deltaL = labA[0] - labB[0];
	float deltaA = labA[1] - labB[1];
	float deltaB = labA[2] - labB[2];
	float c1 = sqrt(labA[1] * labA[1] + labA[2] * labA[2]);
	float c2 = sqrt(labB[1] * labB[1] + labB[2] * labB[2]);
	float deltaC = c1 - c2;
	float deltaH = deltaA * deltaA + deltaB * deltaB - deltaC * deltaC;

	deltaH = deltaH < 0 ? 0 : sqrt(deltaH);
	float sc = 1.0 + 0.045 * c1;
	float sh = 1.0 + 0.015 * c1;
	float deltaLKlsl = deltaL / (1.0);
	float deltaCkcsc = deltaC / (sc);
	float deltaHkhsh = deltaH / (sh);
	float i = deltaLKlsl * deltaLKlsl + deltaCkcsc * deltaCkcsc + deltaHkhsh * deltaHkhsh;

	return i < 0 ? 0 : sqrt(i);
}