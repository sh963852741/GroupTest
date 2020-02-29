#include<iostream>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include<opencv2/opencv.hpp>
#include <fstream>
#include <iostream>
#include <string>
#include <bitset>
#define w 10
#define charSize 100
#define PicLength 20794

using namespace cv;
using namespace std;

void Draw_Template(Mat img);
void Draw_White(Mat img, int pt00, int pt01, int pt10, int pt11);
int Read();
int chksum(void *dataptr, unsigned short len);
void Draw();

//D:\\ZZZ\\za\\Z\\gugu.jpg
int main() {
	Read();


	Draw();
	

	return(0);
}
void Draw_Template(Mat img) {
	//左上定位点
	Draw_White(img, 1, 1, 2, 6);
	Draw_White(img, 2, 1, 5, 2);
	Draw_White(img, 2, 5, 5, 6);
	Draw_White(img, 5, 1, 6, 6);
	Draw_White(img, 7, 0, 8, 8);
	Draw_White(img, 0, 7, 7, 8);
	Draw_White(img, 3, 3, 4, 4);
	//左下定位点
	Draw_White(img, 100, 0, 101, 8);
	Draw_White(img, 101, 7, 108, 8);
	Draw_White(img, 102, 1, 103, 6);
	Draw_White(img, 106, 1, 107, 6);
	Draw_White(img, 103, 1, 106, 2);
	Draw_White(img, 103, 5, 106, 6);
	Draw_White(img, 104, 3, 105, 4);
	//右上定位点
	Draw_White(img, 0, 184, 8, 185);
	Draw_White(img, 7, 185, 8, 192);
	Draw_White(img, 1, 186, 6, 187);
	Draw_White(img, 1, 190, 6, 191);
	Draw_White(img, 1, 187, 2, 190);
	Draw_White(img, 5, 187, 6, 190);
	Draw_White(img, 3, 188, 4, 189);
	//右下对齐点
	Draw_White(img, 104, 187, 105, 190);
	Draw_White(img, 105, 187, 106, 188);
	Draw_White(img, 105, 189, 106, 190);
	Draw_White(img, 106, 187, 107, 190);
}
void Draw_White(Mat img, int pt00, int pt01, int pt10, int pt11)
{
	rectangle(img,
		Point(pt01 * w, pt00 * w),
		Point(pt11 * w, pt10 * w),
		Scalar(256, 256, 256),
		FILLED,
		LINE_8);
}
void Draw_Black(Mat img, int pt00, int pt01, int pt10, int pt11)
{
	rectangle(img,
		Point(pt01 * w, pt00 * w),
		Point(pt11 * w, pt10 * w),
		Scalar(0, 0, 0),
		FILLED,
		LINE_8);
}
void Draw_White_block(Mat img, int pt0, int pt1) {
	Draw_White(img, pt0, pt1, pt0 + 1, pt1 + 1);
}
void Draw_Black_block(Mat img, int pt0, int pt1) {
	Draw_Black(img, pt0, pt1, pt0 + 1, pt1 + 1);
}
int Read() {
	int fill[2] = { 237,17 };
	FILE* rfp = NULL;
	errno_t rerr = 0;
	rerr = fopen_s(&rfp, "D:\\test.txt", "rb");
	if (rerr != 0) return 0; 
	FILE* wfp = NULL;
	errno_t werr = 0;
	werr = fopen_s(&wfp, "D:\\result.txt", "w");
	if (werr != 0) return 0;
	if (rfp)
	{
		fseek(rfp, 0, SEEK_END);
		int size = ftell(rfp);
		int current = 0;
		char * buf = new char[charSize];
		int nRead; 
		fseek(rfp, 0, SEEK_SET);
		while (current != size) {
			nRead = fread(buf, sizeof(char), 18, rfp);
			current += nRead;
			if (nRead > 0)
			{
				int record[20] = { 0 };
				//将读取的字符转为二进制流 存入result文件
				for (int i = 0; i < nRead; i++) record[i] = buf[i] - '\0';
				if (nRead < 18)
					for (int k = nRead; k < 18; k++)
						record[k] = fill[(k - nRead) % 2];
				
				int temp = chksum(record, 18);
				record[18] = temp >> 8;
				record[19] = temp & 0xff;
			
				for (int k = 0; k < 20; k++) {
					int bin = 0, j = 1;
					while (record[k]) {
						bin += j * (record[k] % 2);
						record[k] = record[k] / 2;
						j = j * 10;
					}
					fprintf(wfp, "%08d", bin);
				}
			}
		}
		fclose(rfp);
		fclose(wfp);
		delete buf;
		
	}
}
int chksum(void *dataptr, unsigned short len)
{
	unsigned long acc;
	unsigned short src;
	unsigned char *octetptr;
	acc = 0;
	octetptr = (unsigned char*)dataptr;

	while (len > 1)
	{
		src = (*octetptr) << 8;  //2个字节结合
		octetptr++;
		src |= (*octetptr);

		octetptr++;
		acc += src;
		len -= 2;
	}
	if (len > 0)            //最后单个字节，补0，组成2字节
	{
		src = (*octetptr) << 8;
		acc += src;
	}
	if ((acc & 0xffff0000UL) != 0)  //高16位 + 低16位
	{
		acc = (acc >> 16) + (acc & 0x0000ffffUL);
	}

	src = (unsigned short)acc;
	return src;
}
void Draw() {
	char fill[] = "1110110100010001";
	
	FILE* fp = NULL;
	errno_t err = 0;
	err = fopen_s(&fp, "D:\\result.txt", "r");
	if (err != 0) return;
	//nRead = fread(buf, sizeof(char), 18, rfp);

	fseek(fp, 0, SEEK_END);
	int size = ftell(fp);
	cout << "size" << size << endl;

	int count = 0;
	int length;
	int current = 1;
	Mat QR_Code = Mat::zeros(1080, 1920, CV_8UC3);
	fseek(fp, 0, SEEK_SET);

	while (true) {
		rectangle(QR_Code,
			Point(0, 0),
			Point(192, 108),
			Scalar(0, 0, 0),
			FILLED,
			LINE_8);
		Draw_Template(QR_Code);
		if (count % 2 == 0) {
			Draw_White_block(QR_Code, 0, 8);
			Draw_Black_block(QR_Code, 0, 9);
		}
		else {
			Draw_Black_block(QR_Code, 0, 8);
			Draw_White_block(QR_Code, 0, 9);
		}

		if (size - current > PicLength) length = PicLength;
		else length = size - current;
		int bin = 0, j = 1, k = 15;
		char lengthch[] = "0000000000000000";

		while (length) {
			bin += j * (length % 2);
			lengthch[k--] = (length % 2) + '0';
			length /= 2;
			j = j * 10;
		}
		for (int k = 0; k < 16; k++) {
			if (lengthch[k] == '1') Draw_White_block(QR_Code, 0, k + 10);
			else Draw_Black_block(QR_Code, 0, k + 10);
		}

		bool alert = true;
		int i = 0, chRe = 0;
		j = 26;
		char ch;
		while (alert)
		{
			if (!feof(fp) && current % PicLength != 0) ch = fgetc(fp);
			else if (feof(fp) && current % PicLength != 0) {
				ch = fill[chRe++];
				chRe %= 16;
			}
			if(alert) {
				if (ch == '1') Draw_White_block(QR_Code, i, j);
				else Draw_Black_block(QR_Code, i, j);
				current++;
				if (i % 2 == 0) {
					if (i >= 0 && i < 7) {
						if (j == 183) i++;
						else j++;
					}
					else if (i >= 7 && i < 102) {
						if (j == 191) i++;
						else j++;
					}
					else if (i >= 102 && i <= 107) {
						if (j == 185) j += 5;
						else if (j == 191) i++;
						else j++;
					}
					else if (i >= 100 && i < 108) {
						if (j == 186) j += 5;
						else if (j == 191) i++;
					}
				}
				else {
					if (i >= 0 && i < 7) {
						if (j == 8) i++;
						else j--;
					}
					if (i == 7) {
						if (j == 8) {
							i++;
							j = 0;
						}
						else j--;
					}
					else if (i > 7 && i < 99) {
						if (j == 0) i++;
						else j--;
					}
					else if (i == 99) {
						if (j == 0) {
							i++; j += 8;
						}
						else j--;
					}
					else if (i > 99 && i < 102) {
						if (j == 8) i++;
						else j--;
					}
					else if (i >= 102 && i <= 107) {
						if (j == 191) j -= 5;
						else if (j == 8) i++;
						else j--;
					}
					if (i == 107 && j == 16) {
						cout << "107,16" << endl;
						alert = false;
					}
				}
			}
		}
		cout << "alertfalse " << i << " " << j << " " << endl;
		cout << "currentsize " << current << " " << size << endl;
		string filename = "D:\\QR_Code\\QR_Code" + to_string(count) + ".jpg";
		cout << filename << endl;
		cv::imwrite(filename, QR_Code);
		if (current>=size) break;
		current++;
		count++;
	}
	system("pause");
	fclose(fp);
}