/**
* 2009 DIP Final Project Code
* 
* The code is part of Fantasy Illustrator project.
* Author: ¯Î­§´@(Wong-Yu Ting), ±i©ú¦°(Ming-Hsu Chang), Ã¹¸t³Ç(Sheng-Jie Luo)
* Email: forestking@cmlab.csie.ntu.edu.tw
* 
* Any usage of the code is not allowed without the authorization by the authors.
*/

#ifndef WINBLE_MY_CV_FUNCTION_H
#define WINBLE_MY_CV_FUNCTION_H

#include "cv.h"
#include "highgui.h"
#include <iostream>

using namespace cv;
using namespace std;

CvScalar mycvGet2D_float3(IplImage* img, int y, int x);
void mycvSet2D_float3(IplImage* img, int y, int x, CvScalar scalar1);

CvScalar mycvGet2D_uchar1(IplImage* img, int y, int x);
void mycvSet2D_float1u(IplImage* img, int y, int x, CvScalar scalar1);

CvScalar mycvGet2D_float1(IplImage* img, int y, int x);

void mycvSet2D_float1(IplImage* img, int y, int x, CvScalar scalar1);

#endif // WINBLE_MY_CV_FUNCTION_H