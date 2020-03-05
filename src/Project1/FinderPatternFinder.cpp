#include "FinderPatternFinder.h"

double FinderPatternFinder::average;

double Distance(FinderPattern pattern1, FinderPattern pattern2)
{
	double xDiff = pattern1.position.x - pattern2.position.x;
	double yDiff = pattern1.position.y - pattern2.position.y;
	return sqrt((xDiff * xDiff + yDiff * yDiff));
}
bool FinderPatternFinder::FinderPatternSort1(FinderPattern center1, FinderPattern center2)
{
	double dA = abs(center2.estimatedModuleSize - average);
	double dB = abs(center1.estimatedModuleSize - average);
	return (dA > dB);
}
bool FinderPatternFinder::FinderPatternSort2(FinderPattern a, FinderPattern b)
{
	return a.position.x < b.position.x;
}


void FinderPatternFinder::OrderBestPatterns(vector<FinderPattern> &patterns)
{
	double zeroOneDistance = Distance(patterns[0], patterns[1]);
	double oneTwoDistance = Distance(patterns[1], patterns[2]);
	double zeroTwoDistance = Distance(patterns[0], patterns[2]);

	FinderPattern pointA, pointB, pointC;

	if (oneTwoDistance >= zeroOneDistance && oneTwoDistance >= zeroTwoDistance)
	{
		pointB = patterns[0];
		pointA = patterns[1];
		pointC = patterns[2];
	}
	else if (zeroTwoDistance >= oneTwoDistance && zeroTwoDistance >= zeroOneDistance)
	{
		pointB = patterns[1];
		pointA = patterns[0];
		pointC = patterns[2];
	}
	else
	{
		pointB = patterns[2];
		pointA = patterns[0];
		pointC = patterns[1];
	}

	if ((((pointC.position.x - pointB.position.x) * (pointA.position.y - pointB.position.y)) - ((pointC.position.y - pointB.position.y) * (pointA.position.x - pointB.position.x))) < 0.0)
	{
		FinderPattern temp = pointA;
		pointA = pointC;
		pointC = temp;
	}

	patterns[0] = pointA;
	patterns[1] = pointB;
	patterns[2] = pointC;
}

bool FinderPatternFinder::FoundPatternCross(int stateCount[])
{
	int totalModuleSize = 0;
	for (int i = 0; i < 5; i++)
	{
		int count = stateCount[i];
		if (count == 0)
		{
			return false;
		}
		totalModuleSize += count;
	}
	if (totalModuleSize < 7)
	{
		return false;
	}
	int moduleSize = cvFloor((totalModuleSize << INTEGER_MATH_SHIFT) / 7);
	int maxVariance = cvFloor(moduleSize / 2);
	return abs(moduleSize - (stateCount[0] << INTEGER_MATH_SHIFT)) < maxVariance
		&& abs(moduleSize - (stateCount[1] << INTEGER_MATH_SHIFT)) < maxVariance
		&& abs(3 * moduleSize - (stateCount[2] << INTEGER_MATH_SHIFT)) < 3 * maxVariance
		&& abs(moduleSize - (stateCount[3] << INTEGER_MATH_SHIFT)) < maxVariance
		&& abs(moduleSize - (stateCount[4] << INTEGER_MATH_SHIFT)) < maxVariance;
}

double FinderPatternFinder::CenterFromEnd(int stateCount[], double end)
{
	return (end - stateCount[4] - stateCount[3]) - stateCount[2] / 2.0;
}

int FinderPatternFinder::HaveMultiplyConfirmedCenters()
{
	int confirmedCount = 0;
	double totalModuleSize = 0.0;
	int max = possibleCenters.size();
	for (int i = 0; i < max; i++)
	{
		FinderPattern pattern = possibleCenters[i];
		if (pattern.count >= CENTER_QUORUM)
		{
			confirmedCount++;
			totalModuleSize += pattern.estimatedModuleSize;
		}
	}
	if (confirmedCount < 3)
	{
		return false;
	}
	double average = totalModuleSize / max;
	double totalDeviation = 0.0;
	for (int i = 0; i < max; i++)
	{
		FinderPattern pattern = possibleCenters[i];
		totalDeviation += abs(pattern.estimatedModuleSize - average);
	}
	return totalDeviation <= 0.05 * totalModuleSize;
}

bool FinderPatternFinder::HandlePossibleCenter(int stateCount[], int i, int j)
{
	int stateCountTotal = stateCount[0] + stateCount[1] + stateCount[2] + stateCount[3] + stateCount[4];
	double centerJ = CenterFromEnd(stateCount, j);
	double centerI = CrossCheckVertical(i, cvFloor(centerJ), stateCount[2], stateCountTotal);
	if (!isnan(centerI))
	{
		centerJ = CrossCheckHorizontal(cvFloor(centerJ), cvFloor(centerI), stateCount[2], stateCountTotal);
		if (!isnan(centerJ))
		{
			double estimatedModuleSize = stateCountTotal / 7.0;
			bool found = false;
			int max = possibleCenters.size();
			for (int index = 0; index < max; index++)
			{
				FinderPattern* center = &possibleCenters[index];
				if (center->AboutEquals(estimatedModuleSize, centerI, centerJ))
				{
					center->IncrementCount();
					found = true;
					break;
				}
			}
			if (!found)
			{
				FinderPattern point = FinderPattern(centerJ, centerI, estimatedModuleSize);
				possibleCenters.push_back(point);
			}
			return true;
		}
	}
	return false;
}

vector<FinderPattern> FinderPatternFinder::SelectBestPatterns()
{
	if (possibleCenters.size() > 3) //实在无法判断
	{
		std::sort(possibleCenters.begin(), possibleCenters.end(), FinderPatternSort2); //注意升降序
		for (int i = possibleCenters.size() - 2; i > 1; i--)
		{
			possibleCenters.erase(possibleCenters.begin() + i);
		}
	}
	vector<FinderPattern>temp(3);
	temp.at(0) = (possibleCenters[0]);
	temp.at(1) = (possibleCenters[1]);
	temp.at(2) = (possibleCenters[2]);
	return temp;
}

double FinderPatternFinder::CrossCheckHorizontal(int startJ, int centerI, int maxCount, int originalStateCountTotal)
{
	int maxJ = image.cols;
	int stateCount[5];
	memcpy(stateCount, crossCheckStateCount, sizeof(crossCheckStateCount));

	int j = startJ;
	while (j >= 0 && image.at<uchar>(centerI,j))
	{
		stateCount[2]++;
		j--;
	}
	if (j < 0)
	{
		return NAN;
	}
	while (j >= 0 && !image.at<uchar>(centerI, j) && stateCount[1] <= maxCount)
	{
		stateCount[1]++;
		j--;
	}
	if (j < 0 || stateCount[1] > maxCount)
	{
		return NAN;
	}
	while (j >= 0 && image.at<uchar>(centerI, j) && stateCount[0] <= maxCount)
	{
		stateCount[0]++;
		j--;
	}
	if (stateCount[0] > maxCount)
	{
		return NAN;
	}

	j = startJ + 1;
	while (j < maxJ && image.at<uchar>(centerI, j))
	{
		stateCount[2]++;
		j++;
	}
	if (j == maxJ)
	{
		return NAN;
	}
	while (j < maxJ && !image.at<uchar>(centerI, j) && stateCount[3] < maxCount)
	{
		stateCount[3]++;
		j++;
	}
	if (j == maxJ || stateCount[3] >= maxCount)
	{
		return NAN;
	}
	while (j < maxJ && image.at<uchar>(centerI, j) && stateCount[4] < maxCount)
	{
		stateCount[4]++;
		j++;
	}
	if (stateCount[4] >= maxCount)
	{
		return NAN;
	}


	int stateCountTotal = stateCount[0] + stateCount[1] + stateCount[2] + stateCount[3] + stateCount[4];
	if (5 * abs(stateCountTotal - originalStateCountTotal) >= originalStateCountTotal)
	{
		return NAN;
	}

	return FoundPatternCross(stateCount) ? CenterFromEnd(stateCount, j) : NAN;
}

double FinderPatternFinder::CrossCheckVertical(int startI, int centerJ, int maxCount, int originalStateCountTotal)
{
	int maxI = image.rows;
	int stateCount[5];
	memcpy(stateCount, crossCheckStateCount, sizeof(crossCheckStateCount));

	int i = startI;
	while (i >= 0 && image.at<uchar>(i,centerJ))
	{
		stateCount[2]++;
		i--;
	}
	if (i < 0)
	{
		return NAN;
	}
	while (i >= 0 && !image.at<uchar>(i, centerJ) && stateCount[1] <= maxCount)
	{
		stateCount[1]++;
		i--;
	}

	if (i < 0 || stateCount[1] > maxCount)
	{
		return NAN;
	}
	while (i >= 0 && image.at<uchar>(i, centerJ) && stateCount[0] <= maxCount)
	{
		stateCount[0]++;
		i--;
	}
	if (stateCount[0] > maxCount)
	{
		return NAN;
	}

	i = startI + 1;
	while (i < maxI && image.at<uchar>(i, centerJ))
	{
		stateCount[2]++;
		i++;
	}
	if (i == maxI)
	{
		return NAN;
	}
	while (i < maxI && !image.at<uchar>(i, centerJ) && stateCount[3] < maxCount)
	{
		stateCount[3]++;
		i++;
	}
	if (i == maxI || stateCount[3] >= maxCount)
	{
		return NAN;
	}
	while (i < maxI && image.at<uchar>(i, centerJ) && stateCount[4] < maxCount)
	{
		stateCount[4]++;
		i++;
	}
	if (stateCount[4] >= maxCount)
	{
		return NAN;
	}

	int stateCountTotal = stateCount[0] + stateCount[1] + stateCount[2] + stateCount[3] + stateCount[4];
	if (5 * abs(stateCountTotal - originalStateCountTotal) >= 2 * originalStateCountTotal)
	{
		return NAN;
	}

	return FoundPatternCross(stateCount) ? CenterFromEnd(stateCount, i) : NAN;
}

int FinderPatternFinder::FindRowSkip()
{
	int max = possibleCenters.size();
	if (max <= 1)
	{
		return 0;
	}
	FinderPattern* firstConfirmedCenter = nullptr;
	for (int i = 0; i < max; i++)
	{
		FinderPattern center = possibleCenters[i];
		if (center.count >= CENTER_QUORUM)
		{
			if (firstConfirmedCenter == nullptr)
			{
				firstConfirmedCenter = new FinderPattern(center);
			}
			else
			{
				hasSkipped = true;
				return cvFloor((abs(firstConfirmedCenter->position.x - center.position.x) - abs(firstConfirmedCenter->position.y - center.position.y)) / 2);
			}
		}
	}
	return 0;
}

bool FinderPatternFinder::FindFinderPattern(Mat image, FinderPatternInfo& finderPatternInfo)
{
	bool tryHarder = false;
	this->image = image;
	const int maxI = image.rows;
	const int maxJ = image.cols;

	int iSkip = cvFloor((3 * maxI) / (4 * MAX_MODULES));
	if (iSkip < MIN_SKIP || tryHarder)iSkip = MIN_SKIP;

	bool done = false;
	int stateCount[5] = { 0,0,0,0,0 };
	for (int i = iSkip - 1; i < maxI; i += iSkip)
	{
		stateCount[0] = 0;
		stateCount[1] = 0;
		stateCount[2] = 0;
		stateCount[3] = 0;
		stateCount[4] = 0;
		int currentState = 0;
		for (int j = 0; j < maxJ; j++)
		{
			if (image.at<uchar>(i, j))
			{
				if ((currentState & 1) == 1)
				{
					currentState++;
				}
				stateCount[currentState]++;
			}
			else
			{
				if ((currentState & 1) == 0)
				{
					if (currentState == 4)
					{
						if (FoundPatternCross(stateCount))
						{
							bool confirmed = HandlePossibleCenter(stateCount, i, j);
							if (confirmed)
							{
								iSkip = 2;
								if (hasSkipped)
								{
									done = HaveMultiplyConfirmedCenters();
								}
								else
								{
									int rowSkip = FindRowSkip() / 16 * 9;
									if (rowSkip > stateCount[2])
									{
										i += rowSkip - stateCount[2] - iSkip;
										j = maxJ - 1;
									}
								}
							}
							else
							{
								stateCount[0] = stateCount[4];
								stateCount[1] = 0;
								stateCount[2] = 0;
								stateCount[3] = 0;
								stateCount[4] = 0;
								currentState = 1;
							}
						}
						else
						{
							stateCount[0] = stateCount[2];
							stateCount[1] = stateCount[3];
							stateCount[2] = stateCount[4];
							stateCount[3] = 1;
							stateCount[4] = 0;
							currentState = 3;
						}
					}
					else
					{
						stateCount[++currentState]++;
					}
				}
				else
				{
					stateCount[currentState]++;
				}
			}
		}
		if (FoundPatternCross(stateCount))
		{
			bool confirmed = HandlePossibleCenter(stateCount, i, maxJ);
			if (confirmed)
			{
				iSkip = stateCount[0];
				if (hasSkipped)
				{
					done = HaveMultiplyConfirmedCenters();
				}
			}
		}
	}
	if (possibleCenters.size() < 3)return false;
	vector<FinderPattern>patternInfo = SelectBestPatterns();
	OrderBestPatterns(patternInfo);
	finderPatternInfo = FinderPatternInfo(patternInfo);
	return true;
}
