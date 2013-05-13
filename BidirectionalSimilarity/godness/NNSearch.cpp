#include "NNSearch.h"
#include "MathTool.h"
#include "Color_Transfer.h"
#include "Point2D.h"
#include <time.h>
#include <algorithm>

NNSearch::NNSearch(IplImage* _srcImg, IplImage* _dstImg)
	: srcImg(_srcImg), dstImg(_dstImg)
{
	maskImg = cvCreateImage(cvSize(dstImg->width, dstImg->height), IPL_DEPTH_8U, 1);
	for(int i = 0; i < dstImg->width; i++)
	{
		for(int j = 0; j < dstImg->height; j++)
		{
			cvSet2D(maskImg, j, i, cvScalar(1));
		}
	}
}

void NNSearch::AddSearchConstraint(Rect2D &constraint, bool searchable)
{
	int xMin = (constraint.xMin < 0) ? 0 : constraint.xMin;
	int xMax = (constraint.xMax >= dstImg->width) ? dstImg->width-1 : constraint.xMax;
	int yMin = (constraint.yMin < 0) ? 0 : constraint.yMin;
	int yMax = (constraint.yMax >= dstImg->height) ? dstImg->height-1 : constraint.yMax;

	for(int x = xMin; x <= xMax; x++)
	{
		for(int y = yMin; y <= yMax; y++)
		{
			if(searchable)	cvSet2D(maskImg, y, x, cvScalar(1));
			else			cvSet2D(maskImg, y, x, cvScalar(0));
		}
	}
}

void NNSearch::AddSearchConstraint(Line2D &constraint, bool searchable)
{
	for(int x = 0; x < maskImg->width; x++)
	{
		for(int y = 0; y < maskImg->height; y++)
		{
			if(constraint.PointIsOnLine(Point2D(x, y)))
			{
				if(searchable)	cvSet2D(maskImg, y, x, cvScalar(1));
				else			cvSet2D(maskImg, y, x, cvScalar(0));
			}
		}
	}
}

void NNSearch::SetConstraintMaskZero()
{
	cvZero(maskImg);
}

bool NNSearch::IsSearchable(Patch &dstPatch)
{
	return true;
}

//////////////////////////Start of Brute Force//////////////////////////////////////
Patch BruteForceNNSearch::SearchPatch(Patch &srcPatch)
{
	// Get the patch size.
	int size = srcPatch.size;
	// The distance information.
	float min_distance, distance;

	// The result nn patch.
	Patch result(0, 0, size, dstImg);
	min_distance = HUGE_VAL;

	// Brute force method to find the nn patch.
	for(int h = 0 ; h < dstImg->height ; h++)
	{
		for(int w = 0 ; w < dstImg->width ; w++)
		{
			// A patch on the target image.
			Patch dst(w, h, size, dstImg);

			// This is for constraint NNSearch.
			if(!IsSearchable(dst)) continue;

			// Compute the patch distance.
			distance = srcPatch.Distance(dst);

			if( distance < min_distance )
			{
				min_distance = distance;
				result.x = w;
				result.y = h;
			}
		}
	}

	return result;
}

bool ConstraintedBruteForceNNSearch::IsSearchable(Patch &dstPatch)
{
	// Get the patch information.
	int size = dstPatch.size;
	int x = dstPatch.x;
	int y = dstPatch.y;

	bool isSearchable = true;
	for(int h = -size/2 ; h <= size/2; h++)
	{
		for(int w = -size/2 ; w <= size/2; w++)
		{
			// Get the current checking point.
			int curX = x + w;
			int curY = y + h;

			if(curX < 0 || curY < 0 || curX >= dstImg->width || curY >= dstImg->height)
				continue;

			// Check if the patch overlaps the unmarked pixel.
			CvScalar v = cvGet2D(maskImg, curY, curX);
			if(v.val[0] == 0) isSearchable = false;
		}
	}

	return isSearchable;
}//////////////////////////End of Brute Force//////////////////////////////////////

//////////////////////////Start of Randomized Search//////////////////////////////
RandomizedNNSearch::RandomizedNNSearch(IplImage* _srcImg, IplImage* _dstImg)
	: NNSearch(_srcImg, _dstImg)
{
	NNF_exist = false;
}

Patch RandomizedNNSearch::SearchPatch(Patch &srcPatch)
{
	int iteration = 1, size, srcH, srcW, dstH, dstW, outputX, outputY;
	
	// Get the patch size.
	size = srcPatch.size;
	// Get the image size.
	srcH = srcImg->height;
	srcW = srcImg->width;
	dstH = dstImg->height;
	dstW = dstImg->width;

	// If NNF is not constructed, construct it!! (Search a patch for the first time.)
	if( NNF_exist == false)
	{
		// Step 1 : Initialize the NNF by randomly sampling.
		Initialization(size);

		//Visualization();
		//ReconstructImage(srcImg, dstImg, size);
		
		while(iteration <= 5)
		{
			printf("In Iteration : %d\n", iteration);
			// Step 2 : Propagate the NN vector.
			Propagation(iteration, size, srcPatch);

			// Step 3 : Randomly refine the NN vector in multiple radius.
			RandomSearch(size, srcPatch);

			//Visualization();
			//ReconstructImage(srcImg, dstImg, size);

			iteration++;
		}

		for(int h = 0 ; h < srcH ; h++)
		{
			for(int w = 0 ; w < srcW ; w++)
			{
				int dst_h = h + (int)srcImgNNF[h][w].y;
				int dst_w = w + (int)srcImgNNF[h][w].x;

				if(dst_h > dstH-1 || dst_h < 0)
				{
					printf("y coordinate is out of bound of height of target image\n");
					Patch p;

					return p;
				}

				if(dst_w > dstW-1 || dst_w < 0)
				{
					printf("x coordinate is out of bound of width of target image\n");
					Patch p;

					return p;
				}
			}
		}

//		ReconstructImage(srcImg, dstImg, size);
		NNF_exist = true;

		outputX = srcPatch.x + (int)srcImgNNF[srcPatch.y][srcPatch.x].x;
		outputY = srcPatch.y + (int)srcImgNNF[srcPatch.y][srcPatch.x].y;
	}
	else
	{
		outputX = srcPatch.x + (int)srcImgNNF[srcPatch.y][srcPatch.x].x;
		outputY = srcPatch.y + (int)srcImgNNF[srcPatch.y][srcPatch.x].y;
	}

	Patch output(outputX, outputY, size, dstImg);

	return output;
}

void RandomizedNNSearch::Initialization(int size)
{
	int srcW, srcH, dstW, dstH;
	int h2, w2;
	
	srcW = srcImg->width;
	srcH = srcImg->height;
	dstW = dstImg->width;
	dstH = dstImg->height;

	srcImgNNF = new Vector2D*[srcH];

	for(int h = 0 ; h < srcH ; h++)
	{
		srcImgNNF[h] = new Vector2D[srcW];
	}

	srand(time(NULL));

	for(int h1 = 0 ; h1 < srcH ; h1++)
	{
		for(int w1 = 0 ; w1 < srcW ; w1++)
		{
			// For constrainted search space used.
			do
			{
				w2 = rand() % dstW;
				h2 = rand() % dstH;

			}
			while(! IsSearchable(Patch(w2, h2, size, dstImg)));
			
			srcImgNNF[h1][w1].x = w2 - w1;
			srcImgNNF[h1][w1].y = h2 - h1;
		}
	}
}

void RandomizedNNSearch::Visualization()
{
	/* Visualization, and a and b's range are between -128 ~ +127 */
	float minH = HUGE_VAL, minW = HUGE_VAL, maxH = -HUGE_VAL, maxW = -HUGE_VAL;
	float ratioW, ratioH;
	int srcH, srcW, dstH, dstW;
	
	srcH = srcImg->height;
	srcW = srcImg->width;
	dstH = dstImg->height;
	dstW = dstImg->width;

	minW = -srcW;
	minH = -srcH;
	maxW = dstW;
	maxH = dstH;
	
	ratioW = 1.0f/(float)(maxW - minW);
	ratioH = 1.0f/(float)(maxH - minH);

	IplImage* img, *visImg;
	img = cvCreateImage(cvGetSize(srcImg), IPL_DEPTH_32F, srcImg->nChannels);

	for(int h = 0 ; h < srcH ; h++)
	{
		for(int w = 0 ; w < srcW ; w++)
		{
			float channel1 = (srcImgNNF[h][w].x - minW)*ratioW*2 - 1.0f;
			float channel2 = (srcImgNNF[h][w].y - minH)*ratioH*2 - 1.0f;

			((float *)(img->imageData + h*img->widthStep))[w*img->nChannels + 0] = 0.2;
			((float *)(img->imageData + h*img->widthStep))[w*img->nChannels + 1] = channel1;
			((float *)(img->imageData + h*img->widthStep))[w*img->nChannels + 2] = channel2;
		}
	}

	visImg = myLab2BGR(img);
	
	cvNamedWindow("Visualize");
	cvShowImage("Visualize", visImg);
	cvWaitKey();
	cvReleaseImage(&img);
	cvReleaseImage(&visImg);
}

void RandomizedNNSearch::Propagation(int iteration, int size, Patch &srcPatch)
{
	int srcH, srcW, dstH, dstW;
	int src_h, src_w, dst_h, dst_w;
	float d_mid, d_up, d_left, d_right, d_down;
	
	srcH = srcImg->height;
	srcW = srcImg->width;
	dstH = dstImg->height;
	dstW = dstImg->width;

	if(iteration % 2 == 0)
	{
		for(int h = 0 ; h < srcH ; h++)
		{
			for(int w = 0 ; w < srcW ; w++)
			{
				src_w = w;
				src_h = h;
				dst_w = w + (int)srcImgNNF[h][w].x;
				dst_h = h + (int)srcImgNNF[h][w].y;

				// Compute self distance D0.
				Patch p_src_mid(src_w, src_h, size, srcImg, srcPatch.maskImg, srcPatch.weight, srcPatch.colorModel);
				Patch p_dst_mid(dst_w, dst_h, size, dstImg);
				d_mid = Patch::Distance(p_src_mid, p_dst_mid);
			
				// Compute up-pixel distance D1.
				if( h-1 < 0 || h-1 >= srcH)
				{
					d_up = HUGE_VAL;
				}
				else
				{
					dst_w = w + (int)srcImgNNF[h-1][w].x;
					dst_h = h + (int)srcImgNNF[h-1][w].y;
					
					Patch p_src_up(src_w, src_h, size, srcImg, srcPatch.maskImg, srcPatch.weight, srcPatch.colorModel);
					Patch p_dst_up(dst_w, dst_h, size, dstImg);
					d_up = Patch::Distance(p_src_up, p_dst_up);
					
				}

				// Compute left-pixel distance D2.
				if( w-1 < 0 || w-1 >= srcW)
				{
					d_left = HUGE_VAL;
				}
				else
				{
					dst_w = w + (int)srcImgNNF[h][w-1].x;
					dst_h = h + (int)srcImgNNF[h][w-1].y;

					Patch p_src_left(src_w, src_h, size, srcImg, srcPatch.maskImg, srcPatch.weight, srcPatch.colorModel);
					Patch p_dst_left(dst_w, dst_h, size, dstImg);
					d_left = Patch::Distance(p_src_left, p_dst_left);
				}

				// Compare the distance D0, D1, D2, and take the minimum one's shift vector.
				if( d_up <= d_mid && d_up <= d_left)
				{
					// For constrainted search space.
					int dstX = w + srcImgNNF[h-1][w].x;
					int dstY = h + srcImgNNF[h-1][w].y;
					if(IsSearchable(Patch(dstX, dstY, size, dstImg)))
					{
						srcImgNNF[h][w].x = srcImgNNF[h-1][w].x;
						srcImgNNF[h][w].y = srcImgNNF[h-1][w].y;
					}
				}
				else if( d_left <= d_mid && d_left <= d_up)
				{
					// For constrainted search space.
					int dstX = w + srcImgNNF[h][w-1].x;
					int dstY = h + srcImgNNF[h][w-1].y;
					if(IsSearchable(Patch(dstX, dstY, size, dstImg)))
					{
						srcImgNNF[h][w].x = srcImgNNF[h][w-1].x;
						srcImgNNF[h][w].y = srcImgNNF[h][w-1].y;
					}
				}
			}
		}
	}
	else
	{
		for(int h = srcH-1 ; h >=0 ; h--)
		{
			for(int w = srcW-1 ; w>=0 ; w--)
			{
				src_w = w;
				src_h = h;
				dst_w = w + (int)srcImgNNF[h][w].x;
				dst_h = h + (int)srcImgNNF[h][w].y;

				// Compute self distance D0.
				Patch p_src_mid(src_w, src_h, size, srcImg, srcPatch.maskImg, srcPatch.weight, srcPatch.colorModel);
				Patch p_dst_mid(dst_w, dst_h, size, dstImg);
				d_mid = Patch::Distance(p_src_mid, p_dst_mid);
				
				// Compute bottom-pixel distance D1.
				if( h+1 < 0 || h+1 >= srcH)
				{
					d_down = HUGE_VAL;
				}
				else
				{
					dst_w = w + (int)srcImgNNF[h+1][w].x;
					dst_h = h + (int)srcImgNNF[h+1][w].y;

					Patch p_src_down(src_w, src_h, size, srcImg, srcPatch.maskImg, srcPatch.weight, srcPatch.colorModel);
					Patch p_dst_down(dst_w, dst_h, size, dstImg);
					d_down = Patch::Distance(p_src_down, p_dst_down);
				}

				// Compute right-pixel distance D2.
				if( w+1 < 0 || w+1 >= srcW)
				{
					d_right = HUGE_VAL;
				}
				else
				{
					dst_w = w + (int)srcImgNNF[h][w+1].x;
					dst_h = h + (int)srcImgNNF[h][w+1].y;

					Patch p_src_right(src_w, src_h, size, srcImg, srcPatch.maskImg, srcPatch.weight, srcPatch.colorModel);
					Patch p_dst_right(dst_w, dst_h, size, dstImg);
					d_right = Patch::Distance(p_src_right, p_dst_right);
				}

				// Compare the distance D0, D1, D2, and take the minimum one's shift vector.
				if( d_down <= d_mid && d_down <= d_right)
				{
					// For constrainted search space.
					int dstX = w + srcImgNNF[h+1][w].x;
					int dstY = h + srcImgNNF[h+1][w].y;
					if(IsSearchable(Patch(dstX, dstY, size, dstImg)))
					{
						srcImgNNF[h][w].x = srcImgNNF[h+1][w].x;
						srcImgNNF[h][w].y = srcImgNNF[h+1][w].y;
					}
				}
				else if( d_right <= d_mid && d_right <= d_down)
				{
					// For constrainted search space.
					int dstX = w + srcImgNNF[h][w+1].x;
					int dstY = h + srcImgNNF[h][w+1].y;
					if(IsSearchable(Patch(dstX, dstY, size, dstImg)))
					{
						srcImgNNF[h][w].x = srcImgNNF[h][w+1].x;
						srcImgNNF[h][w].y = srcImgNNF[h][w+1].y;
					}
				}
			}
		}
	}
}

void RandomizedNNSearch::RandomSearch(int size, Patch &srcPatch)
{
	int srcH, srcW, dstH, dstW, radius, exp = 0, R;
	int src_w, src_h, dst_w, dst_h;
	float alpha = 0.5, oldDistance, newDistance;

	srcH = srcImg->height;
	srcW = srcImg->width;
	dstH = dstImg->height;
	dstW = dstImg->width;
	
	radius = max(dstH, dstW);
	R = max(dstH, dstW);

	srand(time(NULL));

	while(radius >= 1)
	{
//		printf("radius = %d\n", radius);

		for(int h = 0 ; h < srcH ; h++)
		{
			for(int w = 0 ; w < srcW ; w++)
			{
				src_w = w;
				src_h = h;
				dst_w = w + (int)srcImgNNF[h][w].x;
				dst_h = h + (int)srcImgNNF[h][w].y;

				int minX = dst_w - radius;
				int maxX = dst_w + radius;
				int minY = dst_h - radius;
				int maxY = dst_h + radius;
				
				if (minX < 0)
					minX = 0;

				if (maxX > dstW - 1)
					maxX = dstW;

				if (minY < 0) 
					minY = 0;

				if (maxY > dstH - 1) 
					maxY = dstH;

				// Randomize search.
				int randX, randY;
				do
				{
					randX = rand() % (maxX - minX) + minX;
					randY = rand() % (maxY - minY) + minY;
				}
				while(! IsSearchable(Patch(randX, randY, size, dstImg)));

				Patch src(src_w, src_h, size, srcImg, srcPatch.maskImg, srcPatch.weight, srcPatch.colorModel);
				Patch oldDst(dst_w, dst_h, size, dstImg);
				Patch newDst(randX, randY, size, dstImg);

				oldDistance = Patch::Distance(src, oldDst);
				newDistance = Patch::Distance(src, newDst);
				
				if(newDistance < oldDistance)
				{
					srcImgNNF[h][w].x = randX - src_w;
					srcImgNNF[h][w].y = randY - src_h;
				}
			}
		}

		exp++;
		radius = R*pow(alpha, (float)exp);
	}
}

bool ConstraintedRandomizedNNSearch::IsSearchable(Patch &dstPatch)
{
	// Get the patch information.
	int size = dstPatch.size;
	int x = dstPatch.x;
	int y = dstPatch.y;

	bool isSearchable = true;
	for(int h = -size/2 ; h <= size/2; h++)
	{
		for(int w = -size/2 ; w <= size/2; w++)
		{
			// Get the current checking point.
			int curX = x + w;
			int curY = y + h;

			if(curX < 0 || curY < 0 || curX >= dstImg->width || curY >= dstImg->height)
				continue;

			// Check if the patch overlaps the unmarked pixel.
			CvScalar v = cvGet2D(maskImg, curY, curX);
			if(v.val[0] == 0) isSearchable = false;
		}
	}

	return isSearchable;
}
//////////////////////////End of Randomized Search//////////////////////////////


/////////////////////////Start of Stereo NNSearch//////////////////////////////
StereoNNSearch::StereoNNSearch(IplImage* _srcImgL, IplImage* _srcImgR, IplImage* _dstImgL, IplImage* _dstImgR)
	: srcImgL(_srcImgL), srcImgR(_srcImgR), dstImgL(_dstImgL), dstImgR(_dstImgR)
{
	maskImgL = cvCreateImage(cvSize(dstImgL->width, dstImgL->height), IPL_DEPTH_8U, 1);
	maskImgR = cvCreateImage(cvSize(dstImgR->width, dstImgR->height), IPL_DEPTH_8U, 1);

	for(int i = 0; i < dstImgL->width; i++)
	{
		for(int j = 0; j < dstImgL->height; j++)
		{
			cvSet2D(maskImgL, j, i, cvScalar(1));
		}
	}

	for(int i = 0; i < dstImgR->width; i++)
	{
		for(int j = 0; j < dstImgR->height; j++)
		{
			cvSet2D(maskImgR, j, i, cvScalar(1));
		}
	}
}

bool StereoNNSearch::IsSearchable(PatchPair &dstPatchPair)
{
	return true;
}
PatchPair StereoBruteForceNNSearch::SearchPatch(PatchPair &srcPatchPair)
{
	float minDist = HUGE_VAL;
	PatchPair finalPP(0, 0, srcPatchPair.patchL.size, dstImgL, dstImgR);

	for(int h = 0 ; h < dstImgL->height ; h++)
	{
		for(int w = 0 ; w < dstImgL->width ; w++)
		{
			PatchPair dstPP(w, h, srcPatchPair.patchL.size, dstImgL, dstImgR);
			float dist = srcPatchPair.Distance(dstPP);

			if(dist < minDist)	
			{
				minDist = dist;
				finalPP = dstPP;
			}
		}
	}

	return finalPP;
}

PatchPair StereoRandomizedNNSearch::SearchPatch(PatchPair &srcPatchPair)
{
	int iteration = 1, size, srcH, srcW, dstH, dstW, outputX, outputY;

	// Get the patch size.
	size = srcPatchPair.patchL.size;
	// Get the image size.
	srcH = srcImgL->height;
	srcW = srcImgL->width;
	dstH = dstImgL->height;
	dstW = dstImgL->width;

	// If NNF is not constructed, construct it!! (Search a patch for the first time.)
	if( NNF_exist == false)
	{
		// Step 1 : Initialize the NNF by randomly sampling.
		Initialization(size);

		//Visualization();
		//ReconstructImage(srcImg, dstImg, size);
		
		while(iteration <= 5)
		{
			printf("In Iteration : %d\n", iteration);
			// Step 2 : Propagate the NN vector.
			Propagation(iteration, size, srcPatchPair);

			// Step 3 : Randomly refine the NN vector in multiple radius.
			RandomSearch(size, srcPatchPair);

			//Visualization();
			//ReconstructImage(srcImg, dstImg, size);

			iteration++;
		}

		for(int h = 0 ; h < srcH ; h++)
		{
			for(int w = 0 ; w < srcW ; w++)
			{
				int dst_h = h + (int)srcImgNNF[h][w].y;
				int dst_w = w + (int)srcImgNNF[h][w].x;

				if(dst_h > dstH-1 || dst_h < 0)
				{
					printf("y coordinate is out of bound of height of target image\n");
					PatchPair pp;

					return pp;
				}

				if(dst_w > dstW-1 || dst_w < 0)
				{
					printf("x coordinate is out of bound of width of target image\n");
					PatchPair pp;

					return pp;
				}
			}
		}

//		ReconstructImage(srcImg, dstImg, size);
		NNF_exist = true;

		outputX = srcPatchPair.patchL.x + (int)srcImgNNF[srcPatchPair.patchL.y][srcPatchPair.patchL.x].x;
		outputY = srcPatchPair.patchL.y + (int)srcImgNNF[srcPatchPair.patchL.y][srcPatchPair.patchL.x].y;
	}
	else
	{
		outputX = srcPatchPair.patchL.x + (int)srcImgNNF[srcPatchPair.patchL.y][srcPatchPair.patchL.x].x;
		outputY = srcPatchPair.patchL.y + (int)srcImgNNF[srcPatchPair.patchL.y][srcPatchPair.patchL.x].y;
	}

	PatchPair output(outputX, outputY, size, dstImgL, dstImgR);

	return output;
}


void StereoRandomizedNNSearch::Initialization(int size)
{
	int srcW, srcH, dstW, dstH;
	int h2, w2;
	
	srcW = srcImgL->width;
	srcH = srcImgL->height;
	dstW = dstImgL->width;
	dstH = dstImgL->height;

	srcImgNNF = new Vector2D*[srcH];

	for(int h = 0 ; h < srcH ; h++)
	{
		srcImgNNF[h] = new Vector2D[srcW];
	}

	srand(time(NULL));

	for(int h1 = 0 ; h1 < srcH ; h1++)
	{
		for(int w1 = 0 ; w1 < srcW ; w1++)
		{
			// For constrainted search space used.
			do
			{
				w2 = rand() % dstW;
				h2 = rand() % dstH;

			}
			while(! IsSearchable(PatchPair(w2, h2, size, dstImgL, dstImgR)));
			
			srcImgNNF[h1][w1].x = w2 - w1;
			srcImgNNF[h1][w1].y = h2 - h1;
		}
	}
}
void StereoRandomizedNNSearch::Propagation(int iteration, int size, PatchPair &srcPatchPair)
{
	int srcH, srcW, dstH, dstW;
	int src_h, src_w, dst_h, dst_w;
	float d_mid, d_up, d_left, d_right, d_down;
	
	srcH = srcImgL->height;
	srcW = srcImgL->width;
	dstH = dstImgL->height;
	dstW = dstImgL->width;

	if(iteration % 2 == 0)
	{
		for(int h = 0 ; h < srcH ; h++)
		{
			for(int w = 0 ; w < srcW ; w++)
			{
				src_w = w;
				src_h = h;
				dst_w = w + (int)srcImgNNF[h][w].x;
				dst_h = h + (int)srcImgNNF[h][w].y;

				// Compute self distance D0.
				PatchPair pp_src_mid(src_w, src_h, size, srcImgL, srcImgR, srcPatchPair.patchL.maskImg, srcPatchPair.patchR.maskImg, srcPatchPair.patchL.weight, srcPatchPair.patchL.colorModel);
				PatchPair pp_dst_mid(dst_w, dst_h, size, dstImgL, dstImgR);
				d_mid = PatchPair::Distance(pp_src_mid, pp_dst_mid);
			
				// Compute up-pixel distance D1.
				if( h-1 < 0 || h-1 >= srcH)
				{
					d_up = HUGE_VAL;
				}
				else
				{
					dst_w = w + (int)srcImgNNF[h-1][w].x;
					dst_h = h + (int)srcImgNNF[h-1][w].y;
					
					PatchPair pp_src_up(src_w, src_h, size, srcImgL, srcImgR, srcPatchPair.patchL.maskImg, srcPatchPair.patchR.maskImg, srcPatchPair.patchL.weight, srcPatchPair.patchL.colorModel);
					PatchPair pp_dst_up(dst_w, dst_h, size, dstImgL, dstImgR);
					d_up = PatchPair::Distance(pp_src_up, pp_dst_up);
				}

				// Compute left-pixel distance D2.
				if( w-1 < 0 || w-1 >= srcW)
				{
					d_left = HUGE_VAL;
				}
				else
				{
					dst_w = w + (int)srcImgNNF[h][w-1].x;
					dst_h = h + (int)srcImgNNF[h][w-1].y;

					PatchPair pp_src_left(src_w, src_h, size, srcImgL, srcImgR, srcPatchPair.patchL.maskImg, srcPatchPair.patchR.maskImg, srcPatchPair.patchL.weight, srcPatchPair.patchL.colorModel);
					PatchPair pp_dst_left(dst_w, dst_h, size, dstImgL, dstImgR);
					d_left = PatchPair::Distance(pp_src_left, pp_dst_left);
				}

				// Compare the distance D0, D1, D2, and take the minimum one's shift vector.
				if( d_up <= d_mid && d_up <= d_left)
				{
					// For constrainted search space.
					int dstX = w + srcImgNNF[h-1][w].x;
					int dstY = h + srcImgNNF[h-1][w].y;
					if(IsSearchable(PatchPair(dstX, dstY, size, dstImgL, dstImgR)))
					{
						srcImgNNF[h][w].x = srcImgNNF[h-1][w].x;
						srcImgNNF[h][w].y = srcImgNNF[h-1][w].y;
					}
				}
				else if( d_left <= d_mid && d_left <= d_up)
				{
					// For constrainted search space.
					int dstX = w + srcImgNNF[h][w-1].x;
					int dstY = h + srcImgNNF[h][w-1].y;
					if(IsSearchable(PatchPair(dstX, dstY, size, dstImgL, dstImgR)))
					{
						srcImgNNF[h][w].x = srcImgNNF[h][w-1].x;
						srcImgNNF[h][w].y = srcImgNNF[h][w-1].y;
					}
				}
			}
		}
	}
	else
	{
		for(int h = srcH-1 ; h >=0 ; h--)
		{
			for(int w = srcW-1 ; w>=0 ; w--)
			{
				src_w = w;
				src_h = h;
				dst_w = w + (int)srcImgNNF[h][w].x;
				dst_h = h + (int)srcImgNNF[h][w].y;

				// Compute self distance D0.
				PatchPair pp_src_mid(src_w, src_h, size, srcImgL, srcImgR, srcPatchPair.patchL.maskImg, srcPatchPair.patchR.maskImg, srcPatchPair.patchL.weight, srcPatchPair.patchL.colorModel);
				PatchPair pp_dst_mid(dst_w, dst_h, size, dstImgL, dstImgR);
				d_mid = PatchPair::Distance(pp_src_mid, pp_dst_mid);
				
				// Compute bottom-pixel distance D1.
				if( h+1 < 0 || h+1 >= srcH)
				{
					d_down = HUGE_VAL;
				}
				else
				{
					dst_w = w + (int)srcImgNNF[h+1][w].x;
					dst_h = h + (int)srcImgNNF[h+1][w].y;

					PatchPair pp_src_down(src_w, src_h, size, srcImgL, srcImgR, srcPatchPair.patchL.maskImg, srcPatchPair.patchR.maskImg, srcPatchPair.patchL.weight, srcPatchPair.patchL.colorModel);
					PatchPair pp_dst_down(dst_w, dst_h, size, dstImgL, dstImgR);
					d_down = PatchPair::Distance(pp_src_down, pp_dst_down);
				}

				// Compute right-pixel distance D2.
				if( w+1 < 0 || w+1 >= srcW)
				{
					d_right = HUGE_VAL;
				}
				else
				{
					dst_w = w + (int)srcImgNNF[h][w+1].x;
					dst_h = h + (int)srcImgNNF[h][w+1].y;

					PatchPair pp_src_right(src_w, src_h, size, srcImgL, srcImgR, srcPatchPair.patchL.maskImg, srcPatchPair.patchR.maskImg, srcPatchPair.patchL.weight, srcPatchPair.patchL.colorModel);
					PatchPair pp_dst_right(dst_w, dst_h, size, dstImgL, dstImgR);
					d_right = PatchPair::Distance(pp_src_right, pp_dst_right);
				}

				// Compare the distance D0, D1, D2, and take the minimum one's shift vector.
				if( d_down <= d_mid && d_down <= d_right)
				{
					// For constrainted search space.
					int dstX = w + srcImgNNF[h+1][w].x;
					int dstY = h + srcImgNNF[h+1][w].y;
					if(IsSearchable(PatchPair(dstX, dstY, size, dstImgL, dstImgR)))
					{
						srcImgNNF[h][w].x = srcImgNNF[h+1][w].x;
						srcImgNNF[h][w].y = srcImgNNF[h+1][w].y;
					}
				}
				else if( d_right <= d_mid && d_right <= d_down)
				{
					// For constrainted search space.
					int dstX = w + srcImgNNF[h][w+1].x;
					int dstY = h + srcImgNNF[h][w+1].y;
					if(IsSearchable(PatchPair(dstX, dstY, size, dstImgL, dstImgR)))
					{
						srcImgNNF[h][w].x = srcImgNNF[h][w+1].x;
						srcImgNNF[h][w].y = srcImgNNF[h][w+1].y;
					}
				}
			}
		}
	}
}
void StereoRandomizedNNSearch::RandomSearch(int size, PatchPair &srcPatchPair)
{
	int srcH, srcW, dstH, dstW, radius, exp = 0, R;
	int src_w, src_h, dst_w, dst_h;
	float alpha = 0.5, oldDistance, newDistance;

	srcH = srcImgL->height;
	srcW = srcImgL->width;
	dstH = dstImgL->height;
	dstW = dstImgL->width;
	
	radius = max(dstH, dstW);
	R = max(dstH, dstW);

	srand(time(NULL));

	while(radius >= 1)
	{
		printf("radius = %d\n", radius);

		for(int h = 0 ; h < srcH ; h++)
		{
			for(int w = 0 ; w < srcW ; w++)
			{
				src_w = w;
				src_h = h;
				dst_w = w + (int)srcImgNNF[h][w].x;
				dst_h = h + (int)srcImgNNF[h][w].y;

				int minX = dst_w - radius;
				int maxX = dst_w + radius;
				int minY = dst_h - radius;
				int maxY = dst_h + radius;
				
				if (minX < 0)
					minX = 0;

				if (maxX > dstW - 1)
					maxX = dstW;

				if (minY < 0) 
					minY = 0;

				if (maxY > dstH - 1) 
					maxY = dstH;

				// Randomize search.
				int randX, randY;
				do
				{
					randX = rand() % (maxX - minX) + minX;
					randY = rand() % (maxY - minY) + minY;
				}
				while(! IsSearchable(PatchPair(randX, randY, size, dstImgL, dstImgR)));

				PatchPair src(src_w, src_h, size, srcImgL, srcImgR, srcPatchPair.patchL.maskImg, srcPatchPair.patchR.maskImg, srcPatchPair.patchL.weight, srcPatchPair.patchL.colorModel);
				PatchPair oldDst(dst_w, dst_h, size, dstImgL, dstImgR);
				PatchPair newDst(randX, randY, size, dstImgL, dstImgR);

				oldDistance = PatchPair::Distance(src, oldDst);
				newDistance = PatchPair::Distance(src, newDst);
				
				if(newDistance < oldDistance)
				{
					srcImgNNF[h][w].x = randX - src_w;
					srcImgNNF[h][w].y = randY - src_h;
				}
			}
		}

		exp++;
		radius = R*pow(alpha, (float)exp);
	}
}

void StereoRandomizedNNSearch::Visualization()
{
	/* Visualization, and a and b's range are between -128 ~ +127 */
	float minH = HUGE_VAL, minW = HUGE_VAL, maxH = -HUGE_VAL, maxW = -HUGE_VAL;
	float ratioW, ratioH;
	int srcH, srcW, dstH, dstW;
	
	srcH = srcImgL->height;
	srcW = srcImgL->width;
	dstH = dstImgL->height;
	dstW = dstImgL->width;

	minW = -srcW;
	minH = -srcH;
	maxW = dstW;
	maxH = dstH;
	
	ratioW = 1.0f/(float)(maxW - minW);
	ratioH = 1.0f/(float)(maxH - minH);

	IplImage* imgL, *imgR, *visImgL, *visImgR;
	imgL = cvCreateImage(cvGetSize(srcImgL), IPL_DEPTH_32F, srcImgL->nChannels);
	imgR = cvCreateImage(cvGetSize(srcImgR), IPL_DEPTH_32F, srcImgR->nChannels);

	for(int h = 0 ; h < srcH ; h++)
	{
		for(int w = 0 ; w < srcW ; w++)
		{
			float channel1 = (srcImgNNF[h][w].x - minW)*ratioW*2 - 1.0f;
			float channel2 = (srcImgNNF[h][w].y - minH)*ratioH*2 - 1.0f;

			((float *)(imgL->imageData + h*imgL->widthStep))[w*imgL->nChannels + 0] = 0.2;
			((float *)(imgL->imageData + h*imgL->widthStep))[w*imgL->nChannels + 1] = channel1;
			((float *)(imgL->imageData + h*imgL->widthStep))[w*imgL->nChannels + 2] = channel2;

			((float *)(imgR->imageData + h*imgR->widthStep))[w*imgR->nChannels + 0] = 0.2;
			((float *)(imgR->imageData + h*imgR->widthStep))[w*imgR->nChannels + 1] = channel1;
			((float *)(imgR->imageData + h*imgR->widthStep))[w*imgR->nChannels + 2] = channel2;
		}
	}

	visImgL = myLab2BGR(imgL);
	visImgR = myLab2BGR(imgR);

	cvNamedWindow("VisualizeL");
	cvNamedWindow("VisualizeR");
	cvShowImage("VisualizeL", visImgL);
	cvShowImage("VisualizeR", visImgR);
	cvWaitKey();
	cvReleaseImage(&imgL);
	cvReleaseImage(&imgR);
	cvReleaseImage(&visImgL);
	cvReleaseImage(&visImgR);
}