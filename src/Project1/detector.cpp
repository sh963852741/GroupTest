#include "detector.h"
using namespace std;
Detector::Detector(Mat image) :image(image) {};
int Detector::Detect()
{
	FinderPatternFinder finder;
	FinderPatternInfo info = finder.FindFinderPattern(image);
	Point2d point = CalcPosition(info, 25, 25, 3, 3);
	return ProcessFinderPatternInfo(info);
}

int Detector::ProcessFinderPatternInfo(FinderPatternInfo info)
{

	return 0;
}

Point2d Detector::CalcPosition(FinderPatternInfo info,int width,int height, int x, int y)
{
	
	dx = (info.topRight.x - info.topLeft.x) / (width - 7);
	dy = (info.bottomLeft.y - info.topLeft.y) / (height - 7);

	return Point2d(info.topLeft.x + (x - 3) * dx, info.topLeft.y + (y - 3) * dy);
}
