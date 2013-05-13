#include "PatchPair.h"

PatchPair::PatchPair()
{}

PatchPair::PatchPair(	int _x, int _y, int _size, IplImage* _srcImgL, IplImage* _srcImgR,
						IplImage* _maskImgL, IplImage* _maskImgR, double _weight,
						ColorDistanceModel _colorModel)
{
	patchL.x = _x;
	patchL.y = _y;
	patchL.size = _size;
	patchL.srcImg = _srcImgL;
	patchL.maskImg = _maskImgL;
	patchL.weight = _weight;
	patchL.colorModel = _colorModel;

	patchR.x = _x;
	patchR.y = _y;
	patchR.size = _size;
	patchR.srcImg = _srcImgR;
	patchR.maskImg = _maskImgR;
	patchR.weight = _weight;
	patchR.colorModel = _colorModel;
}

float PatchPair::Distance(PatchPair p)
{
	// Return the max one.
	float dL = this->patchL.Distance(p.patchL);
	float dR = this->patchR.Distance(p.patchR);

	return ((dL > dR) ? dL : dR);
}

float PatchPair::Distance(PatchPair a, PatchPair b)
{
	// Return the max one.
	float dL = a.patchL.Distance(b.patchL);
	float dR = a.patchR.Distance(b.patchR);

	return ((dL > dR) ? dL : dR);
}