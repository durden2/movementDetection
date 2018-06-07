#pragma once
#include <string>
#include <iostream>
#include <vector>
using namespace std;

class labDistance
{

public:
	unsigned char*** firstImage;
	unsigned char*** secondImage;
	vector<vector<vector<float>>> firstMean;
	vector<vector<vector<float>>> secondMean;
	int width;
	int height;

	vector<float> convertToLab(unsigned char R, unsigned char G, unsigned char B);
	unsigned char **calculateNewPosisions();
	float calculateDelta(vector<float> labA, vector<float> labB);
	void measureFactors();
	vector<vector<vector<float>>> calculateMeanValues(unsigned char **R, unsigned char **G, unsigned char **B, int height, int width);
	labDistance(unsigned char **R, unsigned char **G, unsigned char **B, unsigned char **R2, unsigned char **G2, unsigned char **B2, int height, int width);
};
