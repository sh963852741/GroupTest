#pragma once
#include "FinderPatternFinder.h"
#include "AlignPatternFinder.h"

class Detector
{
public:
	Detector(Mat image, int blockRows,int blockCols, int moduleSize);
	bool Detect();
	char** GetBinaryData();
	~Detector();
private:
	Mat image;
	Mat srcImg;
	FinderPatternFinder finder1;
	AlignmentPatternFinder alignmentFinder;
	FinderPatternInfo finderPatternInfo;
	AlignmentPattern alignmentPattern;
	double overallEstModuleSize;
	int moduleSize;
	int blockRows;
	int blockCols;
	char** res;

	void CalculateModuleSize();
	Point CalcPosition(int moduleSize, int x, int y);
	bool FindAlignmentInRegion(int estAlignmentX, int estAlignmentY, int allowanceFactor);
	void Rectify(int moduleSize, int width, int height);
	
};