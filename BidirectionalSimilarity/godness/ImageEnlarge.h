/**
* Code Author: SHENG-JIE
* Time Generate: 8/27/2011, PM 09:21
*/

#ifndef IMAGE_ENLARGE_H
#define IMAGE_ENLARGE_H

#include "cv.h"
#include "Patch.h"
using namespace cv;
#include <vector>
using namespace std;
#include "Vector2D.h"
#include "Polygon2D.h"

class ImageEnlarge
{
public:
	/**
	* Constructor.
	* @param _srcImg		the source image
	* @param _initDstImg	the initial target image
	* @param _patchSize		the patch size
	*/
	ImageEnlarge(IplImage* _srcImg, IplImage* _initDstImg = NULL, int _height = 0, int _width = 0, int _patchSize = 7);

	/**
	* Destructor.
	*/
	~ImageEnlarge();

	/**
	* Construct the result image iteratively.
	*/
	void Construct();

	/**
	* Add a hard constraint.
	* @param constraint		the new hard constraint
	*/
	//void AddHardConstraint(HardConstraint constraint);

	/**
	* Get the result image.
	* @return		the result image
	*/
	IplImage* GetResultImage();

private:
	/**
	* Initialize the destination image.
	*/
	void InitializeDst();

	/**
	* Construct the nn-field.
	*/
	void Src2DstNNFConstruct();
	void Dst2SrcNNFConstruct();

	/**
	* Update the destination image using voting scheme.
	*/
	void UpdateDst();

	/**
	* Compute the completeness term and coherence term.
	*/
	void Completeness();
	void Coherence();

private:

	// The coherence and completeness term map.
	RgbPixel** cohereTermForPixel;
	RgbPixel** compleTermForPixel;

	// The pixel number that contribute to the coherence and completeness term.
	int** coherePatchNum;
	int** complePatchNum;

	// The source image and target image.
	IplImage* srcImg;
	IplImage* dstImg;

	// The nn-field of source to target and target to source.
	Vector2D** nnfS2T;
	Vector2D** nnfT2S;

	// The user-wanted final-height and final-width
	int height;
	int width;

	// The patch size.
	int patchSize;

};

#endif