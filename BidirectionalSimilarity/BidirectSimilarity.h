//
//  BidirectSimilarity.h
//  BidirectSimilarity
//
//  Created by Rosani Lin on 12/9/20.
//  Copyright (c) 2012年 Rosani Lin. All rights reserved.
//

#ifndef BidirectSimilarity_BidirectSimilarity_h
#define BidirectSimilarity_BidirectSimilarity_h

#include "Rosaniline.h"
#include "PatchMatch.h"

#include <sstream>

class BidirectSimilarity {
    
public:
    
    BidirectSimilarity();
    
    Mat retargeting (const Mat& input_img, double resize_ratio);
    
    Mat reshuffling (const Mat& src_img, const Mat& shuffled_img);
    
private:


    
    double gradualResize (const cv::Mat &src, cv::Mat &dst, RosMat<Point>& reconst_src, RosMat<Point>& reconst_dst, int itr );
    
    
    double cohereTerm (const Mat& src, const Mat& dst, RosMat<Point>& reconst_dst, Mat& cohere_mat, Mat& cohere_count);
    
    double completeTerm (const Mat& src, const Mat& dst, RosMat<Point>& reconst_src, Mat& complete_mat, Mat& complete_count);
    

    
    RosMat<Point> cohereFindSimilarity (const Mat& src, const Mat& dst);
    
    

    const static int    PATCHWIDTH         = 7;
    const static int    PYRAMID_LEVEL      = 4;
    const static int    MAX_ITERATION      = 40;
    
};



#endif
