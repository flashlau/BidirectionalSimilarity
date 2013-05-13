//#ifndef CONSTRAINT_RANDOMIZED_NNSEARCH_H
//#define CONSTRAINT_RANDOMIZED_NNSEARCH_H
//
//#include "NNSearch.h"
//#include "Rect2D.h"
//
//class ConstraintedRandomizedNNSearch : public RandomizedNNSearch
//{
//public:
//	/*** ConstraintedRandomizedNNSearch Public Methods ***/
//	/**
//	* The constructor initializes the value of private variable.
//	* @private variable : NNF_exist
//	*/
//	ConstraintedRandomizedNNSearch();
//
//	/**
//	* The method search the most similar patch in target image according to the input patch in source image.
//	* @param *srcImg		the source image of input patch
//	* @param *dstImg		the target image which is searched to find most similar patch
//	* @param srcPatchPos	the patch position in source image
//	*/
//	virtual Patch SearchPatch(IplImage* srcImg, IplImage* dstImg, Patch srcPatchPos);
//
//protected:
//	/*** ConstraintedRandomizedNNSearch Private Methods ***/
//	/**
//	* The method add an area as a constrainted search region.
//	* @param rectRegion		the rectangle region
//	*/
//	void AddConstraintArea(Rect2D rectRegion, int srcW, int srcH, int dstW, int dstH);
//
//	/**
//	* The method initialize the Nearest Neighbor Field(NNF) by random sampling 
//	* @param *srcImg		the source image of input patch
//	* @param *dstImg		the target image which is searched to find most similar patch
//	*/
//	virtual void Initialization(IplImage* srcImg, IplImage* dstImg);
//	
//	/**
//	* The method propagate the NNF by search the right, up, down, left neighbor of the patch center.
//	* @param iteration		the nth of iteration of the optimization
//	* @param size			the patch size (ex: size = 7 is the 7*7 patch size)
//	* @param *srcImg		the source image of input patch
//	* @param *dstImg		the target image which is searched to find most similar patch
//	*/
//	virtual void Propagation(int iteration, int size, IplImage* srcImg, IplImage* dstImg);
//
//	/**
//	* The method re-assign the NNF value by search the radius of the patch center randomly.
//	* @param size			the patch size (ex: size = 7 is the 7*7 patch size)
//	* @param *srcImg		the source image of input patch
//	* @param *dstImg		the target image which is searched to find most similar patch
//	*/
//	virtual void RandomSearch(int size, IplImage* srcImg, IplImage* dstImg);
//
//	/*** ConstraintedRandomizedNNSearch Private Data ***/
//	// The mask that indicates the constrainted area. The value "false" refers to a
//	// constrainted pixel (NNF should omit the pixel.), while "true" refers to a normal pixel.
//	bool **constraintMask;
//	// The dst image size.
//	int dstW;
//	int dstH;
//};
//
//#endif