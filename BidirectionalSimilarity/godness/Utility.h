/**
* Code Author: Unknown
* Time Generate: 7/2/2011, PM 09:21
*/

#ifndef UTILITY_H
#define UTILITY_H

#include "cv.h"
#include "highgui.h"
#include "Patch.h"

using namespace cv;

float GrayDistance(RgbPixel p1, RgbPixel p2);

float GetMin(float n1, float n2);
float GetMax(float n1, float n2);
float GetMin(float n1, float n2, float n3);


/** Get/Set image pixel values. **/
CvScalar mycvGet2D_float3(IplImage* img, int y, int x);
void mycvSet2D_float3(IplImage* img, int y, int x, CvScalar scalar1);

CvScalar mycvGet2D_uchar1(IplImage* img, int y, int x);
void mycvSet2D_float1u(IplImage* img, int y, int x, CvScalar scalar1);

CvScalar mycvGet2D_float1(IplImage* img, int y, int x);

void mycvSet2D_float1(IplImage* img, int y, int x, CvScalar scalar1);


#endif