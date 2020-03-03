#include <cstdio>
#include "detector.h"
#include "opencv2/opencv.hpp" //opencv 的头文件
#include "slicer.h"
#include "VedioGenerator.h"
using namespace std;

using namespace cv; //opencv 的命名空间


int main()
{
    Mat img = imread("C:\\Users\\yuyua\\Desktop\\test.jpg");  
    Mat imgGray, res;
    cvtColor(img, imgGray, COLOR_BGR2GRAY);
    double x = threshold(imgGray, res, 128, 255, THRESH_BINARY_INV);
    Detector d(res);
    d.Detect();
    d.GetBinaryData(192, 108);
    //VedioGenerator a;
    //a.Read();
    //a.Draw();

    return 0;
}