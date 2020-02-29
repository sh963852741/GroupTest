#include <cstdio>
#include "detector.h"
#include "opencv2/opencv.hpp" //opencv 的头文件
#include "slicer.h"
#include "VedioGenerator.h"
using namespace std;

using namespace cv; //opencv 的命名空间


int main()
{

    VedioGenerator a;
    a.Read();
    a.Draw();
    
    
    return 0;
}