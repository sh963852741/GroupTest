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

class FileToMp4 {
private:
	void Trans(FILE* readFp, FILE* writeFp);//文件加校验码 转二进制 存文件
	void Draw_Template(Mat img, int col, int row, int blockSize);//画定位点 对齐点
	int chksum(void *dataptr, unsigned short len);//校验码
	void Draw(int length, int width, int blockSize);//二进制画图
	void Draw_White_block(Mat img, int pt0, int pt1, int blockSize);
	void ffmpeg(int frameRate);//图片转视频
public:
	void MakePictureSequence();
};