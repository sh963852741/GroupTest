#pragma once
#include "FinderPatternFinder.h"

class Detector
{
public:
	Detector(Mat image);
	int Detect();
private:
	Mat image;
	double dx;
	double dy;
	int ProcessFinderPatternInfo(FinderPatternInfo info); //返回值是暂时的
	Point2d CalcPosition(FinderPatternInfo info, int width, int height, int x, int y);
	//~Detector();

private:

};