//
//  repetitionExtract.h
//  repetitionExtract
//
//  Created by Rosani Lin on 13/4/15.
//  Copyright (c) 2013年 Rosani Lin. All rights reserved.
//

#ifndef __repetitionExtract__repetitionExtract__
#define __repetitionExtract__repetitionExtract__

#include <iostream>
#include "RosaniTools.h"
#include "symmetryDetection.h"


class repetitionExtract {
    
    
public:
    
    repetitionExtract();
    ~repetitionExtract();
    
    // extracts the symmetric element of the image stored in the input path
    void extract(const string& pattern_path);
    
private:
    
    // returns the centroid of the input image
    Point getCentroid (const Mat& img);
    
    // returns the number of symmetric slice of the input image
    int extractNumSlice (const Mat& img, const Point& centroid);
    
    // subfunction of "extractNumSlice", returns the difference after rotation for 360 degrees
    Mat getRotateDiff (const Mat& img , const Point& centroid);
    
    
};

#endif /* defined(__repetitionExtract__repetitionExtract__) */
