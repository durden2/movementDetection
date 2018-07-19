#pragma once

#include <string>
#include "TPGM.h"
#include <iostream>
#include "histogram.cpp"

class Image {
public:
	int frameSize = 4;
	int w;
	int h;
	int max_color;

	unsigned char **R;
	unsigned char **G;
	unsigned char **B;

	unsigned char **moveFrames;

	Image(std::string imagePath) {
		int h_ppm, w_ppm;
		int max_color_ppm;
		int hpos_ppm, i_ppm, j_ppm;

		if ((hpos_ppm = readPPMB_header(imagePath.c_str(), &h_ppm, &w_ppm, &max_color_ppm)) <= 0) exit(1);

		//Tablica dla skladowych R obrazka
		R = new unsigned char *[h_ppm];
		R[0] = new unsigned char[h_ppm * w_ppm];

		for (int i = 1; i < h_ppm; i++)
			R[i] = R[i - 1] + w_ppm;

		//Tablica dla skladowych G obrazka
		G = new unsigned char *[h_ppm];
		G[0] = new unsigned char[h_ppm * w_ppm];
		for (int i = 1; i < h_ppm; i++)
			G[i] = G[i - 1] + w_ppm;

		//Tablica dla skladowych B obrazka
		B = new unsigned char *[h_ppm];
		B[0] = new unsigned char[h_ppm * w_ppm];
		for (int i = 1; i < h_ppm; i++)
			B[i] = B[i - 1] + w_ppm;

		this->w = w_ppm;
		this->h = h_ppm;
		max_color = max_color_ppm;

		if (readPPMB_data(R[0], G[0], B[0], imagePath.c_str(), hpos_ppm, h_ppm, w_ppm, max_color_ppm) == 0) exit(1);
	}

	~Image() {
		delete[] R[0];
		delete[] R;

		delete[] G[0];
		delete[] G;

		delete[] B[0];
		delete[] B;
 
		delete[] moveFrames[0];
		delete[] moveFrames;
	}


	bool replace(std::string &str, const std::string &from, const std::string &to) {
		size_t start_pos = str.find(from);
		if (start_pos == std::string::npos)
			return false;
		str.replace(start_pos, from.length(), to);
		return true;
	}

	void writeImage(std::string imgPath) {
		std::string outfname_ppm = imgPath;
		replace(outfname_ppm, ".ppm", "_simple_sample.ppm");

		if (writePPMB_image(outfname_ppm.c_str(), R[0], G[0], B[0], h, w, max_color) == 0) exit(1);
	}

	void drawFrames() {
		for (int x = 0; x < h - frameSize; x++) {
			for (int y = 0; y < w - frameSize; y++) {
				if ((int) moveFrames[x][y] != 0) {
					R[x][y] = 0;
					G[x][y] = 0;
					B[x][y] = 255;
				}
			}
		}
	}

	void initMovementFrames() {
		moveFrames = new unsigned char *[h];
		moveFrames[0] = new unsigned char[h * w];
		for (int i = 1; i < h; i++)
			moveFrames[i] = moveFrames[i - 1] + w;
	
		int t = 0;

		for (int x = 0; x < h; x++) {
			for (int y = 0; y < w; y++) {
				if (x % frameSize == 0 || y % frameSize == 0) {
					moveFrames[x][y] = 1;
				}
				else {
					moveFrames[x][y] = 0;
				}
			}
		}
	}
};