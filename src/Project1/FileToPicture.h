#pragma once
#include <iostream>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <fstream>
#include <string>
#include <bitset>

using namespace cv;
using namespace std;
class FileToPicture
{
private:
	void Trans(FILE* readFp, FILE* writeFp);
	void Draw_Template(Mat img, int length, int width);
	int chksum(void* dataptr, unsigned short len);
	void Draw(int length, int width, int blockSize);
	void Draw_White_block(Mat img, int pt0, int pt1);
public:
	void MakePictureSequence();
};
