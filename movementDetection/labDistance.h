#pragma once
#include <string>
#include <iostream>
using namespace std;

class labDistance
{

public:
	unsigned char*** firstImage;
	unsigned char*** secondImage;
	float ***firstMean;
	float ***secondMean;
	int width;
	int height;

	float *convertToLab(unsigned char R, unsigned char G, unsigned char B);
	unsigned char **calculateNewPosisions();
	float calculateDelta(float* labA, float* labB);
	void measureFactors();
	float ***calculateMeanValues(unsigned char **R, unsigned char **G, unsigned char **B, int height, int width);
	labDistance(unsigned char **R, unsigned char **G, unsigned char **B, unsigned char **R2, unsigned char **G2, unsigned char **B2, int height, int width);
};
