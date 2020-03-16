#include "FileToVedio.h"

int FileToVedio::DrawHead(int height, int width, int modusize)
{
	//��ͼƬ�л�����żУ��λ�����ؿ�
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

	// ������ͼƬд���ݵ�ͷ��-ͼƬ���ݶ�Ӧ�������ؿ����
	int DataBlockNum = width / 10 * height / 10 - 231 - 1;	//Ĭ�������ؿ����Ϊwidth/10*height/10-64*3-25-4-10-1,4��ʾ�����룬10��ʾͷ����1��ʾ����
	if (data.size() * 4 - currentDataIndex * 4 < DataBlockNum) {		//���һ��ͼ���������ؿ�����
		DataBlockNum = (data.size() - currentDataIndex) * 4;
	}
	int ByteBinary[16] = { 0 };		//���ڴ���������ֽ����Ķ�����ֵ
	//����ʮ����ת������
	int copy = DataBlockNum;
	for (int i = 0; i < 16 && copy; ++i) {
		ByteBinary[15 - i] = copy % 2;
		copy /= 2;
	}
	//for (int i = 0; i < 16; ++i) cout << ByteBinary[i];
	int color = 0;	//���������ɵ����ؿ����ɫ
	for (int i = 0; i < 16; ++i) {
		if (i % 2 == 0) color += ByteBinary[i] * 2;
		else {
			color += ByteBinary[i];
			//cout << color << endl;
			PositionOfPic(height / 10, width / 10, color);	//ÿȡ����bit���������Ǽ����color��������תʮ���ƣ���Ȼ���ٵ��ò�ɫ���ؿ����ɺ���
			color = 0;
		}
	}
	return DataBlockNum;	//����ֵ��ʾ��ǰ��ͼƬ���ж������ؿ�������������ݵ�
}

//��ɫ���ؿ����ɺ���
void FileToVedio::PositionOfPic(int row, int col, int color) {
	string ColorName = "";
	//���Ǹ���colorֵ�ҵ���Ӧ����ɫ���Ӷ��õ����ǵ�rgb�����в�ɫ���ػ�����
	for (map<string, int[4]>::iterator it = value_of_color.begin(); it != value_of_color.end(); it++) {
		if (it->second[3] == color) ColorName = it->first;
	}
	//��ɫɫ�ؿ����з�ʽ---�ж��������л���ż���У��Լ��ж��Ƿ��ڶ�λ��Ͷ�����������
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
	int fileLength = fileIn.tellg(); // �ļ�����
	fileIn.seekg(0, ios::beg);
	for (int i = 0; i < (int)(fileLength / 18); i++) // ÿѭ��һ�μ���һ��У���
	{
		unsigned int accumulate = 0;
		unsigned short checksum = 0;
		for (int j = 0; j < 9; j++)
		{
			fileIn.read((char*)&checksum, 2);
			accumulate += (0x00FF & checksum) << 8;
			accumulate += checksum >> 8;
			data.push_back(checksum);
			data.push_back(checksum >> 8);
		}
		unsigned short carry = accumulate >> 16;
		checksum = ~(accumulate + carry);
		data.push_back(checksum >> 8);
		data.push_back(checksum);
	}

	if (fileIn.eof())return;

	/*��ʣ�೤�Ȳ���18�ֽ�*/
	int i = 0; // ʣ�೤��
	while (!fileIn.eof())
	{
		char temp;
		fileIn.read(&temp, 1);
		data.push_back(temp);
		++i;
	}
	while (i < 18) // ʣ�ಿ�ֲ�0
	{
		data.push_back(0);
		++i;
	}
	unsigned int accumulate = 0;
	unsigned short checksum = 0;
	for (int i = data.size() - 20; i < data.size(); i += 2)
	{
		accumulate += data[i] << 8;
		accumulate += data[i + 1];
	}
	unsigned short carry = accumulate >> 16;
	checksum = ~(accumulate + carry);
	data.push_back(checksum >> 8);
	data.push_back(checksum);

	/*����������ݣ�debug��*/
	/*for (int i = 0; i < data.size(); ++i) {
		cout << data[i] << endl;
	}*/
}

void FileToVedio::DrawImage(int height, int width, int modusize)
{
	int PicNum = data.size() / ((width / 10 * height / 10 - 232) / 4);		//�ܹ����ɵ�ͼƬ��
	if (PicNum * ((width / 10 * height / 10 - 232) / 4) < data.size()) ++PicNum;
	currentPic = 1;		//��ǰͼƬ����ʼ��Ϊ1
	while (currentPic <= PicNum) {
		InitialFormat(height, width, modusize);
		int DataBlockNum = DrawHead(width, height, modusize);//��õ�ǰͼƬ���������ؿ���
		int DataBinary[8] = { 0 };	//����unsigned char���ݵĶ�������
		do {
			unsigned char currerntChar = data[currentDataIndex];	//��ȡ��ǰ���ַ�
			for (int i = 0; i < 8; ++i) {							//ת��Ϊ��������ʽ
				DataBinary[7 - i] = currerntChar % 2;
				currerntChar /= 2;
			}
			//���ɲ�ɫ���ؿ飬��drawHead()
			int color = 0;
			for (int i = 0; i < 8; ++i) {
				if (i % 2 == 0) color += DataBinary[i] * 2;
				else {
					color += DataBinary[i];
					PositionOfPic(height / 10, width / 10, color);	//ÿȡ����bit���������Ǽ����color��������תʮ���ƣ���Ȼ���ٵ��ò�ɫ���ؿ����ɺ���
					color = 0;
				}
			}
			++currentDataIndex;		//׼����ȡ��һ���ַ�
			//�жϵ�ǰ��ȡ�ַ�����Ӧ�����ݿ��Ƿ��ѳ�����ǰͼ�����ַ��飬���ж��Ƿ�Ӧ�л�����һҳͼ
		} while (currentDataIndex < data.size() && ((currentDataIndex * 4) - (currentPic - 1) * (width / 10 * height / 10 - 232)) < DataBlockNum);
		//����4bit��Ϣ��������Ϊ0000��Ȼ����н�����0000 0000�Ļ��ƣ�4�����ؿ飩
		for (int i = 0; i < 6; ++i) {
			PositionOfPic(height / 10, width / 10, 0);
		}
		//��ͼƬ��д�벹����
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
		// bitwise_not(img, img);
		/*++currentPic;*/
		char addr[50];
		sprintf_s(addr, "D:\\QR_Code\\QR_Code%d.png", currentPic++);
		imwrite(addr, img);
		//vedio << img;
	}
	vedio.release();
}



void FileToVedio::DrawBlock(int x, int y, int color[]) //
{
	rectangle(
		img,
		Point(y * BlockSize + 0.5, x * BlockSize + 0.5),
		Point((y + 1) * BlockSize - 0.5, (x + 1) * BlockSize - 0.5),
		Scalar(color[0], color[1], color[2]),	//���ؿ��G��B��Rֵ
		FILLED,
		LINE_8
	);
}

void FileToVedio::InitialFormat(int height, int width, int modusize)//���width,height
{
	//����ͼ�����ñ���ɫΪ��ɫ
	img.create(height, width, CV_8UC3);
	img.setTo(Scalar(255, 255, 255));

	currentRow = 0;
	currentCol = 0;

	for (int i = 1; i < 6; i++) {
		//���Ϸ���
		DrawBlock(1, i, value_of_color["black"]);
		DrawBlock(i, 1, value_of_color["black"]);
		DrawBlock(5, i, value_of_color["black"]);
		DrawBlock(i, 5, value_of_color["black"]);
		//���Ϸ���
		DrawBlock(1, i + width / 10 - 7, value_of_color["black"]);
		DrawBlock(i, 1 + width / 10 - 7, value_of_color["black"]);
		DrawBlock(5, i + width / 10 - 7, value_of_color["black"]);
		DrawBlock(i, width / 10 - 2, value_of_color["black"]);
		//���·���
		DrawBlock(height / 10 - 6, i, value_of_color["black"]);
		DrawBlock(i + height / 10 - 7, 1, value_of_color["black"]);
		DrawBlock(height / 10 - 2, i, value_of_color["black"]);
		DrawBlock(i + height / 10 - 7, 5, value_of_color["black"]);
	}
	for (int i = 0; i < 8; i++) {
		//��������
		DrawBlock(i, 7, value_of_color["black"]);
		DrawBlock(7, i, value_of_color["black"]);
		//��������
		DrawBlock(i, width / 10 - 8, value_of_color["black"]);
		DrawBlock(7, i + width / 10 - 8, value_of_color["black"]);
		//��������
		DrawBlock(i + height / 10 - 8, 7, value_of_color["black"]);
		DrawBlock(height / 10 - 8, i, value_of_color["black"]);
	}

	for (int i = 0; i < 3; i++) {
		//���¶����
		DrawBlock(height / 10 - 5, width / 10 - 5 + i, value_of_color["black"]);
		DrawBlock(height / 10 - 3, width / 10 - 5 + i, value_of_color["black"]);
		DrawBlock(height / 10 - 5 + i, width / 10 - 5, value_of_color["black"]);
		DrawBlock(height / 10 - 5 + i, width / 10 - 3, value_of_color["black"]);
	}
}

void FileToVedio::GenerateVedio(const char* vedioPath, int width, int height, int modusize)
{
	// Ӧ����ʼ�� vedio
	int fourcc = vedio.fourcc('h', 'v', 'c', '1');
	vedio.open(vedioPath, fourcc, 15, Size(1920, 1080), true);
	ReadToMemory();
	DrawImage(width, height, modusize);
	// �ѻ�����ͼ�����vedio
}

FileToVedio::FileToVedio(const char* filePath)
{
	fileIn.open(filePath, ios::binary);

	currentPic = 0;		//���ɵ�ͼƬ���ĳ�ʼֵΪ0
	currentDataIndex = 0;	//��ȡ���ݸ����ĳ�ʼֵΪ0

	/*unsigned char temp = 252;
	for (int i = 0; i < 10200; ++i) {
		data.push_back(temp);
	}*/

	//��ɫ�ֵ�ĳ�ʼֵ
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