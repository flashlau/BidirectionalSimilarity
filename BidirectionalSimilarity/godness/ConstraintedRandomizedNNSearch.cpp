//#include "ConstraintedRandomizedNNSearch.h"
//#include <time.h>
//#include <algorithm>
//
//ConstraintedRandomizedNNSearch::ConstraintedRandomizedNNSearch()
//	: RandomizedNNSearch()
//{}
//
//Patch ConstraintedRandomizedNNSearch::SearchPatch(IplImage* srcImg, IplImage* dstImg, Patch srcPatchPos)
//{
//	int iteration = 0, size, srcH, srcW, dstH, dstW, outputX, outputY;
//	
//	size = srcPatchPos.size;
//	srcH = srcImg->height;
//	srcW = srcImg->width;
//	dstH = dstImg->height;
//	dstW = dstImg->width;
//	
//	if( NNF_exist == false)
//	{
//		Initialization(srcImg, dstImg);
//		AddConstraintArea(Rect2D(110, 94, 161, 141), srcW, srcH, dstW, dstH);
//		//Visualization(srcImg, dstImg);
//		//ReconstructImage(srcImg, dstImg, size);
//	
//		while(iteration <= 2)
//		{
//			printf("In Iteration : %d\n", iteration);
//			Propagation(iteration, size, srcImg, dstImg);
//			RandomSearch(size, srcImg, dstImg);
//			//Visualization(srcImg, dstImg);
//			//ReconstructImage(srcImg, dstImg, size);
//			iteration++;
//		}
//
//		for(int h = 0 ; h < srcH ; h++)
//		{
//			for(int w = 0 ; w < srcW ; w++)
//			{
//				int dst_h = h + (int)srcImgNNF[h][w].y;
//				int dst_w = w + (int)srcImgNNF[h][w].x;
//
//				if(dst_h > dstH-1 || dst_h < 0)
//				{
//					printf("y coordinate is out of bound of height of target image\n");
//					Patch p;
//
//					return p;
//				}
//
//				if(dst_w > dstW-1 || dst_w < 0)
//				{
//					printf("x coordinate is out of bound of width of target image\n");
//					Patch p;
//
//					return p;
//				}
//			}
//		}
//
////		ReconstructImage(srcImg, dstImg, size);
//		NNF_exist = true;
//
//		outputX = srcPatchPos.x + (int)srcImgNNF[srcPatchPos.y][srcPatchPos.x].x;
//		outputY = srcPatchPos.y + (int)srcImgNNF[srcPatchPos.y][srcPatchPos.x].y;
//	}
//	else
//	{
//		outputX = srcPatchPos.x + (int)srcImgNNF[srcPatchPos.y][srcPatchPos.x].x;
//		outputY = srcPatchPos.y + (int)srcImgNNF[srcPatchPos.y][srcPatchPos.x].y;
//	}
//
//	Patch output(outputX, outputY, size, dstImg);
//
//	return output;
//}
//
//void ConstraintedRandomizedNNSearch::AddConstraintArea(Rect2D rectRegion, int srcW, int srcH, int dstW, int dstH)
//{
//	// Modify the constraint mask.
//	for(int y = rectRegion.yMin ; y < rectRegion.yMax ; y++)
//	{
//		for(int x = rectRegion.xMax ; x < rectRegion.xMax ; x++)
//		{
//			if( x >= 0 && y >= 0 && x < dstW && y < dstH)
//			{
//				constraintMask[y][x] = false;
//			}
//		}
//	}
//
//	// Re-seed the vector.
//	srand(time(NULL));
//
//	for(int h1 = 0 ; h1 < srcH ; h1++)
//	{
//		for(int w1 = 0 ; w1 < srcW ; w1++)
//		{
//			int dstX = w1 + srcImgNNF[h1][w1].x;
//			int dstY = h1 + srcImgNNF[h1][w1].y;
//
//			// If located in the constrainted region, re-seed it.
//			while(constraintMask[dstY][dstX] == false)
//			{
//				int w2 = rand() % dstW;
//				int h2 = rand() % dstH;
//
//				// Update the NNF data.
//				srcImgNNF[h1][w1].x = w2 - w1;
//				srcImgNNF[h1][w1].y = h2 - h1;
//
//				// Update x and y for iteratively checking.
//				dstX = w2;
//				dstY = h2;
//			}
//		}
//	}
//}
//
//void ConstraintedRandomizedNNSearch::Initialization(IplImage* srcImg, IplImage* dstImg)
//{
//	int srcW, srcH, dstW, dstH;
//	int h2, w2;
//
//	srcW = srcImg->width;
//	srcH = srcImg->height;
//	dstW = dstImg->width;
//	dstH = dstImg->height;
//
//	// Initialize the NNF (which should be the size of source image).
//	srcImgNNF = new Vector2D*[srcH];
//
//	for(int h = 0 ; h < srcH ; h++)
//	{
//		srcImgNNF[h] = new Vector2D[srcW];
//	}
//
//	// Initialize the whole constrainted mask to 1 (which should be the size of target image).
//	constraintMask = new bool*[dstH];
//
//	for(int h = 0 ; h < dstH ; h++)
//	{
//		constraintMask[h] = new bool[dstW];
//		for(int w = 0 ; w < dstW ; w++)
//		{
//			constraintMask[h][w] = true;
//		}
//	}
//
//	// Randomly seed the vector.
//	srand(time(NULL));
//
//	for(int h1 = 0 ; h1 < srcH ; h1++)
//	{
//		for(int w1 = 0 ; w1 < srcW ; w1++)
//		{
//			w2 = rand() % dstW;
//			h2 = rand() % dstH;
//			
//			srcImgNNF[h1][w1].x = w2 - w1;
//			srcImgNNF[h1][w1].y = h2 - h1;
//		}
//	}
//}
//
//void ConstraintedRandomizedNNSearch::Propagation(int iteration, int size, IplImage* srcImg, IplImage* dstImg)
//{
//	int srcH, srcW, dstH, dstW;
//	int src_h, src_w, dst_h, dst_w;
//	float d_mid, d_up, d_left, d_right, d_down;
//	
//	srcH = srcImg->height;
//	srcW = srcImg->width;
//	dstH = dstImg->height;
//	dstW = dstImg->width;
//
//	if(iteration % 2 == 0)
//	{
//		for(int h = 0 ; h < srcH ; h++)
//		{
//			for(int w = 0 ; w < srcW ; w++)
//			{
//				src_w = w;
//				src_h = h;
//				dst_w = w + (int)srcImgNNF[h][w].x;
//				dst_h = h + (int)srcImgNNF[h][w].y;
//
//				// Compute self distance D0.
//				Patch p_src_mid(src_w, src_h, size, srcImg);
//				Patch p_dst_mid(dst_w, dst_h, size, dstImg);
//				d_mid = Patch::Distance(p_src_mid, p_dst_mid);
//			
//				// Compute up-pixel distance D1.
//				if( h-1 < 0 || h-1 >= srcH)
//				{
//					d_up = HUGE_VAL;
//				}
//				else
//				{
//					dst_w = w + (int)srcImgNNF[h-1][w].x;
//					dst_h = h + (int)srcImgNNF[h-1][w].y;
//					
//					Patch p_src_up(src_w, src_h, size, srcImg);
//					Patch p_dst_up(dst_w, dst_h, size, dstImg);
//					d_up = Patch::Distance(p_src_up, p_dst_up);
//					
//				}
//
//				// Compute left-pixel distance D2.
//				if( w-1 < 0 || w-1 >= srcW)
//				{
//					d_left = HUGE_VAL;
//				}
//				else
//				{
//					dst_w = w + (int)srcImgNNF[h][w-1].x;
//					dst_h = h + (int)srcImgNNF[h][w-1].y;
//
//					Patch p_src_left(src_w, src_h, size, srcImg);
//					Patch p_dst_left(dst_w, dst_h, size, dstImg);
//					d_left = Patch::Distance(p_src_left, p_dst_left);
//				}
//
//				// Compare the distance D0, D1, D2, and take the minimum one's shift vector.
//				if( d_up <= d_mid && d_up <= d_left)
//				{
//					// Check if in constrainted region
//					int dstX = w + srcImgNNF[h-1][w].x;
//					int dstY = h + srcImgNNF[h-1][w].y;
//					if(constraintMask[dstY][dstX] == true)
//					{
//						srcImgNNF[h][w].x = srcImgNNF[h-1][w].x;
//						srcImgNNF[h][w].y = srcImgNNF[h-1][w].y;
//					}
//				}
//				else if( d_left <= d_mid && d_left <= d_up)
//				{
//					// Check if in constrainted region
//					int dstX = w + srcImgNNF[h][w-1].x;
//					int dstY = h + srcImgNNF[h][w-1].y;
//					if(constraintMask[dstY][dstX] == true)
//					{
//						srcImgNNF[h][w].x = srcImgNNF[h][w-1].x;
//						srcImgNNF[h][w].y = srcImgNNF[h][w-1].y;
//					}
//				}
//			}
//		}
//	}
//	else
//	{
//		for(int h = srcH-1 ; h >=0 ; h--)
//		{
//			for(int w = srcW-1 ; w>=0 ; w--)
//			{
//				src_w = w;
//				src_h = h;
//				dst_w = w + (int)srcImgNNF[h][w].x;
//				dst_h = h + (int)srcImgNNF[h][w].y;
//
//				// Compute self distance D0.
//				Patch p_src_mid(src_w, src_h, size, srcImg);
//				Patch p_dst_mid(dst_w, dst_h, size, dstImg);
//				d_mid = Patch::Distance(p_src_mid, p_dst_mid);
//				
//				// Compute bottom-pixel distance D1.
//				if( h+1 < 0 || h+1 >= srcH)
//				{
//					d_down = HUGE_VAL;
//				}
//				else
//				{
//					dst_w = w + (int)srcImgNNF[h+1][w].x;
//					dst_h = h + (int)srcImgNNF[h+1][w].y;
//
//					Patch p_src_down(src_w, src_h, size, srcImg);
//					Patch p_dst_down(dst_w, dst_h, size, dstImg);
//					d_down = Patch::Distance(p_src_down, p_dst_down);
//				}
//
//				// Compute right-pixel distance D2.
//				if( w+1 < 0 || w+1 >= srcW)
//				{
//					d_right = HUGE_VAL;
//				}
//				else
//				{
//					dst_w = w + (int)srcImgNNF[h][w+1].x;
//					dst_h = h + (int)srcImgNNF[h][w+1].y;
//
//					Patch p_src_right(src_w, src_h, size, srcImg);
//					Patch p_dst_right(dst_w, dst_h, size, dstImg);
//					d_right = Patch::Distance(p_src_right, p_dst_right);
//				}
//
//				// Compare the distance D0, D1, D2, and take the minimum one's shift vector.
//				if( d_down <= d_mid && d_down <= d_right)
//				{
//					// Check if in constrainted region
//					int dstX = w + srcImgNNF[h+1][w].x;
//					int dstY = h + srcImgNNF[h+1][w].y;
//					if(constraintMask[dstY][dstX] == true)
//					{
//						srcImgNNF[h][w].x = srcImgNNF[h+1][w].x;
//						srcImgNNF[h][w].y = srcImgNNF[h+1][w].y;
//					}					
//				}
//				else if( d_right <= d_mid && d_right <= d_down)
//				{
//					// Check if in constrainted region
//					int dstX = w + srcImgNNF[h][w+1].x;
//					int dstY = h + srcImgNNF[h][w+1].y;
//					if(constraintMask[dstY][dstX] == true)
//					{
//						srcImgNNF[h][w].x = srcImgNNF[h][w+1].x;
//						srcImgNNF[h][w].y = srcImgNNF[h][w+1].y;
//					}
//				}
//			}
//		}
//	}
//}
//
//void ConstraintedRandomizedNNSearch::RandomSearch(int size, IplImage* srcImg, IplImage* dstImg)
//{
//	int srcH, srcW, dstH, dstW, radius, exp = 0, R;
//	int src_w, src_h, dst_w, dst_h;
//	float alpha = 0.5, oldDistance, newDistance;
//
//	srcH = srcImg->height;
//	srcW = srcImg->width;
//	dstH = dstImg->height;
//	dstW = dstImg->width;
//	
//	radius = max(dstH, dstW);
//	R = max(dstH, dstW);
//
//	srand(time(NULL));
//
//	while(radius >= 1)
//	{
//		printf("radius = %d\n", radius);
//
//		for(int h = 0 ; h < srcH ; h++)
//		{
//			for(int w = 0 ; w < srcW ; w++)
//			{
//				src_w = w;
//				src_h = h;
//				dst_w = w + (int)srcImgNNF[h][w].x;
//				dst_h = h + (int)srcImgNNF[h][w].y;
//
//				// Decide the search region.
//				int minX = dst_w - radius;
//				int maxX = dst_w + radius;
//				int minY = dst_h - radius;
//				int maxY = dst_h + radius;
//				
//				if (minX < 0)
//					minX = 0;
//
//				if (maxX > dstW - 1)
//					maxX = dstW;
//
//				if (minY < 0) 
//					minY = 0;
//
//				if (maxY > dstH - 1) 
//					maxY = dstH;
//
//				// Randomize search.
//				int randX, randY;
//				do
//				{
//					randX = rand() % (maxX - minX) + minX;
//					randY = rand() % (maxY - minY) + minY;
//				}
//				while(constraintMask[randY][randX] == false);
//
//				Patch src(src_w, src_h, size, srcImg);
//				Patch oldDst(dst_w, dst_h, size, dstImg);
//				Patch newDst(randX, randY, size, dstImg);
//
//				oldDistance = Patch::Distance(src, oldDst);
//				newDistance = Patch::Distance(src, newDst);
//				
//				if(newDistance < oldDistance)
//				{
//					srcImgNNF[h][w].x = randX - src_w;
//					srcImgNNF[h][w].y = randY - src_h;
//				}
//			}
//		}
//
//		exp++;
//		radius = R*pow(alpha, (float)exp);
//	}
//}
