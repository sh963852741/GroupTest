#pragma once
#include<string>
#include<iostream>
#include<fstream>
using namespace std;

class DataTranslator {
private:

	unsigned short**DataArray = nullptr;//整体二维码矩阵
	int dx = 25;
	int dy = 25;
	int sum = 0;
	int nowx = 0, nowy = 0, direction = 1;
	int ByteNum = 0;
	int Parity = 0;//奇数张为1，偶数张为0
	//void DtoString();

	int getnextD();
	bool WhetherInArea(int x, int y);
	short InternetCheckSum(unsigned short*buf, int nwords);
	void Initialization(unsigned short **tes);//Parity,DataArray进行初始化
	void OutPutData(unsigned char*data);
public:
	void DateTranslate(unsigned short**tes);//返回二进制数组

};
