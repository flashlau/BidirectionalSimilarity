#ifndef POLYGON2D_H
#define POLYGON2D_H

#include "Point2D.h"
#include <vector>
using namespace std;

class Polygon2D
{
public:
	/*** Polygon2D Public Data ***/
	// The vertice of the polygon.
	vector<Point2D> vertice;

	/*** Point2D Methods ***/
	/**
	* The constructor initializes the data of a polygon.
	* @param _vertice	the vertice set
	*/
	Polygon2D();
	Polygon2D(vector<Point2D> _vertice);


};

#endif