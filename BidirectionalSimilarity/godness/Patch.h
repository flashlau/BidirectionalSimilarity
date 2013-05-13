/**
* Code Author: TAKIA
* Time Generate: 7/1/2011, PM 11:08
* Finish Time: accomplished
*/

/** 
* New Version 1 :
* @ add public variable -- size
* @ add default constructor -- Patch()
* @ add contructor -- Patch(int _x, int_y, int _size, IplImage* srcImg)
*/

#ifndef PATCH_H
#define PATCH_H

#include "cv.h"
using namespace cv;

enum ColorDistanceModel
{
	GRAYSCALE,
	RGB_COLOR
};

//The pixel structure.
struct RgbPixel
{
	/*** RgbPixel Data ***/
	// A value of R channel, and the range between 0 ~ 255
	float r;
	// A value of G channel, and the range between 0 ~ 255
	float g;
	// A value of B channel, and the range between 0 ~ 255
	float b;

	/*** RgbPixel Methods ***/
	/**
	* The constructor initializes the value of 3 channels, including 3 channel r, g, b.
	* @param _r			the value of R channel
	* @param _g			the value of G channel
	* @param _b			the value of B channel
	*/
	RgbPixel(float _r = 0, float _g = 0, float _b = 0)
		:r(_r), g(_g), b(_b) {}
};

struct LabPixel
{
	/*** LabPixel Data ***/
	// A value of L channel, and the range between -1 ~ 1
	float l;
	// A value of A channel, and the range between -1 ~ 1
	float a;
	// A value of B channel, and the range between -1 ~ 1
	float b;

	/*** LabPixel Methods ***/
	/**
	* The constructor initializes the value of 3 channels, including 3 channel l, a, b.
	* @param _l			the value of L channel
	* @param _a			the value of A channel
	* @param _b			the value of B channel
	*/
	LabPixel(float _l = 0, float _a = 0, float _b = 0)
		:l(_l), a(_a), b(_b) {}
};

class Patch
{
public:
		
	/*** Patch Public Data ***/
	// The x coordinate of patch center.
	int x;
	// The y coordinate of patch center.
	int y;
	// The patch size. If size n means the n*n window of patch.
	int size;
	// The patch weight.
	float weight;
	// The source image which the patch belongs to.
	IplImage* srcImg;
	// The mask image which the patch belongs to. 0 means unknown pixels.
	IplImage* maskImg;

	// Distance model.
	ColorDistanceModel colorModel;

	/*** Patch Public Methods ***/
	/**
	* The default constructor for no parameters.
	*/
	Patch() : weight(1.0), srcImg(NULL), maskImg(NULL), colorModel(RGB_COLOR) {};

	/**
	* The constructor initializes the data of a patch, including the coordinate of center, size and source image.
	* @param _x				the x- coordinate of center
	* @param _y				the y- coordinate of center
	* @param _size			the size of patch
	* @param _srcImg		source image the patch belongs to
	* @param _maskImg		the mask image
	* @param _weight		the weight of the patch
	* @param _colorModel	the color distance model
	*/
	Patch(	int _x, int _y, int _size, IplImage* _srcImg, 
			IplImage* _maskImg = NULL, double _weight = 1.0,
			ColorDistanceModel _colorModel = RGB_COLOR);

	/**
	* The distance between two patches of different images.
	* @param p			the patch p of anther image
	*/
	float Distance(Patch p);

	/*** Patch Public Static Methods ***/
	/**
	* The distance between two patches of different images.
	* @param a			the patch a of one image
	* @param b			the patch b of another image
	*/
	static float Distance(Patch a, Patch b);

private:
	
	/**
	* Get a pixel value in this patch.
	* @param x			the coordinate x of pixel
	* @param y			the coordinate y of pixel
	*/
	RgbPixel GetRgbPixel(int x, int y);
	
	/**
	* Get a mask value in this patch.
	* @param x			the coordinate x of pixel
	* @param y			the coordinate y of pixel
	*/
	int GetMaskPixel(int x, int y);
};

#endif