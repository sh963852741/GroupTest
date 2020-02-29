#pragma once
#include "opencv2/opencv.hpp"
#include <fstream>
#include <iostream>
#include <string>
#include <bitset>
#define w 10
#define charSize 100
#define PicLength 20794
using namespace cv;
using namespace std;
class VedioGenerator
{
public:
	void Draw_Template(Mat img);
	void Draw_White(Mat img, int pt00, int pt01, int pt10, int pt11);
	void Draw_Black(Mat img, int pt00, int pt01, int pt10, int pt11);
	void Draw_White_block(Mat img, int pt0, int pt1);
	void Draw_Black_block(Mat img, int pt0, int pt1);
	int Read();
	int chksum(void* dataptr, unsigned short len);
	void Draw();
};