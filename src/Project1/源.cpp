#include <cstdio>
#include "detector.h"
#include "opencv2/opencv.hpp" //opencv 的头文件
#include "slicer.h"
#include "FileToVedio.h"
#include "FileToMp4.h"
#include <bitset>
using namespace std;

using namespace cv; //opencv 的命名空间


int main()
{
	/*FileToVedio a(R"(C:\Users\yuyua\Desktop\e1.bin)");
	a.GenerateVedio(R"(D:\QR_Code\QR_Code.mp4)", 1080, 1920, 10);*/
	
	/*FileToMp4 a;
	a.MakePictureSequence();*/
	int start = getTickCount();

	Mat imgGray, res;
	Mat img;
	VedioSlicer DT(R"(C:\Users\yuyua\Desktop\VID_20200317_142535.mp4)");
	int i = 0;
	while (1)
	{
		++i;
		img = DT.GetNextFrame();
		//if (i % 3 != 1)continue;
		cout << i << ' ';
		//img = imread(R"(D:\QR_Code\Code10_trans.png)");
		if (img.empty())
		{
			break;
		}
		//imwrite(".//1.jpg", res);
 		Detector d(img, 108, 192, 10);
		if (!d.Detect())throw;
		/*char x[2];
		d.picCount(x);
		cout << (int)x[0] << (int)x[1] << endl;*/
		//d.GetBinaryData();
	}

	int end = getTickCount();
	int last = end - start;
	cout << "time consume: " << last / getTickFrequency() << 's' << endl;
	return 0;
}