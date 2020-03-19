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
	map<string, int[4]> value_of_color;		//��ɫ��rgbֵ�Լ�������Ӧ�ı��������ֵ������棬string��ʾ��ɫ�����֣�int[0-2]Ϊrgbֵ��int[3]Ϊ��ɫ����Ӧ��2bit��ʮ����ֵ	
	Mat img;
	VideoWriter vedio;
	int currentDataIndex;//��ǰ���������ݱ�ţ�����ǰ��ȡ��vector<unsigned char> data�еڼ����ַ���
	int currentPic;  //��ǰͼƬ�ı��
	int currentRow; // ����������ͼƬʱָʾ��ǰ��ȡ�е�λ��
	int currentCol;//����������ͼƬʱָʾ��ǰ��ȡ�е�λ��
	bool currentPrity = false;
	void Check_CalaCRC8(unsigned char* pdat, unsigned char len);
	//void DrawHead(int width, int height, int modusize);	
	//void ReadToMemory();
	//void DrawImage(int width, int height, int modusize);
	void DrawBlock(int x, int y, int widthor[]);
	//void PositionOfPic(int row, int col, int color);
	//void InitialFormat(int width, int height, int modusize); // ����λ��Ͷ����
public:
	FileToVedio(const char* filePath);
	void ReadToMemory();
	void GenerateVedio(const char* vedioPath, int width, int height, int modusize);
	void InitialFormat(int width, int height, int modusize); // ����λ��Ͷ���㣨Ϊ�˷���������Ȱ�Ҫ��ĺ������public��
	int DrawHead(int width, int height, int modusize);		//����ֵ��ʾ��ǰͼƬ�����ݱ������ݵ����ؿ����������ж���ʲôʱ��д�벹����
	void PositionOfPic(int row, int col, int color);		//����ɫ���ؿ�ʱȷ�����ؿ�λ�õĺ���
	void DrawImage(int width, int height, int modusize);
};