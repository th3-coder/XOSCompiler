#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

// linux specific directories 

#include <cstring>
#include <iostream>

using namespace cv;

int main()
{
        char path[30];
        std::cout << "Enter file name: ";
        std::cin >> path;
        strncat(path, ".jpg", 5);       
        
        Mat img = imread(path, IMREAD_COLOR);
        
        if(img.empty()) {
                std::cout << "Could not read the image: " << path << std::endl;
                return 1;
        }

        Mat bilateralImg, gaussImg, medianImg;

        bilateralFilter(img, bilateralImg, 45, 45, 45);
        GaussianBlur(img, gaussImg, Size(15, 15), 9);
        medianBlur(img, medianImg, 15);

        imshow("Bilateral", bilateralImg);
        imshow("Gauss", gaussImg);
        imshow("Median Blur", medianImg);

        //imshow("Display window", img);
        int k = waitKey(0); // Wait for a keystroke in the window

        if(k == 's'){
                path[0] = '\0';
                std::cout << "Type output file name:\n";
                std::cin >> path;
                strncat(path, ".png", 5);
                imwrite(path, img);
        }
         
        return 0;
}
