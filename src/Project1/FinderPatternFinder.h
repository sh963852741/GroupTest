#pragma once
#include "opencv2/opencv.hpp"
#include <vector>
#include <algorithm>
using namespace cv;
using namespace std;

struct FinderPattern
{
	Point2d position;
	int count;
	double estimatedModuleSize;
	FinderPattern() {};
	FinderPattern(double posX, double posY, double estimatedModuleSize)
		:position(posX,posY), count(1), estimatedModuleSize(estimatedModuleSize) {};
	void IncrementCount() { count++; };
	bool AboutEquals(double moduleSize, double i, double j)
	{
		if (abs(i - position.y) <= moduleSize && abs(j - position.x) <= moduleSize)
		{
			double moduleSizeDiff = abs(moduleSize - estimatedModuleSize);
			return moduleSizeDiff <= 1.0 || moduleSizeDiff / estimatedModuleSize <= 1.0;
		}
		return false;
	}
};
struct FinderPatternInfo //结构体，用来存储FinderPattern的信息
{
	FinderPattern bottomLeft;
	FinderPattern topLeft;
	FinderPattern topRight;
	FinderPatternInfo(vector<FinderPattern>patternCenters)
		:bottomLeft(patternCenters[0]), topLeft(patternCenters[1]), topRight(patternCenters[2]) {};
	FinderPatternInfo() {};
};

class FinderPatternFinder
{
private:
	Mat image;
	vector<FinderPattern>possibleCenters;
	bool hasSkipped = false;
	int crossCheckStateCount[5] = { 0,0,0,0,0 };
	const int MIN_SKIP = 3;
	const int MAX_MODULES = 57;
	const int INTEGER_MATH_SHIFT = 8;
	const int CENTER_QUORUM = 2;


	static bool FinderPatternSort1(FinderPattern center1, FinderPattern center2);
	static bool FinderPatternSort2(FinderPattern a, FinderPattern b);
	static bool FinderPatternSort3(FinderPattern a, FinderPattern b);
	void OrderBestPatterns(vector<FinderPattern> &patterns);
	bool FoundPatternCross(int stateCount[]);
	double CenterFromEnd(int stateCount[], double end);
	int HaveMultiplyConfirmedCenters();
	bool HandlePossibleCenter(int stateCount[], int i, int j);
	vector<FinderPattern> SelectBestPatterns();
	double CrossCheckHorizontal(int startJ, int centerI, int maxCount, int originalStateCountTotal);
	double CrossCheckVertical(int startI, int centerJ, int maxCount, int originalStateCountTotal);
	int FindRowSkip();
public:
	bool FindFinderPattern(Mat image, FinderPatternInfo& finderPatternInfo); //输入二值化后的图像，返回FinderPattern的信息
	double static average;
};