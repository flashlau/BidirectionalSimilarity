#include "Patch.h"
#include <math.h>

Patch::Patch(int _x, int _y, int _size, IplImage* _srcImg, IplImage* _maskImg, double _weight, ColorDistanceModel _colorModel)
	:x(_x), y(_y), size(_size), weight(_weight), colorModel(_colorModel)
{
	srcImg = _srcImg;
	maskImg = _maskImg;
}

RgbPixel Patch::GetRgbPixel(int x, int y)
{
	RgbPixel p;

	if( x >= 0 && x < srcImg->width && y >= 0 && y < srcImg->height)
	{
		p.b = ((uchar *)(srcImg->imageData + y*srcImg->widthStep))[x*srcImg->nChannels + 0]; 
		p.g = ((uchar *)(srcImg->imageData + y*srcImg->widthStep))[x*srcImg->nChannels + 1];
		p.r = ((uchar *)(srcImg->imageData + y*srcImg->widthStep))[x*srcImg->nChannels + 2];
	}
	else
	{
		p.r = p.g = p.b = 1000;
	}

	return p;
}

int Patch::GetMaskPixel(int x, int y)
{
	if(maskImg == NULL) return 1;

	int v = 0;
	if( x >= 0 && x < maskImg->width && y >= 0 && y < maskImg->height)
	{
		v = ((uchar *)(maskImg->imageData + y*maskImg->widthStep))[x*maskImg->nChannels + 0]; 
	}
	else
	{
		v = 0;
	}

	return v;
}

float Patch::Distance(Patch p)
{
	float sum = 0;
	float count = 0;

	/*Patch not exist*/
	if(x < 0 || x > (srcImg->width - 1) || y < 0 || y > (srcImg->height - 1))
		return HUGE_VAL;

	/*Patch not exist*/
	if(p.x < 0 || p.x > (p.srcImg->width - 1) || p.y < 0 || p.y > (p.srcImg->height -1))
		return HUGE_VAL;

	for(int h = -size/2 ; h <= size/2; h++)
	{
		for(int w = -size/2 ; w <= size/2; w++)
		{
			RgbPixel pixel1, pixel2;
			int maskVal;

			/*if(x + w < 0 || x + w >= this->srcImg->width || y + h < 0 || y + h >= this->srcImg->height)
				continue;

			if(p.x + w < 0 || p.x + w >= p.srcImg->width || p.y + h < 0 || p.y + h >= p.srcImg->height)
				continue;*/

			pixel1 = this->GetRgbPixel(x + w, y + h);
			pixel2 = p.GetRgbPixel(p.x + w, p.y + h);
			maskVal = (maskImg == NULL) ? 1 : this->GetMaskPixel(x + w, y + h);

			float dr = pixel1.r - pixel2.r;
			float dg = pixel1.g - pixel2.g;
			float db = pixel1.b - pixel2.b;

			switch(colorModel)
			{
			case GRAYSCALE:
				sum = (maskVal == 1) ? sum + abs(0.3*dr + 0.59*dg + 0.11*db) : sum;				
				break;
			case RGB_COLOR:
				sum = (maskVal == 1) ? sum + sqrt(dr*dr + dg*dg + db*db) : sum;
				break;
			};

			count = count + maskVal;
		}
	}
	
	return sum / count;
}

float Patch::Distance(Patch a, Patch b)
{
	float sum = 0;
	float count = 0;

	/*Patch not exist*/
	if(a.x < 0 || a.x > (a.srcImg->width - 1) || a.y < 0 || a.y > (a.srcImg->height - 1))
		return HUGE_VAL;

	/*Patch not exist*/
	if(b.x < 0 || b.x > (b.srcImg->width - 1) || b.y < 0 || b.y > (b.srcImg->height - 1))
		return HUGE_VAL;

	for(int h = -a.size/2 ; h <= a.size/2; h++)
	{
		for(int w = -a.size/2 ; w <= a.size/2; w++)
		{
			RgbPixel pixel1, pixel2;
			int maskVal;

			/*if(a.x + w < 0 || a.x + w >= a.srcImg->width || a.y + h < 0 || a.y + h >= a.srcImg->height)
				continue;

			if(b.x + w < 0 || b.x + w >= b.srcImg->width || b.y + h < 0 || b.y + h >= b.srcImg->height)
				continue;*/

			pixel1 = a.GetRgbPixel(a.x + w, a.y + h);
			pixel2 = b.GetRgbPixel(b.x + w, b.y + h);
			maskVal = (a.maskImg == NULL) ? 1 : a.GetMaskPixel(a.x + w, a.y + h);

			float dr = pixel1.r - pixel2.r;
			float dg = pixel1.g - pixel2.g;
			float db = pixel1.b - pixel2.b;
			
			
			switch(a.colorModel)
			{
			case GRAYSCALE:
				sum = (maskVal == 1) ? sum + (0.3*dr + 0.59*dg + 0.11*db) : sum;				
				break;
			case RGB_COLOR:
				sum = (maskVal == 1) ? sum + sqrt(dr*dr + dg*dg + db*db) : sum;
				break;
			};

			count = count + maskVal;

			//printf("dr = %f, dg = %f, db = %f\n", dr, dg, db);
			//printf("count = %f, sum = %f\n", count, sum);
		}
	}
	//printf("sum/count = %f", sum/count);
	//system("pause");

	return sum / count;
}

