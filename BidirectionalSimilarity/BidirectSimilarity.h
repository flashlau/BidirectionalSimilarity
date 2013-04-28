//
//  BidirectSimilarity.h
//  BidirectSimilarity
//
//  Created by Rosani Lin on 12/9/20.
//  Copyright (c) 2012年 Rosani Lin. All rights reserved.
//

#ifndef BidirectSimilarity_BidirectSimilarity_h
#define BidirectSimilarity_BidirectSimilarity_h

#include "RosaniTools.h"
#include "PatchMatch.h"

class BidirectSimilarity {
    
public:
    
    BidirectSimilarity();
    
    void summerization (const string &img_path, double resize_ratio);
    
    
private:

    
    void gradResize (const Mat& src, Mat& dst);
    
    bool withinTest (const Mat& img, const Point& pt);
    
    dynamicArray2D<Point> cohereFindSimilarity (const Mat& src, const Mat& dst);
    
    
    Mat src_img;
    
    patchTools patchTool;
    
    

    constexpr static const int    PATCHWIDTH         = 7;
    
};



#endif
