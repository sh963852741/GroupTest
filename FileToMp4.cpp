#include "FileToMp4.h"
void FileToMp4::MakePictureSequence() {
	char readFile[20];
	FILE* readFp = NULL;
	errno_t readErr = 0;
	while (true) {
		cout << "Please enter the file you want to deliver" << endl
			<< R"(use '\\' to replace '\':)";
		cin >> readFile;
		readErr = fopen_s(&readFp, readFile, "rb");
		if (readErr == 0) break;
		cout << "Can not open the file" << endl;
	}

	FILE* writeFp = NULL;
	errno_t writeErr = 0;
	writeErr = fopen_s(&writeFp, "D:\\result.txt", "w");
	if (writeErr != 0) return;

	Trans(readFp, writeFp);

	int length, width, blockSize;
	while (true) {
		cout << "Please enter the length & width & length of block of your pic:";
		cin >> length >> width >> blockSize;
		if (length && width && blockSize) break;
	}

	Draw(length, width, blockSize);

	int frameRate;
	cout << "Please enter the frame rate you want:";
	cin >> frameRate;

	ffmpeg(frameRate);

	cout << endl << "Finish." << endl;
	system("pause");
}
void FileToMp4::Trans(FILE* readFp, FILE* writeFp) {
	int fill[2] = { 237,17 };//补齐码11101101 00010001

	if (readFp)
	{
		fseek(readFp, 0, SEEK_END);
		int size = ftell(readFp);
		int current = 0;
		char * buf = new char[100];
		int nRead;
		fseek(readFp, 0, SEEK_SET);
		while (current != size) {
			nRead = fread(buf, sizeof(char), 18, readFp);
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
					fprintf(writeFp, "%08d", bin);
				}
			}
		}
		fclose(readFp);
		fclose(writeFp);
		delete buf;
	}
}
int FileToMp4::chksum(void *dataptr, unsigned short len)
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
	return ~src;
}
void FileToMp4::Draw_Template(Mat img, int col, int row, int blockSize) {
	for (int i = 1; i < 6; i++) {
		//左上方框
		Draw_White_block(img, 1, i, blockSize);
		Draw_White_block(img, i, 1, blockSize);
		Draw_White_block(img, 5, i, blockSize);
		Draw_White_block(img, i, 5, blockSize);
		//右上方框
		Draw_White_block(img, 1, i + col - 7, blockSize);
		Draw_White_block(img, i, 1 + col - 7, blockSize);
		Draw_White_block(img, 5, i + col - 7, blockSize);
		Draw_White_block(img, i, col - 2, blockSize);
		//左下方框
		Draw_White_block(img, row - 6, i, blockSize);
		Draw_White_block(img, i + row - 7, 1, blockSize);
		Draw_White_block(img, row - 2, i, blockSize);
		Draw_White_block(img, i + row - 7, 5, blockSize);
	}
	for (int i = 0; i < 8; i++) {
		//左上两边
		Draw_White_block(img, i, 7, blockSize);
		Draw_White_block(img, 7, i, blockSize);
		//右上两边
		Draw_White_block(img, i, col - 8, blockSize);
		Draw_White_block(img, 7, i + col - 8, blockSize);
		//左下两边
		Draw_White_block(img, i + row - 8, 7, blockSize);
		Draw_White_block(img, row - 8, i, blockSize);
	}
	for (int i = 0; i < 3; i++) {
		//右下对齐点
		Draw_White_block(img, row - 5, col - 5 + i, blockSize);
		Draw_White_block(img, row - 3, col - 5 + i, blockSize);
		Draw_White_block(img, row - 5 + i, col - 5, blockSize);
		Draw_White_block(img, row - 5 + i, col - 3, blockSize);
	}

}
void FileToMp4::Draw_White_block(Mat img, int pt0, int pt1, int blockSize) {
	rectangle(img,
		Point(pt1 * blockSize + 0.5, pt0 * blockSize + 0.5),
		Point((pt1 + 1) * blockSize - 0.5, (pt0 + 1) * blockSize - 0.5),
		Scalar(256, 256, 256),
		FILLED, 1, 0);
}
void FileToMp4::Draw(int length, int width, int blockSize) {
	char fill[] = "1110110100010001";//补齐码

	FILE* fp = NULL;
	errno_t err = 0;
	err = fopen_s(&fp, "D:\\result.txt", "r");
	if (err != 0) return;

	fseek(fp, 0, SEEK_END);
	int size = ftell(fp);

	int count = 0;
	int capacity;
	int currentLoc = 1;
	Mat QR_Code = Mat::zeros(width, length, CV_8UC3);
	fseek(fp, 0, SEEK_SET);
	int col = length / 10, row = width / 10;
	int PicLength = col * row - 243;
	//243 = 8*8*3 + 5*5 + 2+16 + 8
	char lengthch[16];

	while (currentLoc < size) {
		rectangle(QR_Code,
			Point(0, 0),
			Point(length, width),
			Scalar(0, 0, 0),
			FILLED,
			LINE_8);
		Draw_Template(QR_Code, col, row, blockSize);
		if (count % 2) Draw_White_block(QR_Code, 0, 9, blockSize);
		else Draw_White_block(QR_Code, 0, 8, blockSize);


		if (size - currentLoc > PicLength) capacity = PicLength;
		else capacity = size - currentLoc;

		int bin = 0, j = 1, k = 15;
		for (int i = 0; i < 16; i++) lengthch[i] = '0';

		while (capacity) {
			bin += j * (capacity % 2);
			lengthch[k--] = (capacity % 2) + '0';
			capacity /= 2;
			j = j * 10;
		}
		for (int k = 0; k < 16; k++)
			if (lengthch[k] == '1') Draw_White_block(QR_Code, 0, k + 10, blockSize);

		bool alert = true;
		int i = 0, chRe = 0;
		j = 26;
		char ch;
		while (alert)
		{
			if (!feof(fp) && currentLoc % PicLength) ch = fgetc(fp);
			else if (feof(fp) && currentLoc % PicLength) {
				ch = fill[chRe++];
				chRe %= 16;
			}
			if (alert) {
				if (ch == '1') Draw_White_block(QR_Code, i, j, blockSize);
				currentLoc++;
				if (i % 2) {
					if (i >= 0 && i < 7) {
						if (j == 8) i++;
						else j--;
					}
					if (i == 7) {
						if (j == 8) {
							i++;
							j -= 8;
						}
						else j--;
					}
					else if (i > 7 && i < row - 9) {
						if (j == 0) i++;
						else j--;
					}
					else if (i == row - 9) {
						if (j == 0) {
							i++; j += 8;
						}
						else j--;
					}
					else if (i > row - 9 && i < row - 6) {
						if (j == 8) i++;
						else j--;
					}
					else if (i >= row - 6 && i < row - 1) {
						if (j == col - 1) j -= 6;
						else if (j == 8) i++;
						else j--;
					}
					else if (i == row - 1) {
						if (j == 16) alert = false;
						else j--;
					}
				}
				else {
					if (i >= 0 && i < 7) {
						if (j == col - 9) i++;
						else j++;
					}
					else if (i >= 7 && i < row - 6) {
						if (j == col - 1) i++;
						else j++;
					}
					else if (i >= row - 6 && i < row) {
						if (j == col - 7) j += 6;
						else if (j == col - 1) i++;
						else j++;
					}
				}
			}
		}
		string filename = "D:\\QR_Code\\QR_Code" + to_string(count) + ".jpg";
		cv::imwrite(filename, QR_Code);
		currentLoc++;
		count++;
	}
	fclose(fp);
}
void FileToMp4::ffmpeg(int frameRate) {
	string strCmd = "ffmpeg -f image2 -i D:\\QR_Code\\QR_Code%d.jpg -vcodec libx264 -r "
		+ to_string(frameRate) + " D:\\QR_Code\\QR_Code_" + to_string(frameRate) + ".mp4";
	char buf[10240] = { 0 };
	FILE *pf = NULL;
	if ((pf = _popen(strCmd.c_str(), "r")) == NULL)
		return;

	string strResult;
	while (fgets(buf, sizeof buf, pf))
		strResult += buf;

	_pclose(pf);

	int iSize = strResult.size();
	if (iSize > 0 && strResult[iSize - 1] == '\n')
	{
		strResult = strResult.substr(0, iSize - 1);
	}
}