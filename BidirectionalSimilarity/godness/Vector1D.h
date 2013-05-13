#ifndef VECTOR1D_H
#define VECTOR1D_H

#include <assert.h>
#include <math.h>

class Vector1D{

public:
	/*** Vector1D Public Data ***/
	float x;

	/*** Vector1D Public Methods ***/
	/**
	* The constructor initializes the data of a vector, including only x.
	* @param _x			the x
	*/
	Vector1D(float _x=0);

	/**
	* The operator add a vector.
	* @param v			the vector to add
	* @return			the result
	*/
	Vector1D operator+(const Vector1D &v) const;
	Vector1D& operator+=(const Vector1D &v);

		/**
	* The operator substract a vector.
	* @param v			the vector to substract
	* @return			the result
	*/
	Vector1D operator-(const Vector1D &v) const;
	Vector1D& operator-=(const Vector1D &v);

	bool operator==(const Vector1D &v) const;
	
	/**
	* The operator multiply a vector.
	* @param v			the vector to multiply
	* @return			the result
	*/
	Vector1D operator*(float f) const;
	Vector1D &operator*=(float f);

	/**
	* The operator divide a vector.
	* @param v			the vector to divide
	* @return			the result
	*/
	Vector1D operator/(float f) const;
	Vector1D &operator/=(float f);

	/**
	* The operator of minus.
	* @return			the vector that multiply -1 on the original vector
	*/
	Vector1D operator-() const;
	
};

#endif