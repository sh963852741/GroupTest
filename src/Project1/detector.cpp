#include "detector.h"

Detector::Detector(Mat& srcImg, int blockRows, int blockCols, int moduleSize) :srcImg(srcImg), blockRows(blockRows), blockCols(blockCols),moduleSize(moduleSize)
{
	Mat imgGray;
	cvtColor(srcImg, imgGray, COLOR_BGR2GRAY);
	threshold(imgGray, image, 80, 255, THRESH_BINARY);
	imwrite(".//1.jpg", image);
	res = new char* [blockRows];
	for (int i = 0; i < blockRows; ++i)
	{
		res[i] = new char[blockCols];
		memset(res[i], 0, blockCols);
	}
	
};
bool Detector::Detect()
{
	bool x = finder1.FindFinderPattern(image, finderPatternInfo);
	if (!x)return false;
	CalculateModuleSize();
	double estAlignmentX = finderPatternInfo.topRight.position.x - finderPatternInfo.topLeft.position.x + finderPatternInfo.bottomLeft.position.x;
	double estAlignmentY = finderPatternInfo.topRight.position.y - finderPatternInfo.topLeft.position.y + finderPatternInfo.bottomLeft.position.y;
	int allowanceFactor = 4;
	while (allowanceFactor <= 16)
	{
		if (FindAlignmentInRegion(estAlignmentX, estAlignmentY, allowanceFactor))
		{
			Rectify(moduleSize, blockCols, blockRows);
			return true;
		}
		allowanceFactor <<= 1;
	}
	return false;
}

char** Detector::GetBinaryData()
{
	for (int i = 0; i < blockRows; ++i)
	{
		for (int j = 0; j < blockCols; ++j)
		{
			Point point = CalcPosition(10, j, i); //强制设置为10
			res[i][j] |= srcImg.ptr<uchar>(point.y, point.x)[0] > 150 ? 0x01 : 0;
			res[i][j] |= srcImg.ptr<uchar>(point.y, point.x)[1] > 120 ? 0x02 : 0;
			res[i][j] |= srcImg.ptr<uchar>(point.y, point.x)[2] > 90 ? 0x04 : 0;
			/*cout << (int)((res[i][j] & 7)^7);*/
			// cout << (int)res[i][j];
		}
		//cout << '\n';
	}
	return res;
}

void Detector::picCount(char data[])
{
	data[1] = 0;
	data[0] = 0;
	Point point = CalcPosition(10, 8, 0); //强制设置为10
	data[0] |= srcImg.ptr<uchar>(point.y, point.x)[0] > 150 ? 0x01 : 0;
	data[0] |= srcImg.ptr<uchar>(point.y, point.x)[1] > 120 ? 0x02 : 0;
	data[0] |= srcImg.ptr<uchar>(point.y, point.x)[2] > 90 ? 0x04 : 0;

	point = CalcPosition(10, 9, 0); //强制设置为10
	data[1] |= srcImg.ptr<uchar>(point.y, point.x)[0] > 150 ? 0x01 : 0;
	data[1] |= srcImg.ptr<uchar>(point.y, point.x)[1] > 120 ? 0x02 : 0;
	data[1] |= srcImg.ptr<uchar>(point.y, point.x)[2] > 90 ? 0x04 : 0;
}

Detector::~Detector()
{
	for (int i = 0; i < blockRows; ++i) {
		delete[] res[i];
	}
	delete[] res;
}

void Detector::CalculateModuleSize()
{
	double size1 = (finderPatternInfo.topRight.position.x - finderPatternInfo.topLeft.position.x) / (192 - 7);
	double size2 = (finderPatternInfo.bottomLeft.position.y - finderPatternInfo.topLeft.position.y) / (108 - 7);
	overallEstModuleSize = (size1 + size2) / 2.0;
}

Point Detector::CalcPosition(int moduleSize, int x, int y)
{
	return Point(x * moduleSize + moduleSize / 2, y * moduleSize + moduleSize / 2);
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
	warpPerspective(srcImg, srcImg, transformMatrix, Size(moduleSize * width, moduleSize * height));
	/*char addr[50];
	static int i = 0;
	sprintf_s(addr, "D:\\QR_Code\\Code%d_trans.png", ++i);
	imwrite(addr, srcImg);*/
}
