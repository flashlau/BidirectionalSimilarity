/**
* Code Author: TAKIA
* Time Generate: 7/1/2011, PM 11:08
* Finish Time: accomplished
*/

#ifndef LINE2D_H
#define LINE2D_H

#include "MathTool.h"
#include "Point2D.h"

class Line2D
{
public:
	
	/*** Line2D Public Data ***/
	// The point p1 of line.
	Point2D p1;
	// The another point p2 of line.
	Point2D p2;

	/*** Line2D Public Methods ***/
	/**
	* The constructor initializes the data of a line, including the point p1 and another point p2.
	* @param _p1		the point of the line
	* @param _p2		the point of the line
	*/
	Line2D(Point2D _p1, Point2D _p2);

	/**
	* The method return the line equation calculated by p1 and p2;
	*/
	Equation2D GetLineEquation();

	/**
	* The method return project distance of the point to line;
	* @param _p			the Point2D
	*/
	float Distance(Point2D p);
	
	/**
	* The method return whether the point is on line.
	* @param _p			the Point2D
	*/
	bool PointIsOnLine(Point2D p);
	
	/**
	* The method return whether the point which point of out line projecting to the line.
	* @param _p			the Point2D
	*/
	Point2D ProjectPoint(Point2D p);
	
	/**
	* The method return whether the point which point of out line projecting to the line.
	* @param _p			the Point2D
	*/
	bool InLineBoundary(Point2D p);

private:

	/*** Line2D Private Data ***/
	// The line equation.
	Equation2D f;
};

#endif