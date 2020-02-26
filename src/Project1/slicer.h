#pragma once
#include "opencv2/opencv.hpp"
using namespace cv;
class VedioSlicer
{
private:
	VideoCapture vedio;
	int frameNum;
public:
	VedioSlicer(String filePath);
	Mat GetSpecificFrame(int frameIndex);
	Mat GetNextFrame();
	int GetTotalFrames();
	bool SetCurrentFrame(int frameIndex);
	int GetCurrentFrame();
};
