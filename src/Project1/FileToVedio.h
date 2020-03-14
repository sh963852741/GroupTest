#pragma once
// #include <string>
#include <opencv2/opencv.hpp>
#include <fstream>
#include <list>
#include <map>
#include <string>
#include <vector>
#define BlockSize 10
using namespace std;
using namespace cv;


class FileToVedio
{
private:
	ifstream fileIn;
	vector<unsigned char> data;
	map<string, int[4]> value_of_color;		//颜色的rgb值以及它所对应的编码我用字典来储存，string表示颜色的名字，int[0-2]为rgb值，int[3]为颜色所对应的2bit的十进制值	
	Mat img;
	VideoWriter vedio;
	int currentDataIndex;//当前读到的数据编号，即当前读取到vector<unsigned char> data中第几个字符了
	int currentPic;  //当前图片的编号
	int currentRow; // 用于在生成图片时指示当前读取行的位置
	int currentCol;//用于在生成图片时指示当前读取列的位置
	bool currentPrity = false;
	//void DrawHead(int width, int height, int modusize);	
	//void ReadToMemory();
	//void DrawImage(int width, int height, int modusize);
	void DrawBlock(int x, int y, int widthor[]);
	//void PositionOfPic(int row, int col, int color);
	//void InitialFormat(int width, int height, int modusize); // 画定位点和对齐点
public:
	FileToVedio(const char* filePath);
	void ReadToMemory();
	void GenerateVedio(const char* vedioPath, int width, int height, int modusize);
	void InitialFormat(int width, int height, int modusize); // 画定位点和对齐点（为了方便测试我先把要测的函数变成public）
	int DrawHead(int width, int height, int modusize);		//返回值表示当前图片中数据保存数据的像素块数，用来判断是什么时候写入补齐码
	void PositionOfPic(int row, int col, int color);		//生彩色像素块时确定像素块位置的函数
	void DrawImage(int width, int height, int modusize);
};