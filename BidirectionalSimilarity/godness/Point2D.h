/**
* Code Author: Sheng-Jie Luo
* Time Generate: 4/17/2011, PM 12:21
*/

#ifndef POINT2D_H
#define POINT2D_H

/**
* The class is the base of any point-based primitive.
*/
class Point2D
{
public:
	/*** Point2D Public Data ***/
	// The x coordinate.
	float x;
	// The y coordinate.
	float y;

	/*** Point2D Methods ***/
	/**
	* The constructor initializes the data of a point, including the position x and y.
	* @param _x			the x coordinate
	* @param _y			the y coordinate
	*/
	Point2D(float _x = 0, float _y = 0);

	/**
	* The operator add a point.
	* @param p			the point to add
	* @return			the result
	*/
	Point2D &operator+=(const Point2D &p);
	Point2D operator+(const Point2D &p) const;

	/**
	* The operator subtract a point.
	* @param p			the point to subtract
	* @return			the result
	*/
	Point2D &operator-=(const Point2D &p);
	Point2D operator-(const Point2D &p) const;

	/**
	* The operator multiply a point.
	* @param p			the point to multiply
	* @return			the result
	*/
	Point2D operator* (float f) const;
	Point2D &operator*=(float f);

	/**
	* The operator divide a point.
	* @param p			the point to divide
	* @return			the result
	*/
	Point2D operator/ (float f) const;
	Point2D &operator/=(float f);
};

#endif