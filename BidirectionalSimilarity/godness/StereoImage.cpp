#include "StereoImage.h"

StereoImage::StereoImage(IplImage* _imgL, IplImage* _imgR)
	:imgL(_imgL), imgR(_imgR)
{
	if(imgL != NULL)
	{
		height = imgL->height;
		width = imgL->width;
	}
}

StereoImage StereoImage::operator=(StereoImage sImg)
{
	imgL = sImg.imgL;
	imgR = sImg.imgR;

	return StereoImage(imgL, imgR);
}