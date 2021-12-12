#define PI 3.1415
#include <iostream>
#include "Image.h"
#include <stdlib.h>
#include <chrono>
#include "stb_image.h"
#include <stdio.h>

void getTimeGrayscale(const char* test);

using namespace std;


int main()
{
  
    /// GrayScale Code
   /* const char* image_file_array[8] = { "img1.jpg", "img2.jpg", "img3.jpg", "img4.jpg", "img5.jpg", "img6.jpg", "img7.jpg", "img8.jpg"};
    for (int i = 0; i < 8; i++)
    {
        getTimeGrayscale(image_file_array[i]);
    }*/

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


    Image test("img1.jpg");
    test.std_convolve_clamp_to_0(0, 3, 3, ker3x3);
    test.std_convolve_clamp_to_0(1, 3, 3, ker3x3);
    test.std_convolve_clamp_to_0(2, 3, 3, ker3x3);
    test.write("new.png");
 
    Image test2("img1.jpg");
    test2.std_convolve_clamp_to_0(0, 5, 5, ker5x5);
    test2.std_convolve_clamp_to_0(1, 5, 5, ker5x5);
    test2.std_convolve_clamp_to_0(2, 5, 5, ker5x5);
    test2.write("new2.png");

    Image test3("img1.jpg");
    test3.std_convolve_clamp_to_0(0, 7, 7, ker7x7);
    test3.std_convolve_clamp_to_0(1, 7, 7, ker7x7);
    test3.std_convolve_clamp_to_0(2, 7, 7, ker7x7);
    test3.write("new3.png");
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
