#include "Line2D.h"
#include "stdlib.h"
#include "Utility.h"
#include <math.h>
#include <iostream>

using namespace std;

Line2D::Line2D(Point2D _p1, Point2D _p2)
	: p1(_p1), p2(_p2)
{
	f.b = 1;
	f.a = -(p1.y - p2.y)/(p1.x - p2.x);
	f.c = - f.a * p1.x - p1.y;
}

Equation2D Line2D::GetLineEquation()
{
	return this->f;
}

float Line2D::Distance(Point2D p)
{
	return abs(f.a*(float)p.x + f.b*(float)p.y + f.c) / sqrt(f.a*f.a + f.b*f.b);
}

bool Line2D::InLineBoundary(Point2D p)
{
	int minX = GetMin(p1.x, p2.x);
	int minY = GetMin(p1.y, p2.y);
	int maxX = GetMax(p1.x, p2.x);
	int maxY = GetMax(p1.y, p2.y);

	return (p.x >= minX && p.x <= maxX && p.y >= minY && p.y <= maxY) ? true : false;
}

bool Line2D::PointIsOnLine(Point2D p)
{
	return (f.a * p.x + f.b * p.y + f.c <= 1 && f.a * p.x + f.b * p.y + f.c >= -1 && InLineBoundary(p)) ? true : false;
}

Point2D Line2D::ProjectPoint(Point2D p)
{
	float t = (-f.a*p.x -f.b*p.y - f.c)/(f.a*f.a + f.b*f.b);

	return Point2D( p.x + f.a*t, p.y + f.b*t);
}