#include "Vector2D.h"

Vector2D::Vector2D(float _x, float _y)
	: x(_x), y(_y)
{}

Vector2D Vector2D::operator+(const Vector2D &v) const
{
	return Vector2D(x + v.x, y + v.y);
}

Vector2D &Vector2D::operator+=(const Vector2D &v)
{
	x += v.x; y += v.y;
	return *this;
}

Vector2D Vector2D::operator-(const Vector2D &v) const
{
	return Vector2D(x - v.x, y - v.y);
}

Vector2D &Vector2D::operator-=(const Vector2D &v)
{
	x -= v.x; y -= v.y;
	return *this;
}

bool Vector2D::operator==(const Vector2D &v) const
{
	return x == v.x && y == v.y;
}

Vector2D Vector2D::operator*(float f) const
{
	return Vector2D(f*x, f*y);
}

Vector2D &Vector2D::operator*=(float f)
{
	x *= f; y *= f;
	return *this;
}

Vector2D Vector2D::operator/(float f) const
{
	assert(f!=0);
	float inv = 1.f / f;
	return Vector2D(x * inv, y * inv);
}

Vector2D &Vector2D::operator/=(float f)
{
	assert(f!=0);
	float inv = 1.f / f;
	x *= inv; y *= inv;
	return *this;
}

Vector2D Vector2D::operator-() const
{
	return Vector2D(-x, -y);
}

float Vector2D::LengthSquared() const
{
	return x*x + y*y;
}

float Vector2D::Length() const
{
	return sqrt(LengthSquared());
}

void Vector2D::Normalize()
{
	float l = Length();
	x /= l; y /= l;
}

float Vector2D::Dot(Vector2D v1, Vector2D v2)
{
	return v1.x * v2.x + v1.y * v2.y;
}

float Vector2D::Cross(Vector2D v1, Vector2D v2)
{
	return v1.x * v2.y - v1.y * v2.x;
}