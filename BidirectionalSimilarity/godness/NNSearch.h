/**
* Code Author: Unknown
* Time Generate: 7/2/2011, PM 09:21
*/

#ifndef NNSEARCH_H
#define NNSEARCH_H

#include "Patch.h"
#include "PatchPair.h"
#include "Vector2D.h"
#include "Rect2D.h"
#include "Line2D.h"
#include "cv.h"
using namespace cv;

/*Base Class*/
class NNSearch
{
public:
	/**
	* Constructor initializes the source image, target image, and the mask image.
	* @param _srcImg		the source image
	* @param _dstImg		the target image
	*/
	NNSearch(IplImage* _srcImg, IplImage* _dstImg);

	/**
	* Given a patch of the source image, find the nearest neighbor patch in the target image.
	* @param srcPatch	the patch from source image
	*/
	virtual Patch SearchPatch(Patch &srcPatch) = 0;

	/**
	* Add a constraint area to prevent searching there.
	* @param constraint		the constraint area
	*/
	void AddSearchConstraint(Rect2D &constraint, bool searchable = false);
	void AddSearchConstraint(Line2D &constraint, bool searchable = false);
	void SetConstraintMaskZero();

protected:
	/**
	* The function check if the searched target patch is valid.
	* @param dstPatch		the patch from target image
	* @param				the patch is valid or not
	*/
	virtual bool IsSearchable(Patch &dstPatch);

protected:
	// The source and target images.
	IplImage* srcImg;
	IplImage* dstImg;

	// The mask image indicates the search space.
	// Therefore, mask image size is the same as target image.
	// The value 1 means that the pixel is searchable.
	// The value 0 means that the pixel is not searchable (i.e., patch can't cover the pixel).
	IplImage* maskImg;
};

class StereoNNSearch
{
public:

	/**
	* Constructor initializes the stereo source image, stereo target image, and the mask image.
	* @param _srcImgL		the left source image
	* @param _srcImgR       the right source image
	* @param _dstImgL		the left target image
	* @param _dstImgR		the right target image
	*/
	StereoNNSearch(IplImage* _srcImgL, IplImage* _srcImgR, IplImage* _dstImgL, IplImage* _dstImgR);

	/**
	* Given a pair of patch of the source image, find the nearest neighbor patch pair in the target image.
	* @param srcPatchPairPos	the patch pair from source image
	*/
	virtual PatchPair SearchPatch(PatchPair &srcPatchPairPos) = 0;

protected:

	/**
	* The function check if the searched target patch is valid.
	* @param dstPatch		the patch from target image
	* @param				the patch is valid or not
	*/
	virtual bool IsSearchable(PatchPair &dstPatchPair);

protected:

	// The stereo source and target images.
	IplImage* srcImgL, *srcImgR;
	IplImage* dstImgL, *dstImgR;

	// The mask image indicates the search space.
	// Therefore, mask image size is the same as target image.
	// The value 1 means that the pixel is searchable.
	// The value 0 means that the pixel is not searchable (i.e., patch can't cover the pixel).
	IplImage* maskImgL, *maskImgR;
};

class StereoBruteForceNNSearch : StereoNNSearch
{
public:

	/*** StereoBruteForceNNSearch Public Methods ***/
	/**
	* Constructor initializes the source image, target image.
	* @param _srcImgL		the left source image
	* @param _srcImgR       the right source image
	* @param _dstImgL       the left target image
	* @param _dstImgR       the right target image
	*/
	StereoBruteForceNNSearch(IplImage* _srcImgL, IplImage* _srcImgR, IplImage* _dstImgL, IplImage* _dstImgR)
		:StereoNNSearch(_srcImgL, _srcImgR, _dstImgL, _dstImgR)
	{}

	/**
	* Given a pair of patch of the source image, find the nearest neighbor patch pair in the target image.
	* @param srcPatchPairPos	the patch pair from source image
	*/
	virtual PatchPair SearchPatch(PatchPair &srcPatchPair);
};

class StereoRandomizedNNSearch : StereoNNSearch
{
public:
	
	StereoRandomizedNNSearch(IplImage* _srcImgL, IplImage* _srcImgR, IplImage* _dstImgL, IplImage* _dstImgR)
		: StereoNNSearch(_srcImgL, _srcImgR, _dstImgL, _dstImgR)
	{NNF_exist = false;}

	virtual PatchPair SearchPatch(PatchPair &srcPatchPair);

protected:

	/*** Stereo RandomizedNNSearch Private Methods ***/
	/**
	* The method initialize the Nearest Neighbor Field(NNF) by random sampling 
	* @param size			the patch size (ex: size = 7 is the 7*7 patch size)
	*/
	virtual void Initialization(int size);

	/**
	* The method propagate the NNF by search the right, up, down, left neighbor of the patch center.
	* @param iteration		the nth of iteration of the optimization
	* @param size			the patch size (ex: size = 7 is the 7*7 patch size)
	*/
	virtual void Propagation(int iteration, int size, PatchPair &srcPatchPair);

	/**
	* The method re-assign the NNF value by search the radius of the patch center randomly.
	* @param size			the patch size (ex: size = 7 is the 7*7 patch size)
	*/
	virtual void RandomSearch(int size, PatchPair &srcPatchPair);

	/**
	* The method visualize the NNF by transfering the Lab Image to RGB Image.
	*/
	void Visualization();

	/*** RandomizedNNSearch Private Data ***/
	//A height*width size of map of source image, which is stored NNF from source patch to target patch.
	Vector2D** srcImgNNF;
	//A boolean value which display the NNF exist or not.
	bool NNF_exist;
};

/*Inherented Class*/
class BruteForceNNSearch : public NNSearch
{
public:
	/*** BruteForceNNSearch Public Methods ***/
	/**
	* Constructor initializes the source image, target image.
	* @param _srcImg		the source image
	* @param _dstImg		the target image
	*/
	BruteForceNNSearch(IplImage* _srcImg, IplImage* _dstImg)
		: NNSearch(_srcImg, _dstImg)
	{}

	/**
	* Given a patch of the source image, find the nearest neighbor patch in the target image using the brute force method.
	* @param srcPatch	the patch from source image
	*/
	virtual Patch SearchPatch(Patch &srcPatch);

};

class ConstraintedBruteForceNNSearch : public BruteForceNNSearch
{
public:
	/*** ConstraintedBruteForceNNSearch Public Methods ***/
	/**
	* Constructor initializes the source image, target image, and a default mask image.
	* @param _srcImg		the source image
	* @param _dstImg		the target image
	*/
	ConstraintedBruteForceNNSearch(IplImage* _srcImg, IplImage* _dstImg)
		: BruteForceNNSearch(_srcImg, _dstImg)
	{}

protected:
	/**
	* The function check if the searched target patch is valid.
	* @param dstPatch		the patch from target image
	* @param				the patch is valid or not
	*/
	virtual bool IsSearchable(Patch &dstPatch);
};

class RandomizedNNSearch : public NNSearch	  
{
public:
	/*** RandomizedNNSearch Public Methods ***/
	/**
	* Constructor initializes the source image, target image.
	* @param _srcImg		the source image
	* @param _dstImg		the target image
	*/
	RandomizedNNSearch(IplImage* _srcImg, IplImage* _dstImg);

	/**
	* Given a patch of the source image, find the nearest neighbor patch in the target image using the randomized method.
	* @param srcPatch	the patch from source image
	*/
	virtual Patch SearchPatch(Patch &srcPatch);

protected:

	/*** RandomizedNNSearch Private Methods ***/
	/**
	* The method initialize the Nearest Neighbor Field(NNF) by random sampling 
	* @param size			the patch size (ex: size = 7 is the 7*7 patch size)
	*/
	virtual void Initialization(int size);

	/**
	* The method propagate the NNF by search the right, up, down, left neighbor of the patch center.
	* @param iteration		the nth of iteration of the optimization
	* @param size			the patch size (ex: size = 7 is the 7*7 patch size)
	*/
	virtual void Propagation(int iteration, int size, Patch &srcPatch);

	/**
	* The method re-assign the NNF value by search the radius of the patch center randomly.
	* @param size			the patch size (ex: size = 7 is the 7*7 patch size)
	*/
	virtual void RandomSearch(int size, Patch &srcPatch);

	/**
	* The method visualize the NNF by transfering the Lab Image to RGB Image.
	*/
	void Visualization();

	/*** RandomizedNNSearch Private Data ***/
	//A height*width size of map of source image, which is stored NNF from source patch to target patch.
	Vector2D** srcImgNNF;
	//A boolean value which display the NNF exist or not.
	bool NNF_exist;
};

class ConstraintedRandomizedNNSearch : public RandomizedNNSearch	  /*Accomplished*/
{
public:
	/*** ConstraintedRandomizedNNSearch Public Methods ***/
	/**
	* Constructor initializes the source image, target image, and a default mask image.
	* @param _srcImg		the source image
	* @param _dstImg		the target image
	*/
	ConstraintedRandomizedNNSearch(IplImage* _srcImg, IplImage* _dstImg)
		: RandomizedNNSearch(_srcImg, _dstImg)
	{}

protected:
	/**
	* The function check if the searched target patch is valid.
	* @param dstPatch		the patch from target image
	* @param				the patch is valid or not
	*/
	virtual bool IsSearchable(Patch &dstPatch);
};

///////////////////////////////////////////////////////////////////////////////////////


class CshNNSearch : public NNSearch
{
public:
	virtual Patch SearchPatch(IplImage* srcImg, IplImage* dstImg, Patch &srcPatchPos);
};

#endif

