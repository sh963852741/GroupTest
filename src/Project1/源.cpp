#include <cstdio>
#include "detector.h"
#include "opencv2/opencv.hpp" //opencv 的头文件
#include "slicer.h"
#include "FileToVedio.h"
using namespace std;

using namespace cv; //opencv 的命名空间


int main()
{
	Mat imgGray, res;
	Mat img;
	VedioSlicer DT(R"(C:\Users\yuyua\Desktop\录制视频_1.mp4)");
	int i = 0;
	while (1)
	{
		
		cout << ++i;
		img = DT.GetNextFrame();
		if (img.empty())
		{
			break;
		}
		imwrite(".//1.jpg", img);
		cvtColor(img, imgGray, COLOR_BGR2GRAY);
		threshold(imgGray, res, 128, 255, THRESH_BINARY_INV);
		Detector d(res, 108, 192, 10);
		if (!d.Detect())throw;
		d.GetBinaryData();
	}

	return 0;
}