#include "MultiscaleEditing.h"

IplImage* MultiscaleGivenInitialGuess(IplImage *srcImg, IplImage *initImg, int level, int max_level)
{
	// The smallest level.
	if(level == max_level)
	{
		// Run process.
		ImageEnlarge process(srcImg, initImg, initImg->height, initImg->width, 7);
		process.Construct();
		IplImage* result = cvCloneImage(process.GetResultImage());

		// Output the result.
		char output_name[100];
		sprintf(output_name, "Result_%d.png", level);
		cvSaveImage(output_name, result);

		return result;
	}

	// Scale down the source image.
	IplImage* sSrcImg = cvCreateImage(cvSize(srcImg->width/2, srcImg->height/2), srcImg->depth, srcImg->nChannels);
	cvResize(srcImg, sSrcImg);

	// Scale down the initial guess image.
	IplImage* sInitImg = cvCreateImage(cvSize(initImg->width/2, initImg->height/2), initImg->depth, initImg->nChannels);
	cvResize(initImg, sInitImg);

	// Get the smaller result.
	IplImage* sResultImg = MultiscaleGivenInitialGuess(sSrcImg, sInitImg, level+1, max_level);
	IplImage* lTmpImg = cvCreateImage(cvSize(sResultImg->width*2, sResultImg->height*2), sResultImg->depth, sResultImg->nChannels);
	for(int h = 0; h < sResultImg->height; h++)
	{
		for(int w = 0; w < sResultImg->width; w++)
		{
			CvScalar v = cvGet2D(sResultImg, h, w);
			cvSet2D(lTmpImg, h*2, w*2, v);
			cvSet2D(lTmpImg, h*2+1, w*2, v);
			cvSet2D(lTmpImg, h*2, w*2+1, v);
			cvSet2D(lTmpImg, h*2+1, w*2+1, v);
		}
	}
	// Resize (enlarge) the smaller result to the correct size.
	IplImage* lResultImg = cvCreateImage(cvSize(initImg->width, initImg->height), sResultImg->depth, sResultImg->nChannels);
	cvResize(lTmpImg, lResultImg);


	ImageEnlarge process(srcImg, lResultImg, lResultImg->height, lResultImg->width);
	process.Construct();
	IplImage* result = cvCloneImage(process.GetResultImage());

	printf("MultiScaleGivenInitialGuess : done....\n");

	char name[100];
	sprintf(name, "Result_%d.png", level);
	cvSaveImage(name, result);
	return result;
}

