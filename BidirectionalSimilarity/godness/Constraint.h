#ifndef CONSTRAINT_H
#define CONSTRAINT_H

#include "Line2D.h"
#include "Vector2D.h"
#include "Polygon2D.h"

struct LineConstraint
{
	Point2D srcEndPoint1;
	Point2D srcEndPoint2;

	vector<Point2D> srcPixelSet;
	vector<Point2D> nnPi;
	vector<Point2D> qj;

	LineConstraint(Point2D _endp1, Point2D _endp2)
		:srcEndPoint1(_endp1.x, _endp1.y), srcEndPoint2(_endp2.x, _endp2.y){}

	void PushDstLinePoint(Vector2D** nnfS2T, Vector2D** nnfT2S, IplImage* dstImg)
	{
		nnPi.clear();
		qj.clear();
		
		for(int i = 0 ; i < srcPixelSet.size() ; i++)
		{
			int y = srcPixelSet[i].y;
			int x = srcPixelSet[i].x;
			
			Point2D nnP(srcPixelSet[i].x + nnfS2T[y][x].x, srcPixelSet[i].y + nnfS2T[y][x].y);
			nnPi.push_back(nnP);	
		}
		
		Line2D line(srcEndPoint1, srcEndPoint2);

		for(int h = 0 ; h < dstImg->height ; h++)
		{
			for(int w = 0 ; w < dstImg->width ; w++)
			{
				Point2D srcP(w + nnfT2S[h][w].x, h + nnfT2S[h][w].y);

				if(line.PointIsOnLine(srcP) == true) qj.push_back(Point2D(w, h));
			}
		}
	}
};

/**
* The structure implements the hard constraint.
*/
struct HardConstraint
{
	HardConstraint(){}
	// The polygon.
	Polygon2D polygon;
	// The shift vector.
	Vector2D shift;
};

#endif