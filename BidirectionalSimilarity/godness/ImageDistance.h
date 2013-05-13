/**
* Code Author: Unknown
* Time Generate: 7/2/2011, PM 09:21
*/

#ifndef IMAGEDISTANCE_H
#define IMAGEDISTANCE_H

#include "cv.h"
#include "Patch.h"
using namespace cv;

class ImageDistance
{
public:
	virtual float ComputeDistance(IplImage* img1, IplImage* img2) = 0;
};

class StereoImageDistance
{
public:
	virtual float ComputeDistance(IplImage* img1L, IplImage* img1R, IplImage* img2L, IplImage* img2R) = 0;
};

class BdsDistance : public ImageDistance
{
public:
	virtual float ComputeDistance(IplImage* img1, IplImage* img2);
	~BdsDistance();

private:
	float Coherence(IplImage* img1, IplImage* img2, int size);
	float Completeness(IplImage* img1, IplImage* img2, int size);
	void ReconstructImage();

	IplImage* labImg1;
	IplImage* labImg2;
};

class STDistace : public StereoImageDistance
{
public:
	virtual float ComputeDistance(IplImage* img1, IplImage* img2);
};

#endif