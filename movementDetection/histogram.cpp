#pragma once
#include <stdio.h>
#include <iostream>
using namespace std;

class histogram {
	public:
	int histogram_size = 255;

	int *R = new int[histogram_size];
	int *G = new int[histogram_size];
	int *B = new int[histogram_size];
	int w = 0;
	int h = 0;

	histogram() {
		for (int i = 0; i < histogram_size; i++) {
			R[i] = 0;
			G[i] = 0;
			B[i] = 0;
		}
	}

	~histogram() {
		delete[] R;
		delete[] G;
		delete[] B;
	}

	void setHistogramValues(unsigned char **Rinput, unsigned char **Ginput, unsigned char **Binput, int w, int h) {
		for (int i = 0; i < w; i++)
		for (int j = 0; j < h; j++) {
			R[Rinput[i][j]]++;
			G[Ginput[i][j]]++;
			B[Binput[i][j]]++;
		}
	}

	double singleCorrelation(int *input, int *input2) {
		double sum = 0;

		for (int i = 0; i < histogram_size; i++) {
			double t = pow(input[i] - input2[i], 2);
			double t2 = pow(input[i] + input2[i], 2);

			if (t2 == 0) continue;

			sum += (t / t2);
		}

		return sum;
	}

	double compareHistograms(histogram hist, histogram hToCompare) {
		double corrR = singleCorrelation(hist.R, hToCompare.R);
		double corrG = singleCorrelation(hist.G, hToCompare.G);
		double corrB = singleCorrelation(hist.B, hToCompare.B);

		double corrValue = (corrR + corrB + corrG) / 3;

		return corrValue;
	};

};