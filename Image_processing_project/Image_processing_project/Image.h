#pragma once
#include <cstdint>
#include <cstdio>
#include <stdint.h>

enum ImageType {
	PNG, JPG, BMP, TGA, JPEG
};

struct Image {
	uint8_t* data = NULL;
	size_t size = 0;
	int w;
	int h;
	int channels;


	Image(const char* filemane);
	Image(int w, int h, int channel);
	Image(const Image& imh);
	~Image();

	bool read(const char* filename);
	bool write(const char* filename);

	ImageType getFileType(const char* filename);
	Image& grayscale();
	Image& std_convolve_clamp_to_0(int channel, int ker_w, int ker_h, double ker[]);
};