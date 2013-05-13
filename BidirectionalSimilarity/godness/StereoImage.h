/**
* Code Author: TAKIA
* Time Generate: 4/17/2011, PM 12:38
*/

#ifndef STEREO_IMAGE_H
#define STEREO_IMAGE_H

#include "cv.h"
#include <math.h>

using namespace std;
using namespace cv;

class StereoImage
{
public:
	
	/*** StereoImage Public Data ***/
	// The left image.
	IplImage* imgL;
	// The right image.
	IplImage* imgR;
	
	// The height of stereo image.
	int height;

	// The width of stereo image.
	int width;
	
	/*** StereoImage Public Method ***/
	/**
	* The constructor initializes the data of a StereoImage, including height and width.
	* @param _imgL			the left image
	* @param _ImgR			the right image
	*/
	StereoImage(IplImage* _imgL, IplImage* _imgR);

	/**
	* The operator assign stereo image.
	* @param sImg			the StereoImage
	* @return				the result of StereoImage
	*/
	StereoImage operator=(StereoImage sImg);
};

#endif