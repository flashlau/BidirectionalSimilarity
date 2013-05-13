#include "Utility.h"

float GrayDistance(RgbPixel p1, RgbPixel p2)
{
	float dr = p1.r - p2.r;
	float dg = p1.g - p2.g;
	float db = p1.b - p2.b;

	return abs(0.3f*dr + 0.59f*dg + 0.11f*db);
}

float GetMin(float n1, float n2)
{
	float min;
	if(n1 < n2)	min = n1;
	else		min = n2;
	return min;
}

float GetMax(float n1, float n2)
{
	float max;
	if(n1 > n2)	max = n1;
	else		max = n2;
	return max;
}

float GetMin(float n1, float n2, float n3)
{
	float min;
	if(n1 < n2)	min = n1;
	else		min = n2;

	if(n3 < min)	min = n3;

	return min;
}

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