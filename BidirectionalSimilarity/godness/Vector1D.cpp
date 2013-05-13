#include "Vector1D.h"

Vector1D::Vector1D(float _x): x(_x)
{}

Vector1D Vector1D::operator+(const Vector1D &v) const
{
	return Vector1D(x + v.x);
}

Vector1D &Vector1D::operator+=(const Vector1D &v)
{
	x += v.x; 
	return *this;
}

Vector1D Vector1D::operator-(const Vector1D &v) const
{
	return Vector1D(x - v.x);
}

Vector1D &Vector1D::operator-=(const Vector1D &v)
{
	x -= v.x;
	return *this;
}

bool Vector1D::operator==(const Vector1D &v) const
{
	return x == v.x;
}

Vector1D Vector1D::operator*(float f) const
{
	return Vector1D(f*x);
}

Vector1D &Vector1D::operator*=(float f)
{
	x *= f;
	return *this;
}

Vector1D Vector1D::operator/(float f) const
{
	assert(f!=0);
	float inv = 1.f / f;
	return Vector1D(x * inv);
}

Vector1D &Vector1D::operator/=(float f)
{
	assert(f!=0);
	float inv = 1.f / f;
	x *= inv;
	return *this;
}

Vector1D Vector1D::operator-() const
{
	return Vector1D(-x);
}