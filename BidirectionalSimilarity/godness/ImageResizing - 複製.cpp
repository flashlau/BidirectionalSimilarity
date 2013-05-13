#include "ImageResizing.h"
#include "NNSearch.h"
#include "Color_Transfer.h"
#include "Patch.h"

ImageResizing::ImageResizing(IplImage* _srcImg, int _patchSize)
{
	srcImg = _srcImg;
	dstImg = cvCloneImage(srcImg);
	patchSize = _patchSize;

	nnfS2T = NULL;
	nnfT2S = NULL;
	compleTermForPixel = NULL;
	cohereTermForPixel = NULL;
	coherePatchNum = NULL;
	complePatchNum = NULL;

	IplImage* floatSrcImg = cvCreateImage(cvGetSize(srcImg), IPL_DEPTH_32F, srcImg->nChannels);

	for(int h = 0 ; h < srcImg->height ; h++)
	{
		for(int w = 0 ; w < srcImg->width ; w++)
		{
			int b = ((uchar *)(srcImg->imageData + h*srcImg->widthStep))[w*srcImg->nChannels + 0];
			int g = ((uchar *)(srcImg->imageData + h*srcImg->widthStep))[w*srcImg->nChannels + 1];
			int r = ((uchar *)(srcImg->imageData + h*srcImg->widthStep))[w*srcImg->nChannels + 2];

			((float *)(floatSrcImg->imageData + h*floatSrcImg->widthStep))[w*floatSrcImg->nChannels + 0] = (float)b;
			((float *)(floatSrcImg->imageData + h*floatSrcImg->widthStep))[w*floatSrcImg->nChannels + 1] = (float)g;
			((float *)(floatSrcImg->imageData + h*floatSrcImg->widthStep))[w*floatSrcImg->nChannels + 2] = (float)r;
		}
	}

	labSrcImg = myBGR2Lab(floatSrcImg);
	labDstImg = NULL;

	cvReleaseImage(&floatSrcImg);
}

void ImageResizing::Construct(int finalWidth, int finalHeight)
{
	int iteration = 0;
	while(dstImg->height != finalHeight || dstImg->width != finalWidth)
	{
		InitializeDst(finalWidth, finalHeight);

		for(int innerIteration = 0 ; innerIteration < 100 ; innerIteration++)
		{
			printf("innerIteration : %d\n", innerIteration);
			Src2DstNNFConstruct();
			Dst2SrcNNFConstruct();
			UpdateDst();
			
			char name[100];
			sprintf(name, "dolphin_%d_%d.jpg", iteration, innerIteration);
			cvSaveImage(name, dstImg);
			/*cvNamedWindow("TempImg");
			cvShowImage("TempImg", dstImg);
			cvWaitKey();*/
		}
		iteration++;
	}
	
	/*cvNamedWindow("Result");
	cvShowImage("Result", dstImg);
	cvWaitKey();*/
}

void ImageResizing::InitializeDst(int finalWidth, int finalHeight)
{
	IplImage* lastDstImg = cvCloneImage(dstImg);
	CvSize dst_cvsize;
	float scale = 0.95;

	if(dstImg != NULL)
		cvReleaseImage(&dstImg);

	dst_cvsize.width = lastDstImg->width * scale;
	dst_cvsize.height = lastDstImg->height * scale;

	if(dst_cvsize.width < finalWidth && dst_cvsize.height > finalHeight)
		dstImg = cvCreateImage(cvSize(finalWidth, dst_cvsize.height), lastDstImg->depth, lastDstImg->nChannels);
	else if(dst_cvsize.height < finalHeight && dst_cvsize.width > finalWidth)
		dstImg = cvCreateImage(cvSize(dst_cvsize.width, finalHeight), lastDstImg->depth, lastDstImg->nChannels);
	else if (dst_cvsize.width < finalWidth && dst_cvsize.height < finalHeight)
		dstImg = cvCreateImage(cvSize(finalWidth, finalHeight), lastDstImg->depth, lastDstImg->nChannels);
	else
		dstImg = cvCreateImage(dst_cvsize, lastDstImg->depth, lastDstImg->nChannels);
	
	cvResize(lastDstImg, dstImg);

	//cvNamedWindow("resizeDst");
	//cvShowImage("resizeDst", dstImg);
	//cvWaitKey();
	
	IplImage* floatDstImg = cvCreateImage(cvGetSize(dstImg), IPL_DEPTH_32F, dstImg->nChannels);

	for(int h = 0 ; h < dstImg->height ; h++)
	{
		for(int w = 0 ; w < dstImg->width ; w++)
		{
			int b = ((uchar *)(dstImg->imageData + h*dstImg->widthStep))[w*dstImg->nChannels + 0];
			int g = ((uchar *)(dstImg->imageData + h*dstImg->widthStep))[w*dstImg->nChannels + 1];
			int r = ((uchar *)(dstImg->imageData + h*dstImg->widthStep))[w*dstImg->nChannels + 2];

			((float *)(floatDstImg->imageData + h*floatDstImg->widthStep))[w*floatDstImg->nChannels + 0] = (float)b;
			((float *)(floatDstImg->imageData + h*floatDstImg->widthStep))[w*floatDstImg->nChannels + 1] = (float)g;
			((float *)(floatDstImg->imageData + h*floatDstImg->widthStep))[w*floatDstImg->nChannels + 2] = (float)r;
		}
	}

	if(labDstImg != NULL)
		cvReleaseImage(&labDstImg);

	labDstImg = myBGR2Lab(floatDstImg);

	cvReleaseImage(&floatDstImg);
	cvReleaseImage(&lastDstImg);

	printf("Initialization : done ................\n");
}

void ImageResizing::Src2DstNNFConstruct()
{
	int srcH;
	int srcW;

	srcH = srcImg->height;
	srcW = srcImg->width;
	RandomizedNNSearch rmns;

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
			Patch dst = rmns.SearchPatch(srcImg, dstImg, src);
			nnfS2T[h][w].x = (float)dst.x - src.x; 
			nnfS2T[h][w].y = (float)dst.y - src.y;
		}
	}

	printf("Src2DstNNFConstruct : done ...............\n");
}

void ImageResizing::Dst2SrcNNFConstruct()
{
	int	dstH;
	int dstW;

	dstH = dstImg->height;
	dstW = dstImg->width;
	RandomizedNNSearch rmns;

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
			Patch src = rmns.SearchPatch(dstImg, srcImg, dst);
			nnfT2S[h][w].x = (float)src.x - dst.x; 
			nnfT2S[h][w].y = (float)src.y - dst.y;
		}
	}

	printf("Dst2SrcNNFConstruct : done ...............\n");
}

void ImageResizing::Coherence()
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

	cohereTermForPixel = new LabPixel*[dstH];
	coherePatchNum = new int*[dstH];

	for(int h = 0 ; h < dstH ; h++)
	{
		cohereTermForPixel[h] = new LabPixel[dstW];
		coherePatchNum[h] = new int[dstW];

		for(int w = 0 ; w < dstW ; w++)
		{
			cohereTermForPixel[h][w].l = 0;
			cohereTermForPixel[h][w].a = 0;
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
					
					LabPixel srcP;
					srcP.l = ((float *)(labSrcImg->imageData + src_y*labSrcImg->widthStep))[src_x*labSrcImg->nChannels + 0];
					srcP.a = ((float *)(labSrcImg->imageData + src_y*labSrcImg->widthStep))[src_x*labSrcImg->nChannels + 1];
					srcP.b = ((float *)(labSrcImg->imageData + src_y*labSrcImg->widthStep))[src_x*labSrcImg->nChannels + 2];

					cohereTermForPixel[h][w].l = cohereTermForPixel[h][w].l + weight*srcP.l;
					cohereTermForPixel[h][w].a = cohereTermForPixel[h][w].a + weight*srcP.a;
					cohereTermForPixel[h][w].b = cohereTermForPixel[h][w].b + weight*srcP.b;

					coherePatchNum[h][w]++;
				}
			}
		}
	}

	printf("Coherence : done ...............\n");
}

void ImageResizing::Completeness()
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

	compleTermForPixel = new LabPixel*[dstH];
	complePatchNum = new int*[dstH];

	for(int h = 0 ; h < dstH ; h++)
	{
		compleTermForPixel[h] = new LabPixel[dstW];
		complePatchNum[h] = new int[dstW];

		for(int w = 0 ; w < dstW ; w++)
		{
			compleTermForPixel[h][w].l = 0;
			compleTermForPixel[h][w].a = 0;
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

							LabPixel srcP;
							srcP.l = ((float *)(labSrcImg->imageData + src_h*labSrcImg->widthStep))[src_w*labSrcImg->nChannels + 0];
							srcP.a = ((float *)(labSrcImg->imageData + src_h*labSrcImg->widthStep))[src_w*labSrcImg->nChannels + 1];
							srcP.b = ((float *)(labSrcImg->imageData + src_h*labSrcImg->widthStep))[src_w*labSrcImg->nChannels + 2];

							compleTermForPixel[h][w].l = compleTermForPixel[h][w].l + weight*srcP.l;
							compleTermForPixel[h][w].a = compleTermForPixel[h][w].a + weight*srcP.a;
							compleTermForPixel[h][w].b = compleTermForPixel[h][w].b + weight*srcP.b;
							
							complePatchNum[h][w]++;
						}
					}
					//complePatchNum[h][w] += imgPatchMap[dst_h][dst_w].size();
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

void ImageResizing::UpdateDst()
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

	Coherence();
	Completeness();
	
	IplImage* floatDstImg = cvCreateImage(cvGetSize(dstImg), IPL_DEPTH_32F, dstImg->nChannels);

	for(int h = 0 ; h < dstH ; h++)
	{
		for(int w = 0 ; w < dstW ; w++)
		{
			float l = (compleTermForPixel[h][w].l + cohereTermForPixel[h][w].l)/((float)complePatchNum[h][w]/Ns + (float)coherePatchNum[h][w]/Nt);
			float a = (compleTermForPixel[h][w].a + cohereTermForPixel[h][w].a)/((float)complePatchNum[h][w]/Ns + (float)coherePatchNum[h][w]/Nt);
			float b = (compleTermForPixel[h][w].b + cohereTermForPixel[h][w].b)/((float)complePatchNum[h][w]/Ns + (float)coherePatchNum[h][w]/Nt);

			((float *)(floatDstImg->imageData + h*floatDstImg->widthStep))[w*floatDstImg->nChannels + 0] = l;
			((float *)(floatDstImg->imageData + h*floatDstImg->widthStep))[w*floatDstImg->nChannels + 1] = a;
			((float *)(floatDstImg->imageData + h*floatDstImg->widthStep))[w*floatDstImg->nChannels + 2] = b;
		}
	}

	IplImage* tmpImg = myLab2BGR(floatDstImg);
	cvReleaseImage(&dstImg);
	dstImg = cvCreateImage(cvSize(dstW, dstH), IPL_DEPTH_8U, srcImg->nChannels);

	for(int h = 0 ; h < dstH ; h++)
	{
		for(int w = 0 ; w < dstW ; w++)
		{
			float b = ((float *)(tmpImg->imageData + h*tmpImg->widthStep))[w*tmpImg->nChannels + 0];
			float g = ((float *)(tmpImg->imageData + h*tmpImg->widthStep))[w*tmpImg->nChannels + 1];
			float r = ((float *)(tmpImg->imageData + h*tmpImg->widthStep))[w*tmpImg->nChannels + 2];

			((uchar *)(dstImg->imageData + h*dstImg->widthStep))[w*dstImg->nChannels + 0] = (int)b;
			((uchar *)(dstImg->imageData + h*dstImg->widthStep))[w*dstImg->nChannels + 1] = (int)g;
			((uchar *)(dstImg->imageData + h*dstImg->widthStep))[w*dstImg->nChannels + 2] = (int)r;
		}
	}

	cvReleaseImage(&tmpImg);
	cvReleaseImage(&floatDstImg);
}

ImageResizing::~ImageResizing()
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
	if(labSrcImg != NULL)
		cvReleaseImage(&labSrcImg);
	if(labDstImg != NULL)
		cvReleaseImage(&labDstImg);
}