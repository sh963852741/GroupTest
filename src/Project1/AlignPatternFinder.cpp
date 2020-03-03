#include "AlignPatternFinder.h"

AlignmentPatternFinder::AlignmentPatternFinder(){};
AlignmentPattern AlignmentPatternFinder::Find(Mat image, int startX, int startY, int width, int height, double moduleSize)
{
	this->image = image;
	this->moduleSize = moduleSize;
	int maxJ = startX + width;
	int middleI = startY + (height >> 1);

	int stateCount[3] = { 0,0,0 };
	for (int iGen = 0; iGen < height; iGen++)
	{
		int i = middleI + ((iGen & 0x01) == 0 ? ((iGen + 1) >> 1) : -((iGen + 1) >> 1));
		stateCount[0] = 0;
		stateCount[1] = 0;
		stateCount[2] = 0;
		int j = startX;

		while (j < maxJ && !image.at<uchar>(i, j))
		{
			j++;
		}
		int currentState = 0;
		while (j < maxJ)
		{
			if (image.at<uchar>(i, j))
			{
				if (currentState == 1)
				{
					stateCount[currentState]++;
				}
				else
				{
					if (currentState == 2)
					{
						if (FoundPatternCross(stateCount))
						{
							AlignmentPattern* confirmed = HandlePossibleCenter(stateCount, i, j);
							if (confirmed != nullptr)
							{
								return *confirmed;
							}
						}
						stateCount[0] = stateCount[2];
						stateCount[1] = 1;
						stateCount[2] = 0;
						currentState = 1;
					}
					else
					{
						stateCount[++currentState]++;
					}
				}
			}
			else
			{
				if (currentState == 1)
				{
					currentState++;
				}
				stateCount[currentState]++;
			}
			j++;
		}
		if (FoundPatternCross(stateCount))
		{
			AlignmentPattern* confirmed = HandlePossibleCenter(stateCount, i, maxJ);
			if (confirmed != nullptr)
			{
				return *confirmed;
			}
		}
	}

	if (!(possibleCenters.size() == 0))
	{
		return  possibleCenters[0];
	}

	throw "Couldn't find enough alignment patterns";
}

AlignmentPattern* AlignmentPatternFinder::HandlePossibleCenter(int stateCount[], int i, int j)
{
	int stateCountTotal = stateCount[0] + stateCount[1] + stateCount[2];
	double centerJ = CenterFromEnd(stateCount, j);
	double centerI = CrossCheckVertical(i, cvFloor(centerJ), 2 * stateCount[1], stateCountTotal);
	if (!isnan(centerI))
	{
		double estimatedModuleSize = (stateCount[0] + stateCount[1] + stateCount[2]) / 3.0;
		int max = possibleCenters.size();
		for (int index = 0; index < max; index++)
		{
			AlignmentPattern center = possibleCenters[index];
			if (center.AboutEquals(estimatedModuleSize, centerI, centerJ))
			{
				return new AlignmentPattern(centerJ, centerI, estimatedModuleSize);
			}
		}

		AlignmentPattern point = AlignmentPattern(centerJ, centerI, estimatedModuleSize);
		possibleCenters.push_back(point);
	}
	return nullptr;
}

bool AlignmentPatternFinder::FoundPatternCross(int stateCount[])
{
	double maxVariance = moduleSize / 2.0;
	for (int i = 0; i < 3; i++)
	{
		if (abs(moduleSize - stateCount[i]) >= maxVariance)
		{
			return false;
		}
	}
	return true;
}

double AlignmentPatternFinder::CenterFromEnd(int stateCount[], int end)
{
	return (end - stateCount[2]) - stateCount[1] / 2.0;
}

double AlignmentPatternFinder::CrossCheckVertical(int startI, int centerJ, int maxCount, int originalStateCountTotal)
{
	int maxI = image.rows;
	int stateCount[5];
	memcpy(stateCount, crossCheckStateCount, sizeof(crossCheckStateCount));
	stateCount[0] = 0;
	stateCount[1] = 0;
	stateCount[2] = 0;

	int i = startI;
	while (i >= 0 && image.at<uchar>(i, centerJ) && stateCount[1] <= maxCount)
	{
		stateCount[1]++;
		i--;
	}

	if (i < 0 || stateCount[1] > maxCount)
	{
		return NAN;
	}
	while (i >= 0 && !image.at<uchar>(i, centerJ) && stateCount[0] <= maxCount)
	{
		stateCount[0]++;
		i--;
	}
	if (stateCount[0] > maxCount)
	{
		return NAN;
	}

	i = startI + 1;
	while (i < maxI && image.at<uchar>(i, centerJ) && stateCount[1] <= maxCount)
	{
		stateCount[1]++;
		i++;
	}
	if (i == maxI || stateCount[1] > maxCount)
	{
		return NAN;
	}
	while (i < maxI && !image.at<uchar>(i, centerJ) && stateCount[2] <= maxCount)
	{
		stateCount[2]++;
		i++;
	}
	if (stateCount[2] > maxCount)
	{
		return NAN;
	}

	int stateCountTotal = stateCount[0] + stateCount[1] + stateCount[2];
	if (5 * abs(stateCountTotal - originalStateCountTotal) >= 2 * originalStateCountTotal)
	{
		return NAN;
	}

	return FoundPatternCross(stateCount) ? CenterFromEnd(stateCount, i) : NAN;
}
