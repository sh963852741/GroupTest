#include "FileToVedio.h"

int FileToVedio::DrawHead(int height, int width, int modusize)
{
	//在图片中绘制奇偶校验位的像素块
	if (currentPic % 2 == 0)
	{
		PositionOfPic(height / 10, width / 10, 1);
		PositionOfPic(height / 10, width / 10, 2);
	}
	else
	{
		PositionOfPic(height / 10, width / 10, 2);
		PositionOfPic(height / 10, width / 10, 1);
	}

	// 这里向图片写数据的头部-图片数据对应的总像素块个数
	int DataBlockNum = width / 10 * height / 10 - 231 - 1;	//默认总像素块个数为width/10*height/10-64*3-25-4-10-1,4表示结束码，10表示头部，1表示舍弃
	if (data.size() * 4 - currentDataIndex * 4 < DataBlockNum) {		//最后一张图的数据像素块总数
		DataBlockNum = (data.size() - currentDataIndex) * 4;
	}
	int ByteBinary[16] = { 0 };		//用于存放数据区字节数的二进制值
	//进行十进制转二进制
	int copy = DataBlockNum;
	for (int i = 0; i < 16 && copy; ++i) {
		ByteBinary[15 - i] = copy % 2;
		copy /= 2;
	}
	//for (int i = 0; i < 16; ++i) cout << ByteBinary[i];
	int color = 0;	//计算所生成的像素块的颜色
	for (int i = 0; i < 16; ++i) {
		if (i % 2 == 0) color += ByteBinary[i] * 2;
		else {
			color += ByteBinary[i];
			//cout << color << endl;
			PositionOfPic(height / 10, width / 10, color);	//每取两个bit，根据它们计算出color（二进制转十进制），然后再调用彩色像素块生成函数
			color = 0;
		}
	}
	return DataBlockNum;	//返回值表示当前的图片中有多少像素块是拿来存放数据的
}

//彩色像素块生成函数
void FileToVedio::PositionOfPic(int row, int col, int color) {
	string ColorName = "";
	//这是根据color值找到对应的颜色，从而得到它们的rgb，进行彩色像素绘生成
	for (map<string, int[4]>::iterator it = value_of_color.begin(); it != value_of_color.end(); it++) {
		if (it->second[3] == color) ColorName = it->first;
	}
	//彩色色素块排列方式---判断是奇数行还是偶数行，以及判断是否在定位点和对齐点的区域内
	for (; currentRow < row; ++currentRow) {//
		for (; currentCol < col && currentCol >= 0;) {//
			if (currentRow <= 7) {
				if ((currentRow <= 7 && currentCol <= 7) || (currentRow <= 7 && currentCol >= col - 8 && currentCol <= col - 1)) {
					if (currentRow % 2 == 0) ++currentCol;
					else --currentCol;
					continue;
				}
				else if (currentRow % 2 == 0) {
					DrawBlock(currentRow, currentCol, value_of_color[ColorName]);
					++currentCol;
					return;
				}
				else if (currentRow % 2 == 1) {
					DrawBlock(currentRow, currentCol, value_of_color[ColorName]);
					--currentCol;
					return;
				}
			}
			else if (currentRow > 7 && currentRow < row - 8) {
				if (currentRow % 2 == 0) {
					DrawBlock(currentRow, currentCol, value_of_color[ColorName]);
					++currentCol;
					return;
				}
				else {
					DrawBlock(currentRow, currentCol, value_of_color[ColorName]);
					--currentCol;
					return;
				}
			}
			else {
				if ((currentRow >= row - 8 && currentRow <= row - 1 && currentCol <= 7) || (currentRow >= row - 6 && currentRow <= row - 2 && currentCol >= col - 6 && currentCol <= col - 2)) {
					if (currentRow % 2 == 0) ++currentCol;
					else --currentCol;
					continue;
				}
				else if (currentRow % 2 == 0) {
					DrawBlock(currentRow, currentCol, value_of_color[ColorName]);
					++currentCol;
					return;
				}
				else if (currentRow % 2 == 1) {
					DrawBlock(currentRow, currentCol, value_of_color[ColorName]);
					--currentCol;
					return;
				}
			}
		}
		if (currentRow % 2 == 0) --currentCol;
		else ++currentCol;
	}
}

void FileToVedio::ReadToMemory()
{
	fileIn.seekg(0, ios::end);
	int fileLength = fileIn.tellg(); // 文件长度
	fileIn.seekg(0, ios::beg);
	for (int i = 0; i < (int)(fileLength / 18); i++) // 每循环一次计算一次校验和
	{
		unsigned int accumulate = 0;
		unsigned short checksum = 0;
		for (int j = 0; j < 9; j++)
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

	if (fileIn.eof())return;

	/*当剩余长度不足18字节*/
	int i = 0; // 剩余长度
	while (!fileIn.eof())
	{
		char temp;
		fileIn.read(&temp, 1);
		data.push_back(temp);
		++i;
	}
	while (i < 18) // 剩余部分补0
	{
		data.push_back(0);
		++i;
	}
	unsigned int accumulate = 0;
	unsigned short checksum = 0;
	for (int i = data.size() - 20; i < data.size(); i += 2)
	{
		accumulate += data[i];
		accumulate += data[i + 1] << 8;
	}
	unsigned short carry = accumulate >> 16;
	checksum = ~(accumulate + carry);
	data.push_back(checksum);
	data.push_back(checksum >> 8);

	/*输出所有数据（debug）*/
	/*for (int i = 0; i < data.size(); ++i) {
		cout << data[i] << endl;
	}*/
}

void FileToVedio::DrawImage(int height, int width, int modusize)
{
	int PicNum = data.size() / ((width / 10 * height / 10 - 232) / 4);		//总共生成的图片数
	if (PicNum * ((width / 10 * height / 10 - 232) / 4) < data.size()) ++PicNum;
	currentPic = 1;		//当前图片数初始化为1
	while (currentPic <= PicNum) {
		InitialFormat(height, width, modusize);
		int DataBlockNum = DrawHead(width, height, modusize);//获得当前图片的数据像素块数
		int DataBinary[8] = { 0 };	//保存unsigned char数据的二进制数
		do {
			unsigned char currerntChar = data[currentDataIndex];	//获取当前的字符
			for (int i = 0; i < 8; ++i) {							//转化为二进制形式
				DataBinary[7 - i] = currerntChar % 2;
				currerntChar /= 2;
			}
			//生成彩色像素块，如drawHead()
			int color = 0;
			for (int i = 0; i < 8; ++i) {
				if (i % 2 == 0) color += DataBinary[i] * 2;
				else {
					color += DataBinary[i];
					PositionOfPic(height / 10, width / 10, color);	//每取两个bit，根据它们计算出color（二进制转十进制），然后再调用彩色像素块生成函数
					color = 0;
				}
			}
			++currentDataIndex;		//准备获取下一个字符
			//判断当前读取字符所对应的数据块是否已超出当前图的总字符块，即判断是否应切换到下一页图
		} while (currentDataIndex < data.size() && ((currentDataIndex * 4) - (currentPic - 1) * (width / 10 * height / 10 - 232)) < DataBlockNum);
		//舍弃4bit信息，将其置为0000，然后进行结束码0000 0000的绘制（4个像素块）
		for (int i = 0; i < 6; ++i) {
			PositionOfPic(height / 10, width / 10, 0);
		}
		//往图片中写入补齐码
		if (DataBlockNum < width / 10 * height / 10 - 232) {
			for (int i = 0; i < (width / 10 * height / 10 - 230) - DataBlockNum; ++i) {
				switch (i % 8) {
				case 0:case 2:
					PositionOfPic(height / 10, width / 10, 3);
					break;
				case 1:
					PositionOfPic(height / 10, width / 10, 2);
					break;
				case 3:case 5:case 7:
					PositionOfPic(height / 10, width / 10, 1);
					break;
				case 4:case 6:
					PositionOfPic(height / 10, width / 10, 0);
					break;
				}
			}
		}
		cout << currentPic++;
		vedio << img;
	}
	vedio.release();
}



void FileToVedio::DrawBlock(int x, int y, int color[]) //
{
	rectangle(
		img,
		Point(y * BlockSize + 0.5, x * BlockSize + 0.5),
		Point((y + 1) * BlockSize - 0.5, (x + 1) * BlockSize - 0.5),
		Scalar(color[0], color[1], color[2]),	//像素块的G、B、R值
		FILLED,
		LINE_8
	);
}

void FileToVedio::InitialFormat(int height, int width, int modusize)//添加width,height
{
	//生成图像并设置背景色为黑色
	img.create(height, width, CV_8UC3);
	img.setTo(Scalar(0, 0, 0));

	currentRow = 0;
	currentCol = 0;

	for (int i = 1; i < 6; i++) {
		//左上方框
		DrawBlock(1, i, value_of_color["white"]);
		DrawBlock(i, 1, value_of_color["white"]);
		DrawBlock(5, i, value_of_color["white"]);
		DrawBlock(i, 5, value_of_color["white"]);
		//右上方框
		DrawBlock(1, i + width / 10 - 7, value_of_color["white"]);
		DrawBlock(i, 1 + width / 10 - 7, value_of_color["white"]);
		DrawBlock(5, i + width / 10 - 7, value_of_color["white"]);
		DrawBlock(i, width / 10 - 2, value_of_color["white"]);
		//左下方框
		DrawBlock(height / 10 - 6, i, value_of_color["white"]);
		DrawBlock(i + height / 10 - 7, 1, value_of_color["white"]);
		DrawBlock(height / 10 - 2, i, value_of_color["white"]);
		DrawBlock(i + height / 10 - 7, 5, value_of_color["white"]);
	}
	for (int i = 0; i < 8; i++) {
		//左上两边
		DrawBlock(i, 7, value_of_color["white"]);
		DrawBlock(7, i, value_of_color["white"]);
		//右上两边
		DrawBlock(i, width / 10 - 8, value_of_color["white"]);
		DrawBlock(7, i + width / 10 - 8, value_of_color["white"]);
		//左下两边
		DrawBlock(i + height / 10 - 8, 7, value_of_color["white"]);
		DrawBlock(height / 10 - 8, i, value_of_color["white"]);
	}

	for (int i = 0; i < 3; i++) {
		//右下对齐点
		DrawBlock(height / 10 - 5, width / 10 - 5 + i, value_of_color["white"]);
		DrawBlock(height / 10 - 3, width / 10 - 5 + i, value_of_color["white"]);
		DrawBlock(height / 10 - 5 + i, width / 10 - 5, value_of_color["white"]);
		DrawBlock(height / 10 - 5 + i, width / 10 - 3, value_of_color["white"]);
	}
}

void FileToVedio::GenerateVedio(const char* vedioPath, int width, int height, int modusize)
{
	// 应当初始化 vedio
	int fourcc = vedio.fourcc('m', 'p', '4', 'v');
	vedio.open(vedioPath, fourcc, 1, Size(1920, 1080), true);
	ReadToMemory();
	DrawImage(width, height, modusize);
	// 把画出的图像放入vedio
}

FileToVedio::FileToVedio(const char* filePath)
{
	fileIn.open(filePath, ios::binary);

	currentPic = 0;		//生成的图片数的初始值为0
	currentDataIndex = 0;	//读取数据个数的初始值为0

	/*unsigned char temp = 252;
	for (int i = 0; i < 10200; ++i) {
		data.push_back(temp);
	}*/

	//颜色字典的初始值
	value_of_color["black"][0] = 0;
	value_of_color["black"][1] = 0;
	value_of_color["black"][2] = 0;
	value_of_color["black"][3] = 1;//

	value_of_color["white"][0] = 255;
	value_of_color["white"][1] = 255;
	value_of_color["white"][2] = 255;
	value_of_color["white"][3] = 2;//

	value_of_color["red"][0] = 0;
	value_of_color["red"][1] = 0;
	value_of_color["red"][2] = 255;
	value_of_color["red"][3] = 0;//

	value_of_color["green"][0] = 0;
	value_of_color["green"][1] = 255;
	value_of_color["green"][2] = 0;
	value_of_color["green"][3] = 3;//
}


//int main()
//{
//	FileToVedio file("C:\\Users\\10900\\Desktop\\test.txt");
//	//file.InitialFormat(480, 640, 10);
//	//file.DrawHead(480, 640, 10);
//	//file.ReadToMemory();
//	file.DrawImage(600, 1000, 10);
//	system("pause");
//
//	return 0;
//}