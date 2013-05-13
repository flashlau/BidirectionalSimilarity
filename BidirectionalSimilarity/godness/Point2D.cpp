#include "Point2D.h"

#include <assert.h>
#include <math.h>

Point2D::Point2D(float _x, float _y)
	: x(_x), y(_y)
{}

Point2D &Point2D::operator+=(const Point2D &p)
{
	x += p.x; y += p.y;
	return *this;
}
Point2D Point2D::operator+(const Point2D &p) const
{
	return Point2D(x + p.x, y + p.y);
}
Point2D &Point2D::operator-=(const Point2D &p)
{
	x -= p.x; y -= p.y;
	return *this;
}
Point2D Point2D::operator-(const Point2D &p) const
{
	return Point2D(x - p.x, y - p.y);
}
Point2D Point2D::operator* (float f) const
{
	return Point2D(f * x, f * y);
}
Point2D &Point2D::operator*=(float f)
{
	x *= f; y *= f;
	return *this;
}
Point2D Point2D::operator/ (float f) const
{
	assert(f!=0);
	float inv = 1.f/f;
	return Point2D(inv*x, inv*y);
}
Point2D &Point2D::operator/=(float f) 
{
	assert(f!=0);
	float inv = 1.f/f;
	x *= inv; y *= inv;
	return *this;
}