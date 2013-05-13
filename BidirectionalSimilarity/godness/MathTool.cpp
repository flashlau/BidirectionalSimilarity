#include "MathTool.h"

Equation2D::Equation2D(float _a, float _b, float _c)
	: a(_a), b(_b), c(_c)
{
}

float Equation2D::GetX(float y)
{
	return (-b*y - c) / a; 
}

float Equation2D::GetY(float x)
{
	return (-c -a*x) / b;
}
