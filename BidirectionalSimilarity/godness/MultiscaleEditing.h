#ifndef MULTISCALE_EDITING_H
#define MULTISCALE_EDITING_H

#include <vector>
using namespace std;
#include "cv.h"
#include "highgui.h"
using namespace cv;

#include "ImageEnlarge.h"

IplImage* MultiscaleGivenInitialGuess(IplImage *srcImg, IplImage *initImg, int level = 0, int max_level = 5);

#endif