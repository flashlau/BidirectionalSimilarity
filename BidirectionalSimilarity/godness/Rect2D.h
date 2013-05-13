#ifndef RECT2D_H
#define RECT2D_H

class Rect2D
{
public:
	/*** Rect2D Public Data ***/
	// The minimum and maximum x and y values that indicate the region range.
	int xMin, xMax;
	int yMin, yMax;

	/*** Point2D Methods ***/
	/**
	* The constructor initializes the data of a rectangle.
	* @param _xMin		the x min
	* @param _yMin		the y min
	* @param _xMax		the x max
	* @param _yMax		the y max
	*/
	Rect2D(int _xMin, int _yMin, int _xMax, int _yMax);


};

#endif