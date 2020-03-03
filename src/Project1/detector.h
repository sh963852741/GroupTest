#pragma once
#include "FinderPatternFinder.h"
#include "AlignPatternFinder.h"

class Detector
{
public:
	Detector(Mat image);
	int Detect();
	unsigned short** GetBinaryData(int width, int height);
private:
	Mat image;
	FinderPatternFinder finder1;
	AlignmentPatternFinder alignmentFinder;
	FinderPatternInfo finderPatternInfo;
	AlignmentPattern alignmentPattern;
	double dx;
	double dy;

	double CalculateModuleSize();
	int ProcessFinderPatternInfo(FinderPatternInfo info); //返回值是暂时的
	Point2d CalcPosition(int width, int height, int x, int y);
	AlignmentPattern FindAlignmentInRegion(double moduleSize, int estAlignmentX, int estAlignmentY, int allowanceFactor);
	//~Detector();
};