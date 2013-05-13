#include "ImageEnlarge.h"
#include "ImageDistance.h"
#include "Color_Transfer.h"
#include "NNSearch.h"
#include "Rect2D.h"
#include <time.h>

extern int ITERATION_ENLARGE;

ImageEnlarge::ImageEnlarge(IplImage* _srcImg, IplImage* _initDstImg, int _height, int _width, int _patchSize)
	: srcImg(_srcImg), dstImg(_initDstImg), patchSize(_patchSize)
{
	// The user-wanted final-height and final-width;
	height = _height;
	width = _width;

	// The nn field.
	nnfS2T = NULL;
	nnfT2S = NULL;

	// The coherence and completeness term.
	coherePatchNum = NULL;
	complePatchNum = NULL;
	compleTermForPixel = NULL;
	cohereTermForPixel = NULL;

	// Initialize the hard constraint mask.
	/*srcHardConstraintMask = cvCreateImage(cvGetSize(srcImg), IPL_DEPTH_8U, 1);
	dstHardConstraintMask = cvCreateImage(cvGetSize(srcImg), IPL_DEPTH_8U, 1);
	cvZero(srcHardConstraintMask);
	cvZero(dstHardConstraintMask);*/
}  

void ImageEnlarge::Construct()
{
	// Initialize the dst image.
	InitializeDst();
	cvSaveImage("Enlarge_0.jpg", dstImg);

	// Iterative refine the dst image until converge.
	for(int iteration = 1 ; iteration <= ITERATION_ENLARGE ; iteration++)
	{
		printf("Iteration : %d\n", iteration);
		
		// Construct the NNField.
		#pragma omp parallel sections
		{
			#pragma omp section
			{
				Src2DstNNFConstruct();
			}
			#pragma omp section
			{
				Dst2SrcNNFConstruct();
			}
		}

		// Vote to refine the result.
		UpdateDst();

		// Write out the results.
		char name[100];
		sprintf(name, "Enlarge_%d.jpg", iteration);
		cvSaveImage(name, dstImg);
	}
}

//void ImageEnlarge::AddHardConstraint(HardConstraint constraint)
//{
//	hardConstraints.push_back(constraint);
//
//	// Fill the constraint mask.
//	vector<Point2D> ori_vertices = constraint.polygon.vertice;
//	CvPoint *src_vertices = new CvPoint[ori_vertices.size()];
//	CvPoint *dst_vertices = new CvPoint[ori_vertices.size()];
//	for(int i = 0; i < ori_vertices.size(); i++)
//	{
//		src_vertices[i].x = ori_vertices[i].x;
//		src_vertices[i].y = ori_vertices[i].y;
//		dst_vertices[i].x = ori_vertices[i].x + constraint.shift.x;
//		dst_vertices[i].y = ori_vertices[i].y + constraint.shift.y;
//	}
//
//	cvFillConvexPoly(srcHardConstraintMask, src_vertices, ori_vertices.size(), cvScalar(10 * hardConstraints.size()));
//	cvFillConvexPoly(dstHardConstraintMask, dst_vertices, ori_vertices.size(), cvScalar(10 * hardConstraints.size()));
//}

IplImage* ImageEnlarge::GetResultImage()
{
	return dstImg;
}

void ImageEnlarge::InitializeDst()
{
	// If the initial target image is given, do nothing here.
	if(dstImg != NULL) return;

	// Create the target image.
	dstImg = cvCreateImage(cvSize(width, height), srcImg->depth, srcImg->nChannels);

	// The size information.
	int srcH = srcImg->height;
	int srcW = srcImg->width;
	int dstH = dstImg->height;
	int dstW = dstImg->width;

	srand(time(NULL));

	// Initialize nn field (SRC->DST).
	nnfS2T = new Vector2D*[srcH];
	for(int h = 0 ; h < srcH ; h++)
	{
		nnfS2T[h] = new Vector2D[srcW];
		for(int w = 0 ; w < srcW ; w++)
		{
			int dstX = rand() % dstW;
			int dstY = rand() % dstH;
			
			nnfS2T[h][w].x = (float)dstX - w; 
			nnfS2T[h][w].y = (float)dstY - h;
		}
	}


	// Initialize nn field (DST->SRC).
	nnfT2S = new Vector2D*[dstH];
	for(int h = 0 ; h < dstH ; h++)
	{
		nnfT2S[h] = new Vector2D[dstW];
		for(int w = 0 ; w < dstW ; w++)
		{
			int srcX = rand() % srcW;
			int srcY = rand() % srcH;

			nnfT2S[h][w].x = (float)srcX - w; 
			nnfT2S[h][w].y = (float)srcY - h;
		}
	}

	// Voting to get the initial guess.
	UpdateDst();

	printf("Initialization : done ................\n");
}

void ImageEnlarge::Src2DstNNFConstruct()
{
	// Source image size.
	int srcH = srcImg->height;
	int srcW = srcImg->width;

	// Randomized search (SRC->DST).
	RandomizedNNSearch rmns(srcImg, dstImg);

	if(nnfS2T != NULL)
	{
		for(int h = 0 ; h < srcH ; h++)
		{
			delete [] nnfS2T[h];
		}
		delete [] nnfS2T;
		nnfS2T = NULL;
	}

	nnfS2T = new Vector2D*[srcH];
	for(int h = 0 ; h < srcH ; h++)
	{
		nnfS2T[h] = new Vector2D[srcW];
		for(int w = 0 ; w < srcW ; w++)
		{
			Patch src(w, h, patchSize, srcImg);
			Patch dst = rmns.SearchPatch(src);
			nnfS2T[h][w].x = (float)dst.x - src.x; 
			nnfS2T[h][w].y = (float)dst.y - src.y;
		}
	}

	printf("Src2DstNNFConstruct : done ...............\n");
}

void ImageEnlarge::Dst2SrcNNFConstruct()
{
	// Target image size.
	int	dstH = dstImg->height;
	int dstW = dstImg->width;

	// Randomized search (DST->SRC).
	RandomizedNNSearch rmns(dstImg, srcImg);

	if(nnfT2S != NULL)
	{
		for(int h = 0 ; h < dstH ; h++)
		{
			delete [] nnfT2S[h];
		}
		delete [] nnfT2S;
		nnfT2S = NULL;
	}

	nnfT2S = new Vector2D*[dstH];
	for(int h = 0 ; h < dstH ; h++)
	{
		nnfT2S[h] = new Vector2D[dstW];
		for(int w = 0 ; w < dstW ; w++)
		{
			Patch dst(w, h, patchSize, dstImg);
			Patch src = rmns.SearchPatch(dst);
			nnfT2S[h][w].x = (float)src.x - dst.x; 
			nnfT2S[h][w].y = (float)src.y - dst.y;
		}
	}

	printf("Dst2SrcNNFConstruct : done ...............\n");
}

void ImageEnlarge::Coherence()
{
	int dstH, dstW, srcH, srcW;
	float weight;

	dstH = dstImg->height;
	srcH = srcImg->height;
	dstW = dstImg->width;
	srcW = srcImg->width;

	weight = 1.0f/(dstH*dstW);

	if(cohereTermForPixel != NULL || coherePatchNum != NULL)
	{
		for(int h = 0 ; h < dstH ; h++)
		{
			delete [] cohereTermForPixel[h];
			delete [] coherePatchNum[h];
		}
		delete [] cohereTermForPixel;
		delete [] coherePatchNum;

		cohereTermForPixel = NULL;
		coherePatchNum = NULL;
	}

	cohereTermForPixel = new RgbPixel*[dstH];
	coherePatchNum = new int*[dstH];

	for(int h = 0 ; h < dstH ; h++)
	{
		cohereTermForPixel[h] = new RgbPixel[dstW];
		coherePatchNum[h] = new int[dstW];

		for(int w = 0 ; w < dstW ; w++)
		{
			cohereTermForPixel[h][w].r = 0;
			cohereTermForPixel[h][w].g = 0;
			cohereTermForPixel[h][w].b = 0;

			coherePatchNum[h][w] = 0;
		}
	}

	for(int h = 0 ; h < dstH ; h++)
	{
		for(int w = 0 ; w < dstW ; w++)
		{
			for(int dh = -patchSize/2 ; dh <= patchSize/2 ; dh++)
			{
				for(int dw = -patchSize/2 ; dw <= patchSize/2 ; dw++)
				{
					int dst_x = w + dw;
					int dst_y = h + dh;

					if( dst_x < 0 || dst_x > dstW-1 || dst_y < 0 || dst_y > dstH-1)
						continue;

					Patch dst(dst_x, dst_y, patchSize, dstImg);
					Patch src(dst_x + nnfT2S[dst.y][dst.x].x, dst_y + nnfT2S[dst.y][dst.x].y, patchSize, srcImg);

					int src_x = src.x - dw;
					int src_y = src.y - dh;

					if( src_x < 0 || src_x > srcW-1 || src_y < 0 || src_y > srcH-1)
						continue;
					
					int r = ((uchar *)(srcImg->imageData + src_y*srcImg->widthStep))[src_x*srcImg->nChannels + 0];
					int g = ((uchar *)(srcImg->imageData + src_y*srcImg->widthStep))[src_x*srcImg->nChannels + 1];
					int b = ((uchar *)(srcImg->imageData + src_y*srcImg->widthStep))[src_x*srcImg->nChannels + 2];
					
					RgbPixel srcP(r, g, b);

					cohereTermForPixel[h][w].r = cohereTermForPixel[h][w].r + srcP.r;
					cohereTermForPixel[h][w].g = cohereTermForPixel[h][w].g + srcP.g;
					cohereTermForPixel[h][w].b = cohereTermForPixel[h][w].b + srcP.b;

					coherePatchNum[h][w]++;
				}
			}
		}
	}

	printf("Coherence : done ...............\n");
}

void ImageEnlarge::Completeness()
{
	int dstH, dstW, srcH, srcW;
	float weight;

	dstH = dstImg->height;
	srcH = srcImg->height;
	dstW = dstImg->width;
	srcW = srcImg->width;

	weight = 1.0f/(srcH*srcW);

	std::vector<Patch>** imgPatchMap = new vector<Patch>* [dstH];

	for(int h = 0 ; h < dstH ; h++)
	{
		imgPatchMap[h] = new vector<Patch>[dstW];
	}


	for(int h = 0 ; h < srcH ; h++)
	{
		for(int w = 0 ; w < srcW ; w++)
		{
			int dst_y = h + (int)nnfS2T[h][w].y;
			int dst_x = w + (int)nnfS2T[h][w].x;

			Patch p(w, h, patchSize, srcImg);
			imgPatchMap[dst_y][dst_x].push_back(p);
		}
	}

	if(compleTermForPixel != NULL || complePatchNum != NULL)
	{
		for(int h = 0 ; h < dstH ; h++)
		{
			delete [] compleTermForPixel[h];
			delete [] complePatchNum[h];
		}
		delete [] compleTermForPixel;
		delete [] complePatchNum;

		compleTermForPixel = NULL;
		complePatchNum = NULL;
	}

	compleTermForPixel = new RgbPixel*[dstH];
	complePatchNum = new int*[dstH];

	for(int h = 0 ; h < dstH ; h++)
	{
		compleTermForPixel[h] = new RgbPixel[dstW];
		complePatchNum[h] = new int[dstW];

		for(int w = 0 ; w < dstW ; w++)
		{
			compleTermForPixel[h][w].r = 0;
			compleTermForPixel[h][w].g = 0;
			compleTermForPixel[h][w].b = 0;

			complePatchNum[h][w] = 0;
		}
	}

	for(int h = 0 ; h < dstH ; h++)
	{
		for(int w = 0 ; w < dstW ; w++)
		{
			for(int dh = -patchSize/2 ; dh <= patchSize/2 ; dh++)
			{
				for(int dw = -patchSize/2 ; dw <= patchSize/2 ; dw++)
				{
					int dst_h = h + dh;
					int dst_w = w + dw;

					if(dst_h < 0 || dst_h > dstH-1 || dst_w < 0 || dst_w > dstW-1)
						continue;
					
					if(imgPatchMap[dst_h][dst_w].size() != 0)
					{
						for(int i = 0 ; i < imgPatchMap[dst_h][dst_w].size() ; i++)
						{
							int src_h = imgPatchMap[dst_h][dst_w][i].y - dh;
							int src_w = imgPatchMap[dst_h][dst_w][i].x - dw;

							if(src_h < 0 || src_h > srcH-1 || src_w < 0 || src_w > srcW-1)
								continue;

							int r = ((uchar *)(srcImg->imageData + src_h*srcImg->widthStep))[src_w*srcImg->nChannels + 0];
							int g = ((uchar *)(srcImg->imageData + src_h*srcImg->widthStep))[src_w*srcImg->nChannels + 1];
							int b = ((uchar *)(srcImg->imageData + src_h*srcImg->widthStep))[src_w*srcImg->nChannels + 2];

							RgbPixel srcP(r, g, b);
							
							compleTermForPixel[h][w].r = compleTermForPixel[h][w].r + srcP.r;
							compleTermForPixel[h][w].g = compleTermForPixel[h][w].g + srcP.g;
							compleTermForPixel[h][w].b = compleTermForPixel[h][w].b + srcP.b;
							
							complePatchNum[h][w]++;
						}
					}
					
				}
			}
		}
	}

	for(int h = 0 ; h < dstH ; h++)
	{
		delete [] imgPatchMap[h];
	}
	delete [] imgPatchMap;

	printf("Completeness : done ...............\n");
}

void ImageEnlarge::UpdateDst()
{
	printf("UpdataDst........\n");

	int dstH, dstW, srcH, srcW;
	float Ns, Nt;

	dstH = dstImg->height;
	srcH = srcImg->height;
	dstW = dstImg->width;
	srcW = srcImg->width;
	
	Ns = srcH*srcW;
	Nt = dstH*dstW;

	#pragma omp parallel sections
	{

		#pragma omp section
		{
			Coherence();
		}
	
		#pragma omp section
		{
			Completeness();
		}
	}
	cvReleaseImage(&dstImg);
	dstImg = cvCreateImage(cvSize(dstW, dstH), IPL_DEPTH_8U, srcImg->nChannels);

	for(int h = 0 ; h < dstH ; h++)
	{
		for(int w = 0 ; w < dstW ; w++)
		{
			float r = (Nt * compleTermForPixel[h][w].r + Ns * cohereTermForPixel[h][w].r) / (Nt * (float)complePatchNum[h][w] + Ns* (float)coherePatchNum[h][w]);
			float g = (Nt * compleTermForPixel[h][w].g + Ns * cohereTermForPixel[h][w].g) / (Nt * (float)complePatchNum[h][w] + Ns* (float)coherePatchNum[h][w]);
			float b = (Nt * compleTermForPixel[h][w].b + Ns * cohereTermForPixel[h][w].b) / (Nt * (float)complePatchNum[h][w] + Ns* (float)coherePatchNum[h][w]);

			((uchar *)(dstImg->imageData + h*dstImg->widthStep))[w*dstImg->nChannels + 0] = (r - (int)r < 0.5) ? (int)r : (int)(r+1);
			((uchar *)(dstImg->imageData + h*dstImg->widthStep))[w*dstImg->nChannels + 1] = (g - (int)g < 0.5) ? (int)g : (int)(g+1);
			((uchar *)(dstImg->imageData + h*dstImg->widthStep))[w*dstImg->nChannels + 2] = (b - (int)b < 0.5) ? (int)b : (int)(b+1);

		}
	}
}

ImageEnlarge::~ImageEnlarge()
{
	if(cohereTermForPixel != NULL || compleTermForPixel != NULL)
	{
		for(int h = 0 ; h < dstImg->height ; h++)
		{
			delete [] cohereTermForPixel[h];
			delete [] coherePatchNum[h];
			delete [] compleTermForPixel[h];
			delete [] complePatchNum[h];
		}
		delete [] cohereTermForPixel;
		delete [] coherePatchNum;
		delete [] compleTermForPixel;
		delete [] complePatchNum;
	}

	if(srcImg != NULL)
		cvReleaseImage(&srcImg);
	if(dstImg != NULL)
		cvReleaseImage(&dstImg);
}