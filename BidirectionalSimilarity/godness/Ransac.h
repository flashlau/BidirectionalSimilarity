#ifndef RANSAC_H
#define RANSAC_H

#include <iostream>
#include <vector>
#include "Point2D.h"
#include "Line2D.h"

using namespace std;

enum RansacModel
{
	LINE_MODEL,
	REGION_MODEL
};

class Ransac
{
public:

	Ransac(vector<Point2D> &_pixelSet, RansacModel model);
	void Run(vector<Point2D> &maxInlier, vector<Point2D> &minOutlier);
	Line2D GetModelLine();

private:
	
	void SetParameters(int _sample, vector<Point2D> &pixelSet, float _threshold, RansacModel _model);
	int CountInlier();

	int sample, maxInlier, kTimes;
	float threshold;
	RansacModel model;
	
	Line2D* modelLine;
	
	vector<Point2D> pixelSet;
	vector<Point2D> inlierSet;
	vector<Point2D> outlierSet;
	vector<int> sampleSet;
};


#endif