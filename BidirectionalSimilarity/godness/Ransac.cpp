#include "Ransac.h"
#include "Line2D.h"
#include <math.h>
#include <time.h>

Ransac::Ransac(vector<Point2D> &_pixelSet, RansacModel _model)
{
	float line_threshold = 2;
	float region_threshold = 2;

	switch(_model)
	{
	case LINE_MODEL:
		SetParameters(2, _pixelSet, line_threshold, _model);
		break;
	case REGION_MODEL:
		SetParameters(3, _pixelSet, region_threshold, _model);
		break;
	};
}

void Ransac::SetParameters(int _sample, vector<Point2D> &_pixelSet, float _threshold, RansacModel _model)
{
	sample = _sample;
	threshold = _threshold;
	model = _model;
	kTimes = 200;
	maxInlier = 0;
	pixelSet = _pixelSet;
}

void Ransac::Run(vector<Point2D> &maxInlierSet, vector<Point2D> &minOutlierSet)
{
	srand(time(NULL));
	sampleSet.clear();

	for(int k = 0 ; k < kTimes ; k++)
	{
		inlierSet.clear();
		outlierSet.clear();
		sampleSet.clear();

		for(int i = 0 ; i < sample ; i++)
		{
			int num = rand() % pixelSet.size();
			sampleSet.push_back(num);
		}

		int tmpInlier = CountInlier();
	
		if(tmpInlier > maxInlier)
		{
			maxInlierSet.clear();
			minOutlierSet.clear();

			if(model == LINE_MODEL)
			{
				if(modelLine != NULL) delete [] modelLine;
				modelLine = new Line2D(pixelSet[sampleSet[0]], pixelSet[sampleSet[1]]);
			}
			else
			{
			}

			maxInlier = tmpInlier;
			maxInlierSet = inlierSet;
			minOutlierSet = outlierSet;
		}
	}

	printf("Ransac : done......\n");
}

int Ransac::CountInlier()
{
	int inlier = 0;
	
	if(model == LINE_MODEL)
	{
		Point2D p1 = pixelSet[sampleSet[0]];
		Point2D p2 = pixelSet[sampleSet[1]];

		Line2D line(p1, p2);

		for(int i = 0 ; i < pixelSet.size() ; i++)
		{
			if( i == sampleSet[0] || i == sampleSet[1])
				continue;

			float dist = line.Distance(pixelSet[i]);
			
			if(dist <= threshold && line.InLineBoundary(pixelSet[i]))
			{
				inlier++;
				inlierSet.push_back(pixelSet[i]);
			}
			else
			{
				outlierSet.push_back(pixelSet[i]);
			}
		}
		
	}
	else if (model == REGION_MODEL)
	{

	}

	return inlier;
}
Line2D Ransac::GetModelLine()
{
	Line2D model(modelLine->p1, modelLine->p2);

	return model;
}

