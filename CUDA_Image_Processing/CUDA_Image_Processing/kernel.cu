#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define BYTE_BOUND(value) value < 0 ? 0 : (value > 255 ? 255 : value)
#include <stdlib.h>
#include <chrono>
#include "stb_image.h"
#include <stdio.h>
#include <fstream>
#include <cstdint>
#include <cstdio>
#include <stdint.h>
#include "stb_image_write.h"

#include "cuda_runtime.h"
#include <cuda.h>
#include <cstdio>
#include <iostream>
#include "device_launch_parameters.h"


using namespace std;

__global__ void run_on_gpu();
__device__ void execute(const char* name);
void grayscale(int channels, size_t size, unsigned char* data);



int main() {
    int width, height, channels;
    unsigned char* img = stbi_load("img1.jpg", &width, &height, &channels, 0);
    size_t size = width * height * channels;

    if (img == NULL) {
        printf("Error in loading the image\n");
        exit(1);
    }

    printf("Loaded image with a width of %dpx, a height of %dpx and %d channels\n", width, height, channels);

    grayscale(channels, size, img);

    stbi_write_png("sky.png", width, height, channels, img, width * channels);
    stbi_write_jpg("sky2.jpg", width, height, channels, img, 100);

    run_on_gpu << <1, 2 >> > ();
    cudaDeviceSynchronize();
    cout << "Finished" << endl;
}

void grayscale(int channels, size_t size, unsigned char* data) {
    if (channels < 3)
    {
        std::cout << "Image has less than 3 channels" << endl;
    }
    else {
        for (int i = 0; i < size; i += channels)
        {
            int gray = (data[i] + data[i + 1] + data[i + 2]) / 3;
            memset(data + i, gray, 3);
        }
    }
   
}

__global__ void run_on_gpu() {
    const char* name;
    if (threadIdx.x == 0) {
        name = "Thread 1";
    }
    else {
        name = "Thread 2";
    }
    execute(name);
}

__device__ void execute(const char* name) {
    printf("%s: first\n", name);
    printf("%s: second\n", name);
    printf("%s: third\n", name);
}
