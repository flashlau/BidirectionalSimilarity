/**
* 2009 DIP Final Project Code
* 
* The code is part of Fantasy Illustrator project.
* Author: ¯Î­§´@(Wong-Yu Ting), ±i©ú¦°(Ming-Hsu Chang), Ã¹¸t³Ç(Sheng-Jie Luo)
* Email: forestking@cmlab.csie.ntu.edu.tw
* 
* Any usage of the code is not allowed without the authorization by the authors.
*/

#ifndef _UNINCLUDE
#define _UNINCLUDE

#include "cv.h"
#include "highgui.h"
#include <iostream>
#include <math.h>
#include "Utility.h"

#endif
 
using namespace cv;
using namespace std;

IplImage* color_transfer(IplImage* img_src, IplImage* img_tar);
IplImage* color_transfer2(IplImage* img_src, IplImage* img_tar);
IplImage* myBGR2Lab(IplImage* img_src);
IplImage* myLab2BGR(IplImage* img_src);