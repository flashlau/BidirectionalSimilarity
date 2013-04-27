//
//  PatchMatch.h
//  PatchMatch
//
//  Created by Rosani Lin on 13/4/27.
//  Copyright (c) 2013年 Rosani Lin. All rights reserved.
//

#ifndef __PatchMatch__PatchMatch__
#define __PatchMatch__PatchMatch__

#include <iostream>
#include "RosaniTools.h"


class PatchMatch {
    
public:
    
    PatchMatch ();
    ~PatchMatch ();
    
    dynamicArray2D<Point> matchFile (const string& img_src, const string& img_dst);
    
    dynamicArray2D<Point> matchImage (const Mat& img_src, const Mat& img_dst);
    
    void coordinateMappingDisplay (const Mat& src, const Mat& dst, dynamicArray2D<Point>& NearNei);
    
    
private:
    
    
    void propagation (const Mat& src, const Mat& dst, dynamicArray2D<Point>& NearNei);
    
    void randomSearch (const Mat& src, const Mat& dst, Point& NearNei_pt, const Point loc, double local_err);
    
    bool convergenceTest (dynamicArray2D<Point>& NearNei_new, dynamicArray2D<Point>& NearNei_old);
    
    
    
    void coordinateTrim (const Mat& src, Point& pt);
    
    dynamicArray2D<Point> cloneNearNei (dynamicArray2D<Point>& NearNei);
    
    
    patchTools patchTool;
    
    constexpr static int PATCHWIDTH  =  7;

};

#endif /* defined(__PatchMatch__PatchMatch__) */
