#include "detector.h"

Detector::Detector(Mat image) :image(image) {};
bool Detector::Detect()
{
	finderPatternInfo = finder1.FindFinderPattern(image);
	CalculateModuleSize();
	double estAlignmentX = finderPatternInfo.topRight.position.x - finderPatternInfo.topLeft.position.x + finderPatternInfo.bottomLeft.position.x;
	double estAlignmentY = finderPatternInfo.topRight.position.y - finderPatternInfo.topLeft.position.y + finderPatternInfo.bottomLeft.position.y;
	int allowanceFactor = 4;
	while (allowanceFactor <= 16)
	{
		if (FindAlignmentInRegion(estAlignmentX, estAlignmentY, allowanceFactor))return true;
		allowanceFactor <<= 1;
	}
	return false;
}

unsigned short** Detector::GetBinaryData(int width, int height, int moduleSize)
{
	Rectify(moduleSize, width, height);
	unsigned short** res = new unsigned short* [height];
	for (int i = 0; i < width; ++i)
	{
		res[i] = new unsigned short[width];
	}
	for (int i = 0; i < height; ++i)
	{
		for (int j = 0; j < width; ++j)
		{
			Point2d point = CalcPosition(moduleSize, j, i);
			res[i][j] = image.at<uchar>(point) ? 1 : 0;
			cout << res[i][j];
		}
		cout << '\n';
	}
	return res;
}

void Detector::CalculateModuleSize()
{
	double size1 = (finderPatternInfo.topRight.position.x - finderPatternInfo.topLeft.position.x) / (192 - 7);
	double size2 = (finderPatternInfo.bottomLeft.position.y - finderPatternInfo.topLeft.position.y) / (108 - 7);
	overallEstModuleSize = (size1 + size2) / 2.0;
}

Point2d Detector::CalcPosition(int moduleSize, int x, int y)
{
	return Point2d(x * moduleSize + moduleSize / 2, y * moduleSize + moduleSize / 2);
}

bool Detector::FindAlignmentInRegion(int estAlignmentX, int estAlignmentY, int allowanceFactor)
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

	void* p = alignmentFinder.Find(image, alignmentAreaLeftX, alignmentAreaTopY, alignmentAreaRightX - alignmentAreaLeftX, alignmentAreaBottomY - alignmentAreaTopY, overallEstModuleSize);
	if (p)
	{
		alignmentPattern = *(AlignmentPattern*)p;
		return true;
	}
	else return false;
}

void Detector::Rectify(int moduleSize, int width, int height)
{
	vector<Point2f>dst, src;

	dst.push_back(Point2d(moduleSize * 3.5, moduleSize * 3.5)); // TopLeft
	dst.push_back(Point2d(moduleSize * (width - 3.5), moduleSize * 3.5)); // TopRight
	dst.push_back(Point2d(moduleSize * 3.5, moduleSize * (height - 3.5))); // BottomLeft
	dst.push_back(Point2d(moduleSize * (width - 3.5), moduleSize * (height - 3.5))); // BottomRight

	src.push_back(finderPatternInfo.topLeft.position);
	src.push_back(finderPatternInfo.topRight.position);
	src.push_back(finderPatternInfo.bottomLeft.position);
	src.push_back(alignmentPattern.position);

	Mat transformMatrix = getPerspectiveTransform(src, dst);
	warpPerspective(image, image, transformMatrix, Size(moduleSize * width, moduleSize * height));
}
