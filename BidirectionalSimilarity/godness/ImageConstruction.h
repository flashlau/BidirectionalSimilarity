/**
* Code Author: Unknown
* Time Generate: 8/2/2011, PM 09:21
*/

#ifndef IMAGECONSTRUCTION_H
#define IMAGECONSTRUCTION_H

#include "cv.h"
#include "Vector2D.h"
using namespace cv;

class ImageConstruction
{
public:
	virtual void Construct(int finalWidth, int finalHeight) = 0;

protected:
	virtual void InitializeDst(int finalWidth, int finalHeight) {};
	virtual void Src2DstNNFConstruct() {};
	virtual void Dst2SrcNNFConstruct() {};
	virtual void UpdateDst() {};

	virtual void Completeness() {};
	virtual void Coherence() {};

	IplImage* srcImg;
	IplImage* dstImg;

	Vector2D** nnfS2T;
	Vector2D** nnfT2S;

	int patchSize;
};

#endif