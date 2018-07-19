#pragma once
#include <stdio.h>
#include <iostream>
using namespace std;

class histogram {
	public:
	int histogram_size = 255;

	int *R;
	int *G;
	int *B;
	int w = 0;
	int h = 0;

	histogram() {
		R = new int[histogram_size];
		G = new int[histogram_size];
		B = new int[histogram_size];
		for (int i = 0; i < histogram_size; i++) {
			R[i] = 0;
			G[i] = 0;
			B[i] = 0;
		}
	}

	void setHistogramValues(unsigned char **Rinput, unsigned char **Ginput, unsigned char **Binput, int w, int h, int startX = 0, int startY = 0) {
		for (int i = startX; i < h; i++)
			for (int j = startY; j < w; j++) {
				R[Rinput[i][j]]++;
				G[Ginput[i][j]]++;
				B[Binput[i][j]]++;
			}
	}


};