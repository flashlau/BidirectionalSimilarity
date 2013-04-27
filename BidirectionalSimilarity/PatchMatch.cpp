//
//  PatchMatch.cpp
//  PatchMatch
//
//  Created by Rosani Lin on 13/4/27.
//  Copyright (c) 2013年 Rosani Lin. All rights reserved.
//

#include "PatchMatch.h"


PatchMatch::PatchMatch () {
    
    patchTool = patchTools(PATCHWIDTH, PATCHWIDTH);
    srand((int)time(NULL));
    
}

PatchMatch::~PatchMatch () {
    
}

dynamicArray2D<Point> PatchMatch::matchFile (const string& img_src, const string& img_dst) {
    
    Mat src = imread(img_src.c_str()), dst = imread(img_dst.c_str());
    
//    resizeMat(src, 0.6);
//    resizeMat(dst, 0.6);
    
    return matchImage(src, dst);
    
}

dynamicArray2D<Point> PatchMatch::matchImage (const cv::Mat &img_src, const cv::Mat &img_dst) {
    
    dynamicArray2D<Point> NearNei = dynamicArray2D<Point>(img_dst.rows, img_dst.cols);
    dynamicArray2D<Point> NearNei_pre = dynamicArray2D<Point>(img_dst.rows, img_dst.cols);
    
    
    // random initialization of Nearest Neighbor Field
    NearNei.forEach_withCorr([&](Point& nn, int i, int j) {
        
        nn = Point(rand()%img_src.cols - j, rand()%img_src.rows - i);
        
    });
    
    
    while (!convergenceTest(NearNei, NearNei_pre)) {

        NearNei_pre = cloneNearNei(NearNei);
        
        propagation(img_src, img_dst, NearNei);
        
    }
    

    
//    coordinateMappingDisplay(img_src, img_dst, NearNei);
    
    
    
    return NearNei;
}


void PatchMatch::propagation(const cv::Mat &src, const cv::Mat &dst, dynamicArray2D<Point> &NearNei) {
    
    for (int i = 0; i < NearNei.rows; i ++) {
        for (int j = 0; j < NearNei.cols; j ++) {
            
            double local_err = Vec3bDiff(dst.at<Vec3b>(i, j), src.at<Vec3b>(NearNei.at(i, j) + Point(j, i)));
            double upper_err = INFINITY, left_err = INFINITY;
            
            Point new_loc(0);
            
            
            if ( i >= 1 ) {
                
                new_loc = NearNei.at(i - 1, j) + Point(j, i);
                coordinateTrim(src, new_loc);
                
                upper_err = Vec3bDiff(dst.at<Vec3b>(i, j), src.at<Vec3b>(new_loc));
                
                if ( local_err > upper_err ) {
                    
                    NearNei.at(i, j) = new_loc - Point(j, i);
                    local_err = upper_err;
                }
            }
            
            if ( j >= 1 ) {
                
                new_loc = NearNei.at(i, j - 1) + Point(j, i);
                coordinateTrim(src, new_loc);
                
                left_err = Vec3bDiff(dst.at<Vec3b>(i, j), src.at<Vec3b>(new_loc));
            
                if ( local_err > left_err ) {
                    
                    NearNei.at(i, j) = new_loc - Point(j, i);
                    local_err = left_err;
                }
                
            }
            
            randomSearch(src, dst, NearNei.at(i, j), Point(j, i), local_err);
            
        }
    }
    
    
    for (int i = NearNei.rows - 1; i >= 0; i --) {
        for (int j = NearNei.cols - 1; j >= 0; j --) {
            
            double local_err = Vec3bDiff(dst.at<Vec3b>(i, j), src.at<Vec3b>(NearNei.at(i, j) + Point(j, i)));
            double lower_err = INFINITY, right_err = INFINITY;
            
            Point new_loc(0);
            
            if ( i < NearNei.rows - 1 ) {
                
                new_loc = NearNei.at(i + 1, j) + Point(j, i);
                coordinateTrim(src, new_loc);
                
                lower_err = Vec3bDiff(dst.at<Vec3b>(i, j), src.at<Vec3b>(new_loc));
                
                if ( local_err > lower_err ) {
                    
                    NearNei.at(i, j) = new_loc - Point(j, i);
                    local_err = lower_err;
                }
            }
            
            if ( j < NearNei.cols - 1 ) {
                
                new_loc = NearNei.at(i, j + 1) + Point(j, i);
                coordinateTrim(src, new_loc);
                
                right_err = Vec3bDiff(dst.at<Vec3b>(i, j), src.at<Vec3b>(new_loc));
                
                if ( local_err > right_err ) {
                    
                    NearNei.at(i, j) = new_loc - Point(j, i);
                    local_err = right_err;
                }
                
            }
            
            randomSearch(src, dst, NearNei.at(i, j), Point(j, i), local_err);
            
        }
    }
    
    
    
}


void PatchMatch::randomSearch (const Mat& src, const Mat& dst, Point& NearNei_pt, const Point loc, double local_err) {
    
    double alpha = 1.0, w = MAX(dst.rows, src.rows), random_err = INFINITY;
    
    while ( alpha * w >= 1 ) {
        
        Point random_loc = Point( NearNei_pt.x*(rand()%3 - 1), NearNei_pt.y*(rand()%3 - 1));
        random_loc += loc;
        random_loc = Point((random_loc.x + src.cols)%src.cols, (random_loc.y + src.rows)%src.rows);
        
        random_err = Vec3bDiff(src.at<Vec3b>(random_loc), dst.at<Vec3b>(loc));
        
        if ( local_err > random_err ) {
            
            NearNei_pt = random_loc - loc;
            local_err = random_err;
        }
        
        alpha /= 2.0;
    }
    
    NearNei_pt = NearNei_pt + loc;
    coordinateTrim(src, NearNei_pt);
    NearNei_pt = NearNei_pt - loc;
    
}


bool PatchMatch::convergenceTest(dynamicArray2D<Point> &NearNei_new, dynamicArray2D<Point> &NearNei_old) {
    
    bool test = true;
    
    NearNei_new.forEach_withCorr([&](Point nn, int i, int j) {
        
        if ( nn != NearNei_old.at(i, j) ) {
            test = false;
        }
        
    });
    
    return test;
}


void PatchMatch::coordinateMappingDisplay (const Mat& src, const Mat& dst, dynamicArray2D<Point>& NearNei) {
    
    Mat mapped = dst.clone().setTo(0);
    
    NearNei.forEach_withCorr([&] (Point nn, int i, int j) {
        
        mapped.at<Vec3b>(i, j) = src.at<Vec3b>(nn + Point(j, i));
        
    });
    
    showMat(dst, "dst", 1);
    showMat(mapped);
    
}


dynamicArray2D<Point> PatchMatch::cloneNearNei(dynamicArray2D<Point> &NearNei) {
    
    dynamicArray2D<Point> temp = dynamicArray2D<Point>(NearNei.rows, NearNei.cols);
    
    temp.forEach_withCorr([&](Point& nn, int i, int j) {
        
        nn = NearNei.at(i, j);
        
    });
    
    return temp;
}


void PatchMatch::coordinateTrim(const cv::Mat &src, Point &pt) {
    
    pt.x = pt.x < 0 ? 0 : pt.x;
    pt.y = pt.y < 0 ? 0 : pt.y;
    
    pt.x = pt.x >= src.cols ? src.cols - 1 : pt.x;
    pt.y = pt.y >= src.rows ? src.rows - 1 : pt.y;
    
}






