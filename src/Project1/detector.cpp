#include "detector.h"

Detector::Detector(Mat image) :image(image) {};
int Detector::Detect()
{
	finderPatternInfo = finder1.FindFinderPattern(image);
	double moduleSize = CalculateModuleSize();
	double estAlignmentX = finderPatternInfo.topRight.x - finderPatternInfo.topLeft.x + finderPatternInfo.bottomLeft.x;
	double estAlignmentY = finderPatternInfo.topRight.y - finderPatternInfo.topLeft.y + finderPatternInfo.bottomLeft.y;
	alignmentPattern = FindAlignmentInRegion(moduleSize, estAlignmentX, estAlignmentY, 4);
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
			Point2d point = CalcPosition(width, height, j, i);
			res[i][j] = image.at<uchar>(point) ? 1 : 0;
			cout << res[i][j];
		}
		cout << '\n';
	}
	return res;
}

double Detector::CalculateModuleSize()
{
	double size1 = (finderPatternInfo.topRight.x - finderPatternInfo.topLeft.x) / (192 - 7);
	double size2 = (finderPatternInfo.bottomLeft.y-finderPatternInfo.topLeft.y) / (108 - 7);
	return (size1 + size2) / 2.0;
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

AlignmentPattern Detector::FindAlignmentInRegion(double overallEstModuleSize, int estAlignmentX, int estAlignmentY, int allowanceFactor)
{
	int allowance = cvFloor(allowanceFactor * overallEstModuleSize);
	int alignmentAreaLeftX = max(0, estAlignmentX - allowance);
	int alignmentAreaRightX = min(image.cols - 1, estAlignmentX + allowance);
	if (alignmentAreaRightX - alignmentAreaLeftX < overallEstModuleSize * 3)
	{
		throw "Error";
	}

	int alignmentAreaTopY = max(0, estAlignmentY - allowance);
	int alignmentAreaBottomY = min(image.rows - 1, estAlignmentY + allowance);

	return alignmentFinder.Find(image, alignmentAreaLeftX, alignmentAreaTopY, alignmentAreaRightX - alignmentAreaLeftX, alignmentAreaBottomY - alignmentAreaTopY, overallEstModuleSize);
}
