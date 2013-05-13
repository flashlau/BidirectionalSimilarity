/**
* Code Author: TAKIA
* Time Generate: 7/1/2011, PM 11:08
* Finish Time: accomplished
*/

#ifndef MATH_TOOL_H
#define MATH_TOOL_H

//ax + by + c = 0
class Equation2D
{
public:

	/*** Equation2D Public Data ***/
	// The x coefficient.
	float a;
	// The y coefficient.
	float b;
	// The constant.
	float c;
	
	//*** Equation2D Public Methods ***/
	/**
	* The constructor initializes the data of an equation, including the coefficient a and constant b.
	* @param _a			the x coefficient
	* @param _b			the y coefficient
	* @param _c			the constant
	*/
	Equation2D(float _a = 0, float _b = 0, float _c = 0);

	/**
	* The method gets the y value of the equation.
	* @param x			the x coordinate
	*/
	float GetY(float x);

	/**
	* The method gets the x value of the equation.
	* @param y			the y coordinate
	*/
	float GetX(float y);
};

#endif