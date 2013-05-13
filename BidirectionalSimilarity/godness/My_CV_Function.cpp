/**
* 2009 DIP Final Project Code
* 
* The code is part of Fantasy Illustrator project.
* Author: ¯Î­§´@(Wong-Yu Ting), ±i©ú¦°(Ming-Hsu Chang), Ã¹¸t³Ç(Sheng-Jie Luo)
* Email: forestking@cmlab.csie.ntu.edu.tw
* 
* Any usage of the code is not allowed without the authorization by the authors.
*/

#include "My_CV_Function.h"

CvScalar mycvGet2D_float3(IplImage* img, int y, int x)
{
	float* temp_ptr = &((float*)(img->imageData + img->widthStep*y))[x*3];
	CvScalar scalar1 = cvScalar(temp_ptr[0], temp_ptr[1], temp_ptr[2], 0);
	//(float)Image1->imageData[idx0*Image1->widthStep+idx1+2]
	return scalar1;
}

void mycvSet2D_float3(IplImage* img, int y, int x, CvScalar scalar1)
{
	float* temp_ptr = &((float*)(img->imageData + img->widthStep*y))[x*3];
	temp_ptr[0] = (float)scalar1.val[0];
	temp_ptr[1] = (float)scalar1.val[1];
	temp_ptr[2] = (float)scalar1.val[2];
}

CvScalar mycvGet2D_uchar1(IplImage* img, int y, int x)
{
	uchar* temp_ptr = &((uchar*)(img->imageData + img->widthStep*y))[x];
	CvScalar scalar1 = cvRealScalar(temp_ptr[0]);
	//(float)Image1->imageData[idx0*Image1->widthStep+idx1+2]
	return scalar1;
}

void mycvSet2D_float1u(IplImage* img, int y, int x, CvScalar scalar1)
{
	uchar* temp_ptr = &((uchar*)(img->imageData + img->widthStep*y))[x];
	temp_ptr[0] = (uchar)scalar1.val[0];
}

CvScalar mycvGet2D_float1(IplImage* img, int y, int x)
{
	float* temp_ptr = &((float*)(img->imageData + img->widthStep*y))[x];
	CvScalar scalar1 = cvRealScalar(temp_ptr[0]);
	//(float)Image1->imageData[idx0*Image1->widthStep+idx1+2]
	return scalar1;
}

void mycvSet2D_float1(IplImage* img, int y, int x, CvScalar scalar1)
{
	float* temp_ptr = &((float*)(img->imageData + img->widthStep*y))[x];
	temp_ptr[0] = (float)scalar1.val[0];
}