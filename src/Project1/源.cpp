#include <cstdio>
#include "detector.h"
#include "opencv2/opencv.hpp" //opencv 的头文件
#include "slicer.h"
#include "FileToPicture.h"
using namespace std;

using namespace cv; //opencv 的命名空间


int main()
{
    Mat imgGray, res;
    for (int i = 0; i <= 393; i++)
    {
        Mat img = imread("D:\\QR_Code\\QR_Code" + to_string(i) + ".jpg");
        cvtColor(img, imgGray, COLOR_BGR2GRAY);
        threshold(imgGray, res, 128, 255, THRESH_BINARY_INV);
        Detector d(res);
        if (!d.Detect())throw;
        // d.GetBinaryData(192, 108, 10);
    }

    
    // FileToPicture a;
    // a.MakePictureSequence();

    return 0;
}