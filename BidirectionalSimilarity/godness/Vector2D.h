/**
* Code Author: Sheng-Jie Luo
* Time Generate: 4/17/2011, PM 12:38
*/

#ifndef VECTOR2D_H
#define VECTOR2D_H

#include <assert.h>
#include <math.h>

class Vector2D
{
public:
	/*** Vector2D Public Data ***/
	float x, y;

	/*** Vector2D Public Methods ***/
	/**
	* The constructor initializes the data of a vector, including the x and y.
	* @param _x			the x
	* @param _y			the y
	*/
	Vector2D(float _x=0, float _y=0);

	/**
	* The operator add a vector.
	* @param v			the vector to add
	* @return			the result
	*/
	Vector2D operator+(const Vector2D &v) const;
	Vector2D& operator+=(const Vector2D &v);

	/**
	* The operator substract a vector.
	* @param v			the vector to substract
	* @return			the result
	*/
	Vector2D operator-(const Vector2D &v) const;
	Vector2D& operator-=(const Vector2D &v);

	bool operator==(const Vector2D &v) const;
	
	/**
	* The operator multiply a vector.
	* @param v			the vector to multiply
	* @return			the result
	*/
	Vector2D operator*(float f) const;
	Vector2D &operator*=(float f);

	/**
	* The operator divide a vector.
	* @param v			the vector to divide
	* @return			the result
	*/
	Vector2D operator/(float f) const;
	Vector2D &operator/=(float f);

	/**
	* The operator of minus.
	* @return			the vector that multiply -1 on the original vector
	*/
	Vector2D operator-() const;

	/**
	* Compute the squared length of the vector.
	* @return			the squared length of the vector
	*/
	float LengthSquared() const;

	/**
	* Compute the length of the vector.
	* @return			the length of the vector
	*/
	float Length() const;

	/**
	* Normalize the vector.
	*/
	void Normalize();

	/**
	* Compute the dot product of two vector.
	* @return			the dot product
	*/
	static float Dot(Vector2D v1, Vector2D v2);

	/**
	* Compute the cross product of two vector.
	* @return			the cross product
	*/
	static float Cross(Vector2D v1, Vector2D v2);
};

#endif