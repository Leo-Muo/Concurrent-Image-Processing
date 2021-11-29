#include <iostream>
#include "Image.h"

int main()
{
    std::cout << "Hello World!\n";

    Image blank(100, 100, 3);
    blank.write("blank.jpg");


    Image test("imageJPG.jpg");
    test.write("new.png");


    Image copy = test;

    for (int i = 0; i < (copy.w*copy.channels); ++i)
    {
        copy.data[i] = 255;
    }
    copy.write("copy.png");
    
}
