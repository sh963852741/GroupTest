#include"DataTranslation.h"

bool DataTranslator::WhetherInArea(int x, int y)
{
	if ((x > 7 && x < (dx - 8) && y >= 0 && y < dy) || (y > 7 && y < (dy - 8) && x >= 0 && x <= dx))
		return 1;
	else
	{
		if (x >= (dx - 8) && y >= (dy - 8) && x < dx&&y < dy)
		{
			if (x > (dx - 7) && x<(dx - 1) && y>(dy - 7) && y < (dy - 1))
				return 0;
			else
				return 1;
		}
		else
			return 0;
	}
}

int DataTranslator::getnextD()
{

	while (WhetherInArea(nowx, nowy) == 0)
	{
		if (nowy >= dy)
		{
			nowy--;
			nowx++;
			direction = -1;
		}
		else if (nowy < 0)
		{
			nowy++;
			nowx++;
			direction = 1;
		}
		else
		{
			nowy += direction;
		}
	}
	int j = DataArray[nowx][nowy];
	nowy += direction;
	return j;
}

short DataTranslator::InternetCheckSum(unsigned short*buf, int nwords)
{
	unsigned long sum;
	for (sum = 0; nwords > 1; )
	{
		sum += *(unsigned short*)buf++;
		nwords -= 2;
	}
	if (nwords > 0)
		sum += *(unsigned char*)buf;
	while (sum >> 16)
	{
		sum = (sum >> 16) + sum & 0xffff;
	}
	return ~sum;
}
void DataTranslator::Initialization(unsigned short**tes)
{
	DataArray = tes;
	if (DataArray[0][8] == 1)//读入两个bit
		Parity = 0;
	else
		Parity = 1;
	int i = 16;
	nowx = 0, nowy = 10;
	while (i != 0)//读入16个bit 对Bytnum初始化
	{
		int j = getnextD();
		//cout << j;
		if (j == 1)
			ByteNum = (ByteNum << 1) | 1;
		else ByteNum = ByteNum << 1;
		i--;
	}
}

void DataTranslator::OutPutData(unsigned char*data)
{
	ofstream outFIle("1.dat", ios::app | ios::binary);
	for (int i = 0; i < sum; i++)
	{
		cout << data[i] << " ";
		outFIle.write((const char*)&data[i], sizeof(char));
	}
	outFIle.close();
}

void DataTranslator::DateTranslate(unsigned short**tes)
{
	Initialization(tes);
	unsigned char* test = new unsigned char[ByteNum];//单张图片的数据
	unsigned char* DataCheckArray = new unsigned char[20];
	int CheckArrayIndex = 0;
	ByteNum = 20;
	int BitNums = ByteNum * 8;
	int SingleDataCodes;//每一组数据码长度；
	int SingleErrorCodes;//每一组纠错码长度
	int testindex = 0;
	while (BitNums != 0)
	{
		SingleDataCodes = 144;
		SingleErrorCodes = 16;
		CheckArrayIndex = 0;
		while (SingleDataCodes != 0)
		{
			for (int i = 0; i < 8; i++)
			{
				int j = getnextD();
				if (j == 1)
					DataCheckArray[CheckArrayIndex] = (DataCheckArray[CheckArrayIndex] << 1) | 1;
				else DataCheckArray[CheckArrayIndex] = DataCheckArray[CheckArrayIndex] << 1;
			}
			CheckArrayIndex++;
			BitNums -= 8;
			SingleDataCodes -= 8;
		}
		while (SingleErrorCodes != 0)
		{
			for (int i = 0; i < 8; i++)
			{
				int j = getnextD();
				if (j == 1)
					DataCheckArray[CheckArrayIndex] = (DataCheckArray[CheckArrayIndex] << 1) | 1;
				else DataCheckArray[CheckArrayIndex] = DataCheckArray[CheckArrayIndex] << 1;
			}
			CheckArrayIndex++;
			BitNums -= 8;
			SingleErrorCodes -= 8;
		}
		if (InternetCheckSum((unsigned short*)DataCheckArray, 20) == 1)//进行检验
		{
			for (int i = 0; i < 18; i++)
			{

				test[testindex] = DataCheckArray[i];

				testindex++;
			}
		}


	}
	sum = testindex;
	OutPutData(test);
}