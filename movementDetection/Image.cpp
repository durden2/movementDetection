#pragma once

#include <string>
#include "TPGM.h"
#include <iostream>
#include "histogram.cpp"

class Image {
public:
	int w;
	int h;
	int max_color;
	histogram hist = histogram();

	unsigned char **R;
	unsigned char **G;
	unsigned char **B;

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

		w = w_ppm;
		h = h_ppm;
		max_color = max_color_ppm;

		if (readPPMB_data(R[0], G[0], B[0], imagePath.c_str(), hpos_ppm, h_ppm, w_ppm, max_color_ppm) == 0) exit(1);

		hist.setHistogramValues(R, G, B, h, w);
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

	void drawFrames(unsigned char **frames) {
		for (int x = 0; x < h - 10; x++) {
			for (int y = 0; y < w - 10; y++) {
				if ((int) frames[x][y] != 0) {
					R[x][y] = 0;
					G[x][y] = 0;
					B[x][y] = 255;
				}
			}
		}
	}

	double compareImages(Image imgToCompare) {
		return hist.compareHistograms(hist, imgToCompare.hist);
	}
};