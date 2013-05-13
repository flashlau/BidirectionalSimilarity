#include "ImageDistance.h"
#include "Color_Transfer.h"
#include "NNSearch.h"
#include "Patch.h"
#include <vector>

float BdsDistance::ComputeDistance(IplImage *img1, IplImage *img2)
{
	int size = 7, dstH, dstW, srcH, srcW;
	
	IplImage* floatImg1 = cvCreateImage(cvGetSize(img1), IPL_DEPTH_32F, img1->nChannels);
	IplImage* floatImg2 = cvCreateImage(cvGetSize(img2), IPL_DEPTH_32F, img2->nChannels);

	dstH = img2->height;
	dstW = img2->width;
	srcH = img1->height;
	srcW = img1->width;

	for(int h = 0 ; h < srcH ; h++)
	{
		for(int w = 0 ; w < srcW ; w++)
		{
			int b = ((uchar *)(img1->imageData + h*img1->widthStep))[w*img1->nChannels + 0];
			int g = ((uchar *)(img1->imageData + h*img1->widthStep))[w*img1->nChannels + 1];
			int r = ((uchar *)(img1->imageData + h*img1->widthStep))[w*img1->nChannels + 2];

			((float *)(floatImg1->imageData + h*floatImg1->widthStep))[w*floatImg1->nChannels + 0] = (float)b;
			((float *)(floatImg1->imageData + h*floatImg1->widthStep))[w*floatImg1->nChannels + 1] = (float)g;
			((float *)(floatImg1->imageData + h*floatImg1->widthStep))[w*floatImg1->nChannels + 2] = (float)r;
		}
	}

	for(int h = 0 ; h < dstH ; h++)
	{
		for(int w = 0 ; w < dstW ; w++)
		{
			int b = ((uchar *)(img2->imageData + h*img2->widthStep))[w*img2->nChannels + 0];
			int g = ((uchar *)(img2->imageData + h*img2->widthStep))[w*img2->nChannels + 1];
			int r = ((uchar *)(img2->imageData + h*img2->widthStep))[w*img2->nChannels + 2];

			((float *)(floatImg2->imageData + h*floatImg2->widthStep))[w*floatImg2->nChannels + 0] = (float)b;
			((float *)(floatImg2->imageData + h*floatImg2->widthStep))[w*floatImg2->nChannels + 1] = (float)g;
			((float *)(floatImg2->imageData + h*floatImg2->widthStep))[w*floatImg2->nChannels + 2] = (float)r;
		}
	}

	labImg1 = myBGR2Lab(floatImg1);
	labImg2 = myBGR2Lab(floatImg2);
	
	cvReleaseImage(&floatImg1);
	cvReleaseImage(&floatImg2);

	return Completeness(img1, img2, size) + Coherence(img1, img2, size);
}

float BdsDistance::Coherence(IplImage* img1, IplImage* img2, int size)
{
	int dstH, dstW, srcH, srcW;
	float coherenceError = 0, weight;
	RandomizedNNSearch rmns(img1, img2);

	dstH = img2->height;
	dstW = img2->width;
	srcH = img1->height;
	srcW = img1->width;

	Vector2D** nnfT2S;
	nnfT2S = new Vector2D*[dstH];
	for(int h = 0 ; h < dstH ; h++)
	{
		nnfT2S[h] = new Vector2D[dstW];
		for(int w = 0 ; w < dstW ; w++)
		{
			Patch dst(w, h, size, img2);
			Patch src = rmns.SearchPatch(dst);
			nnfT2S[h][w].x = (float)src.x - dst.x; 
			nnfT2S[h][w].y = (float)src.y - dst.y;
		}
	}

	weight = 1.0f/(dstH*dstW);
	
	for(int h = 0 ; h < dstH ; h++)
	{
		for(int w = 0 ; w < dstW ; w++)
		{
			LabPixel dstP;
			dstP.l = ((float *)(labImg2->imageData + h*labImg2->widthStep))[w*labImg2->nChannels + 0];
			dstP.a = ((float *)(labImg2->imageData + h*labImg2->widthStep))[w*labImg2->nChannels + 1];
			dstP.b = ((float *)(labImg2->imageData + h*labImg2->widthStep))[w*labImg2->nChannels + 2];
		
			for(int dh = -size/2 ; dh <= size/2 ; dh++)
			{
				for(int dw = -size/2 ; dw <= size/2 ; dw++)
				{
					int dst_x = w + dw;
					int dst_y = h + dh;

					if( dst_x < 0 || dst_x > dstW-1 || dst_y < 0 || dst_y > dstH-1)
						continue;

					Patch dst(dst_x, dst_y, size, img2);
					Patch src(dst_x + nnfT2S[dst.y][dst.x].x, dst_y + nnfT2S[dst.y][dst.x].y, size, img1);
				
					int src_x = src.x - dw;
					int src_y = src.y - dh;

					if( src_x < 0 || src_x > srcW-1 || src_y < 0 || src_y > srcH-1)
						continue;
					
					LabPixel srcP;
					srcP.l = ((float *)(labImg1->imageData + src_y*labImg1->widthStep))[src_x*labImg1->nChannels + 0];
					srcP.a = ((float *)(labImg1->imageData + src_y*labImg1->widthStep))[src_x*labImg1->nChannels + 1];
					srcP.b = ((float *)(labImg1->imageData + src_y*labImg1->widthStep))[src_x*labImg1->nChannels + 2];

					float dl = srcP.l - dstP.l;
					float da = srcP.a - dstP.a;
					float db = srcP.b - dstP.b;

					coherenceError = coherenceError + weight * sqrt(dl*dl + da*da + db*db);
				}
			}

		}
		
	}

	for(int h = 0 ; h < dstH ; h++)
	{
		delete [] nnfT2S[h];
	}
	delete [] nnfT2S;

	printf("cohere = %f\n", coherenceError);

	return coherenceError;
}

float BdsDistance::Completeness(IplImage* img1, IplImage* img2, int size)
{
	int dstH, dstW, srcH, srcW;
	float completenessError = 0, weight;
	RandomizedNNSearch rmns(img1, img2);

	srcH = img1->height;
	srcW = img1->width;
	dstH = img2->height;
	dstW = img2->width;

	weight = 1.0f/(srcH*srcW);

	Vector2D** nnfS2T;
	nnfS2T = new Vector2D*[srcH];
	for(int h = 0 ; h < srcH ; h++)
	{
		nnfS2T[h] = new Vector2D[srcW];
		for(int w = 0 ; w < srcW ; w++)
		{
			Patch src(w, h, size, img1);
			Patch dst = rmns.SearchPatch(src);
			nnfS2T[h][w].x = (float)dst.x - src.x; 
			nnfS2T[h][w].y = (float)dst.y - src.y;
		}
	}
	
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

			Patch p(w, h, size, img1);
			imgPatchMap[dst_y][dst_x].push_back(p);
		}
	}
	
	for(int h = 0 ; h < dstH ; h++)
	{
		for(int w = 0 ; w < dstW ; w++)
		{
			LabPixel dstP;
			dstP.l = ((float *)(labImg2->imageData + h*labImg2->widthStep))[w*labImg2->nChannels + 0];
			dstP.a = ((float *)(labImg2->imageData + h*labImg2->widthStep))[w*labImg2->nChannels + 1];
			dstP.b = ((float *)(labImg2->imageData + h*labImg2->widthStep))[w*labImg2->nChannels + 2];

			for(int dh = -size/2 ; dh <= size/2 ; dh++)
			{
				for(int dw = -size/2 ; dw <= size/2 ; dw++)
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

							LabPixel srcP;
							srcP.l = ((float *)(labImg1->imageData + src_h*labImg1->widthStep))[src_w*labImg1->nChannels + 0];
							srcP.a = ((float *)(labImg1->imageData + src_h*labImg1->widthStep))[src_w*labImg1->nChannels + 1];
							srcP.b = ((float *)(labImg1->imageData + src_h*labImg1->widthStep))[src_w*labImg1->nChannels + 2];

							float dl = srcP.l - dstP.l;
							float da = srcP.a - dstP.a;
							float db = srcP.b - dstP.b;

							completenessError = completenessError + weight * sqrt(dl*dl + da*da + db*db);
						}
					}
				}
			}			
		}
	}
	
	for(int h = 0 ; h < srcH ; h++)
	{
		delete [] nnfS2T[h];
	}
	delete [] nnfS2T;
	
	printf("complete = %f\n", completenessError);

	return completenessError;
}

BdsDistance::~BdsDistance()
{
	if(labImg1 != NULL)
		cvReleaseImage(&labImg1);
	if(labImg2 != NULL)
		cvReleaseImage(&labImg2);
}