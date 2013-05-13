/**
* Code Author: Sheng-Jie
* Time Generate: 9/1/2011, AM 00:44
* Finish Time: accomplished
*/

#ifndef PATCHPAIR_H
#define PATCHPAIR_H

#include "cv.h"
using namespace cv;
#include "Patch.h"

class PatchPair
{
public:
	// The left patch.
	Patch patchL;
	// The right patch.
	Patch patchR;

	/*** PatchPair Public Methods ***/
	/**
	* The default constructor for no parameters.
	*/
	PatchPair();

	/**
	* The constructor initializes the data of a patch, including the coordinate of center, size and source image.
	* @param _x				the x- coordinate of center
	* @param _y				the y- coordinate of center
	* @param _size			the size of patch
	* @param _srcImgL		source image the patch L belongs to
	* @param _srcImgR		source image the patch R belongs to
	* @param _maskImgL		the mask image the patch L belongs to
	* @param _maskImgR		the mask image the patch R belongs to
	* @param _weight		the weight of the patch
	* @param _colorModel	the color distance model
	*/
	PatchPair(	int _x, int _y, int _size, IplImage* _srcImgL, IplImage* _srcImgR,
				IplImage* _maskImgL = NULL, IplImage* _maskImgR = NULL, double _weight = 1.0,
				ColorDistanceModel _colorModel = RGB_COLOR);


	/**
	* The distance between two patch pairs of different images.
	* @param p			the patch pair p of anther image
	*/
	float Distance(PatchPair p);

/*** Patch Public Static Methods ***/
	/**
	* The distance between two patch pairs of different images.
	* @param a			the patch pair a of one image
	* @param b			the patch pair b of another image
	*/
	static float Distance(PatchPair a, PatchPair b);
};


#endif