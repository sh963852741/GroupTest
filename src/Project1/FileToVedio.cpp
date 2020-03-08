#include "FileToVedio.h"

void FileToVedio::DrawHead(int width, int height, int modusize)
{
	// 这里向图片写数据的头部
}

void FileToVedio::ReadToMemory()
{
	fileIn.seekg(0, ios::end);
	int fileLength = fileIn.tellg();
	fileIn.seekg(0, ios::beg);
	for(int i=0;i< (int)(fileLength/18);i++)
	{
		unsigned int accumulate = 0;
		unsigned short checksum = 0;
		for (int j=0; j < 9; j++)
		{
			fileIn.read((char*)&checksum, 2);
			accumulate += checksum;
			data.push_back(checksum);
			data.push_back(checksum >> 8);
		}
		unsigned short carry = accumulate >> 16;
		checksum = ~(accumulate + carry);
		data.push_back(checksum);
		data.push_back(checksum >> 8);
	}
	// 当剩余长度不足18字节
	int i = 0;
	while (!fileIn.eof())
	{
		char temp;
		fileIn.read(&temp, 1);
		data.push_back(temp);
		++i;
	}
	while (i<18)
	{
		data.push_back(0);
		++i;
	}
}

void FileToVedio::DrawImage(int width, int height, int modusize)
{
	InitialFormat(width, height, modusize);
	DrawHead(width,height, modusize);
	// 写入头部后继续写入数据直到图片结束
}

void FileToVedio::DrawBlock(int x, int y, int modusize, char color)
{

}

void FileToVedio::InitialFormat(int width, int height, int modusize)
{
}

void FileToVedio::GenerateVedio(const char* vedioPath, int width, int height, int modusize)
{
	// 应当初始化 vedio
	ReadToMemory();
	DrawImage(width, height, modusize);
	// 把画出的图像放入vedio
}

FileToVedio::FileToVedio(const char* filePath)
{
	fileIn.open(filePath, ios::binary);
}
