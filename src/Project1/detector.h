#pragma once
#include "FinderPatternFinder.h"
#include "AlignPatternFinder.h"

class Detector
{
public:
	Detector(Mat image);
	bool Detect();
	unsigned short** GetBinaryData(int width, int height,int moduleSize);
private:
	Mat image;
	FinderPatternFinder finder1;
	AlignmentPatternFinder alignmentFinder;
	FinderPatternInfo finderPatternInfo;
	AlignmentPattern alignmentPattern;
	double overallEstModuleSize;

	void CalculateModuleSize();
	Point2d CalcPosition(int moduleSize, int x, int y);
	bool FindAlignmentInRegion(int estAlignmentX, int estAlignmentY, int allowanceFactor);
	void Rectify(int moduleSize, int width, int height);
	//~Detector();
};