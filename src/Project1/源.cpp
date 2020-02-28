#include <cstdio>
#include "detector.h"
#include "opencv2/opencv.hpp" //opencv 的头文件
#include "slicer.h"
using namespace std;

using namespace cv; //opencv 的命名空间


int main()
{

    Mat img = imread("C:\\Users\\yuyua\\Desktop\\下载.png");  //这个图片需要自己准备，放在project目录下，或者直接写绝对路径
    Mat imgGray, res;
    cvtColor(img, imgGray, COLOR_BGR2GRAY);
    double x = threshold(imgGray, res, 128, 255, THRESH_BINARY_INV);
    //cout << (int)res.at<uchar>(120, 60);
    Detector lalala(res);
    lalala.Detect();
    lalala.GetBinaryData(25,25);
    
    waitKey(10000);
    return 0;
}