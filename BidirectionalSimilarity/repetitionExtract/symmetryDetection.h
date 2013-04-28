//
//  symmetryDetection.h
//  repetitionExtract
//
//  Created by Rosani Lin on 13/4/16.
//  Copyright (c) 2013年 Rosani Lin. All rights reserved.
//

#ifndef __repetitionExtract__symmetryDetection__
#define __repetitionExtract__symmetryDetection__

#include <iostream>
#include <cmath>
#include "RosaniTools.h"

#define PI 3.1415926535897932384626433832795


class symmetryDetection {

    
public:
    
    symmetryDetection();
    ~symmetryDetection();
    
    double detectSymmetryAngle (const Mat& img);
    
    
private:
    
    
    // returns the min radius that contains whole object in the input image
    int getMinContainingRadius (const Mat& img);
    
    // retures the centroid of input image using firsr order image moment
    Point getCentroid (const Mat& img);
    
    // returns the vector contains all circle points of given redius w.r.t the given centroid
    vector<Point> getCirclePoint (const Point& centroid, int radius);
    
    
    // returns sum( mat[i]*mat[n - i] ) for i = 1...n
    double convolveMat (const Mat& mat);
    
    // shifts all elements one step forward within the given matrix
    void shiftMatElement (Mat& mat);
    
    // returns the mode of the given array
    int mode (const vector<int>& array);
    
    
    // The minimum radius while traveling all radiuses
    static const int INNER_MOST_RADIUS       = 20;
    
    // The minimum radius while sampling circles
    static const int CONCENTRIC_MIN_RADIUS   = 20;
    
    // The sample step
    static const int CONCENTRIC_SAMPLE_STEP  = 3;
    
    
};


#endif /* defined(__repetitionExtract__symmetryDetection__) */
