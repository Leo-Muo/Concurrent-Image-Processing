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


__global__ void gaussian_blur(int* dev_ker_w, int* dev_ker_h, double* dev_kern, int* dev_channels, size_t* dev_size, int* dev_width, int* dev_height, unsigned char* dev_image, unsigned char* dev_output_image);
void grayscale(int channels, size_t size, unsigned char* data);


int main() {

    double ker3x3[9] = {
        1, 3, 1,
        3, 9, 3,
        1, 3, 1
    };

    double ker5x5[25] = {
       1,4,7,4,1,
       4,16,26,16,4,
       7,26,41,26,7,
       4,16,26,16,4,
       1,4,7,4,1
    };

    double ker7x7[49] = {
       1,4,7,10,7,4,1,
       4,12,26,33,26,12,4,
       7,26,55,71,55,26,7,
       10,33,71,91,71,33,10,
       7,26,55,71,55,26,7,
       4,12,26,33,26,12,4,
       1,4,7,10,7,4,1
    };

    int width, height, channels;
    
    int kernel_diamension = 7;

    /*if (img == NULL) {
        printf("Error in loading the image\n");
        exit(1);
    }*/

    //printf("Loaded image with a width of %dpx, a height of %dpx and %d channels\n", width, height, channels);

    double* dev_kern;
    int* dev_ker_w;
    int* dev_ker_h;
    int* dev_channels;
    int* dev_width;
    int* dev_height;
    size_t* dev_size;
    unsigned char* dev_image;
    unsigned char* dev_output_image = NULL;
    unsigned char* dev_data_temp = NULL;
    int j = 1;

    const char* image_file_array[8] = { "img1.jpg", "img2.jpg", "img3.jpeg", "img4.jpg", "img5.jpg", "img6.jpg", "img7.jpg", "img8.jpg" };
    
    for (int i = 1; i <= 32; i++)
    {
        unsigned char *img = stbi_load(image_file_array[i], &width, &height, &channels, 0);
       // std::cout << "Read: " << image_file_array[3] << endl;
        size_t size = width * height * channels;
        unsigned char* output_img = (unsigned char*)malloc(size * sizeof(unsigned char));
        unsigned char* data_temp = (unsigned char*)malloc(size * sizeof(unsigned char));

        auto start = chrono::steady_clock::now();
        cudaMalloc(&dev_ker_w, sizeof(int));
        cudaMalloc(&dev_ker_h, sizeof(int));
        cudaMalloc(&dev_channels, sizeof(int));
        cudaMalloc(&dev_width, sizeof(int));
        cudaMalloc(&dev_height, sizeof(int));
        cudaMalloc(&dev_size, sizeof(size_t));
        cudaMalloc((void**)&dev_image, size * sizeof(unsigned char));
        cudaMalloc((void**)&dev_output_image, size * sizeof(unsigned char));
        cudaMalloc(&dev_kern, 49 * sizeof(double));

        cudaMemcpy(dev_ker_w, &kernel_diamension, sizeof(int), cudaMemcpyHostToDevice);
        cudaMemcpy(dev_ker_h, &kernel_diamension, sizeof(int), cudaMemcpyHostToDevice);
        cudaMemcpy(dev_channels, &channels, sizeof(int), cudaMemcpyHostToDevice);
        cudaMemcpy(dev_width, &width, sizeof(int), cudaMemcpyHostToDevice);
        cudaMemcpy(dev_height, &height, sizeof(int), cudaMemcpyHostToDevice);
        cudaMemcpy(dev_size, &size, sizeof(size_t), cudaMemcpyHostToDevice);
        cudaMemcpy(dev_kern, &ker7x7, 49 * sizeof(double), cudaMemcpyHostToDevice);
        cudaMemcpy(dev_image, img, size * sizeof(unsigned char), cudaMemcpyHostToDevice);

        gaussian_blur << <i, channels >> > (dev_ker_w, dev_ker_h, dev_kern, dev_channels, dev_size, dev_width, dev_height, dev_image, dev_output_image);
        cudaDeviceSynchronize();
        cudaMemcpy(output_img, dev_output_image, size * sizeof(unsigned char), cudaMemcpyDeviceToHost);

        auto end = chrono::steady_clock::now();
        auto time = chrono::duration_cast<chrono::milliseconds>(end - start).count();
        //std::cout << "Elapsed time in milliseconds: " << time << " ms" << endl;
       // std::cout << "=========================================================" << endl;

       // stbi_write_png("result.png", width, height, channels, output_img, width * channels);

        int fileNumber = 0;
        
        std::ofstream myfile;
        myfile.open("results.csv", std::ios_base::app);
        myfile << time << "," << i << endl;
        myfile.close();



    }

    

    cudaFree(dev_ker_w);
    cudaFree(dev_ker_h);
    cudaFree(dev_channels);
    cudaFree(dev_width);
    cudaFree(dev_height);
    cudaFree(dev_size);
    cudaFree(dev_image);
    cudaFree(dev_output_image);

    cout << "Finished" << endl;
}


__global__ void gaussian_blur(int* dev_ker_w, int* dev_ker_h, double* dev_kern, int* dev_channels,
    size_t* dev_size, int* dev_width, int* dev_height, unsigned char* dev_image, unsigned char* dev_output_image) {

    const auto num_row = *dev_height / gridDim.x;

    if (num_row >= 1) {

        int cr = (*dev_ker_w - 1) / 2;
        int cc = (*dev_ker_h - 1) / 2;
        int center = cr * (*dev_ker_w) + cc;


        int dmo = 0;
        for (int i = 0; i < ((*dev_ker_h) * (*dev_ker_w)); i++)
        {
            dmo += dev_kern[i];
        }


        unsigned long start_row = num_row * blockIdx.x;
        unsigned long end_row;
        if (blockIdx.x == gridDim.x - 1)
        {
             end_row = *dev_height;
        }
        else {
             end_row = num_row * (blockIdx.x + 1);
        }
        

        int channel = threadIdx.x;
        int k = (start_row * (*dev_width) ) + channel;
        while ( k <= ((*dev_width) * end_row * (*dev_channels)) - 1)
        {
            double sum = 0;

            for (long i = -((long)cr); i < (long)*dev_ker_h - cr; ++i) {
                long row = ((long)k / (*dev_channels)) / (*dev_width) - i;
                if (row < 0 || row >(*dev_height) - 1) {
                    continue;
                }

                for (long j = -((long)cc); j < (long)*dev_ker_w - cc; ++j) {
                    long col = ((long)k / (*dev_channels)) % (*dev_width) - j;
                    if (col < 0 || col >(*dev_width) - 1) {
                        continue;
                    }
                    sum += dev_kern[center + i * (long)*dev_ker_w + j] * dev_image[(row * (*dev_width) + col) * (*dev_channels) + channel];
                }
            }
            dev_output_image[k] = (unsigned char)BYTE_BOUND(round(sum / dmo));
            k += *dev_channels; 
        }
    }
    else {
        printf("Number of BLocks are large\n");
    }
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