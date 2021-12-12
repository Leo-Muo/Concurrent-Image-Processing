#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define BYTE_BOUND(value) value < 0 ? 0 : (value > 255 ? 255 : value)
#include "Image.h"
#include "stb_image.h"
#include "stb_image_write.h"
#include <iostream>
#include <vector>
#include <numeric>

using namespace std;

Image::Image(const char* filename) {


	if (read(filename)) {
		std::cout << "Read: " << filename << endl;
		size = w * h * channels;  
	}
	else {
		std::cout << "Failed to Read: " << filename << endl;
	}
}


Image:: Image(int w, int h, int channels): w(w), h(h), channels(channels){
	size = w * h * channels;
	data = new uint8_t[size];
}


Image:: Image(const Image& img): Image(img.w,img.h,img.channels) {
	memcpy(data, img.data, size);
}


Image:: ~Image() {
	stbi_image_free(data);
}

 bool Image::read(const char* filename){
	 data = stbi_load(filename, &w, &h, &channels, 0);
	 return data != NULL;
 }
  

 bool Image::write(const char* filename){
	 ImageType type = getFileType(filename);
	 int success = 0;

	 switch (type) {
		case PNG:
			success = stbi_write_png(filename, w, h, channels, data, w*channels);
			break;
		case JPG:
			success = stbi_write_jpg(filename, w, h, channels, data, 100);
			break;
		case BMP:
			success = stbi_write_bmp(filename, w, h, channels, data);
			break;
		case TGA:
			success = stbi_write_tga(filename, w, h, channels, data);
			break;
	 }
	
	 return success != 0;
 }

 ImageType Image::getFileType(const char* filename) {
	 const char* ext = strrchr(filename, '.');

	 if (ext != nullptr) {
		 if (strcmp(ext, ".png") == 0)
		 {
			 return PNG;
		 }else if (strcmp(ext, ".jpg") == 0)
		 {
			 return JPG;
		 }
		 else if (strcmp(ext, ".bmp") == 0)
		 {
			 return BMP;
		 }
		 else if (strcmp(ext, ".tga") == 0)
		 {
			 return TGA;
		 }
		 else if (strcmp(ext, ".jpeg") == 0)
		 {
			 return JPEG;
		 }
	 }
	 return PNG;
 }

 Image& Image::grayscale() {
	 if (channels < 3)
	 {
		 std::cout << "Image has less than 3 channels" << endl;
	 }
	 else {
		 for (int i = 0; i < size; i+=channels)
		 {
			 int gray = (data[i] + data[i+1] + data[i+2]) / 3;
			 memset(data + i, gray, 3);
		 }
	 }
	 return *this;
 }


 Image& Image::std_convolve_clamp_to_0(int channel, int ker_w, int ker_h, double ker[]) {

	 int dmo = 0;
	 for (int i = 0; i < (ker_h*ker_w) ; i++)
	 {
		 dmo += ker[i];
	 }

	 uint8_t* new_data = new uint8_t[w*h];

	 int cr = (ker_w - 1) / 2;
	 int cc = (ker_h - 1) / 2;
	 int center = cr * ker_w + cc;

	 for (int k = channel; k < size; k += channels) {
		 double sum = 0;
		 std::vector<double> temp;

		 for (long i = -((long)cr); i < (long)ker_h - cr; ++i) {
			 long row = ((long)k / channels) / w-i;
			 if (row < 0 || row > h - 1) {
				 continue;
			 }
			 for (long j = -((long)cc); j < (long)ker_w - cc; ++j) {
				 long col = ((long)k / channels) % w-j;
				 if (col < 0 || col > w - 1) {
					 continue;
				 }
				sum += ker[center + i * (long)ker_w + j] * data[(row * w + col) * channels + channel];
			 }
		 }

		 new_data[k / channels] = (uint8_t)BYTE_BOUND(round(sum/dmo));
	 }
	 for (int k = channel; k < size; k += channels) {
		 data[k] = new_data[k / channels];
	 }
	 return *this;
 }