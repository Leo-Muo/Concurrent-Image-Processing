#include <iostream>
#include "Image.h"
#include <stdlib.h>
#include <chrono>
#include "stb_image.h"
#include <stdio.h>
#include <fstream>

void getTimeGrayscale(const char* test);
void GaussianBlur(double kernel[], int diamension, const char* filename, int fileNumber);

using namespace std;


int main()
{
 
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

    const char* image_file_array[8] = { "img1.jpg", "img2.jpg", "img3.jpeg", "img4.jpg", "img5.jpg", "img6.jpg", "img7.jpg", "img8.jpg" };

    /*for (int i = 0; i < 8; i++)
    {
         GaussianBlur(ker5x5, 5, image_file_array[i], i);
    }*/

    GaussianBlur(ker7x7, 7, image_file_array[1], 2);

}


void GaussianBlur(double kernel[], int diamension, const char* filename, int fileNumber) {

    auto start = chrono::steady_clock::now();
    Image img(filename);
    for (int i = 0; i < img.channels; i++)
    {
        img.std_convolve_clamp_to_0(i, diamension, diamension, kernel);
    }
    img.write("res.png");
    auto end = chrono::steady_clock::now();
    auto time= chrono::duration_cast<chrono::milliseconds>(end - start).count();
    std::cout << "Elapsed time in milliseconds: " << time << " ms" << endl;
    std::cout << "=========================================================" << endl;

    std::ofstream myfile;
    myfile.open("resultTime.csv", std::ios_base::app);
    myfile << time << "," << fileNumber + 1 << endl;
    myfile.close();

}

void getTimeGrayscale(const char* filename) {

    Image test(filename);
    auto start = chrono::steady_clock::now();
    test.grayscale();
    auto end = chrono::steady_clock::now();

    cout << "Elapsed time in milliseconds: "
        << chrono::duration_cast<chrono::milliseconds>(end - start).count()
        << " ms" << endl;

    std::cout << "=========================================================" << endl;
}