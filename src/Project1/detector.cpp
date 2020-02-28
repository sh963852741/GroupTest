#include "detector.h"
using namespace std;
Detector::Detector(Mat image) :image(image) {};
int Detector::Detect()
{
	FinderPatternFinder finder;
	finderPatternInfo = finder.FindFinderPattern(image);
	Point2d point = CalcPosition(25, 25, 3, 3);
	return ProcessFinderPatternInfo(finderPatternInfo);
}

unsigned short** Detector::GetBinaryData(int width, int height)
{
	unsigned short** res = new unsigned short*[height];
	for (int i = 0; i < width; ++i)
	{
		res[i] = new unsigned short[width];
	}
	for (int i = 0; i < height; ++i)
	{
		for (int j = 0; j < width; ++j)
		{
			Point2d point = CalcPosition(25, 25, j, i);
			res[i][j] = image.at<uchar>(point) ? 1 : 0;
			cout << res[i][j] << ' ';
		}
		cout << '\n';
	}
	return res;
}

int Detector::ProcessFinderPatternInfo(FinderPatternInfo info)
{

	return 0;
}

Point2d Detector::CalcPosition(int width,int height, int x, int y)
{
	
	dx = (finderPatternInfo.topRight.x - finderPatternInfo.topLeft.x) / (width - 7);
	dy = (finderPatternInfo.bottomLeft.y - finderPatternInfo.topLeft.y) / (height - 7);

	return Point2d(finderPatternInfo.topLeft.x + (x - 3) * dx, finderPatternInfo.topLeft.y + (y - 3) * dy);
}
