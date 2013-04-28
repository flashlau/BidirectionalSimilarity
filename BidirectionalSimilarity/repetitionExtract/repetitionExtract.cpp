//
//  repetitionExtract.cpp
//  repetitionExtract
//
//  Created by Rosani Lin on 13/4/15.
//  Copyright (c) 2013年 Rosani Lin. All rights reserved.
//

#include "repetitionExtract.h"


repetitionExtract::repetitionExtract () {
    
}

repetitionExtract::~repetitionExtract () {
    
}


void repetitionExtract::extract (const string &pattern_path) {
    
    Mat input_pattern, pattern;
    input_pattern = imread(pattern_path.c_str());
    
    cvtColor(input_pattern, pattern, CV_BGR2GRAY);
    resizeMat(pattern, 0.4);
    
    
    int slices = extractNumSlice(pattern, getCentroid(pattern));
    
    cout<<"Detected slices = "<<slices<<endl;
    
    
    symmetryDetection symDetect = symmetryDetection();
    double sym_angle = symDetect.detectSymmetryAngle(pattern);

    
    
    for (int i = 0; i < slices/2; i ++) {

        infLine(pattern, getCentroid(pattern), tan((i*(360/slices) + sym_angle)/180*PI));
    }
    
    showMat(pattern);
    

    
    
    
}


Point repetitionExtract::getCentroid(const cv::Mat &img) {
    
    
    Moments mo = moments(img);
    
    return Point(mo.m10/mo.m00, mo.m01/mo.m00);

}





int repetitionExtract::extractNumSlice(const cv::Mat &img, const Point &centroid) {
    
    
    Mat shrink_img = img.clone();
    int slices = 0;

    resizeMat(shrink_img, 0.5);

    
    Mat rotate_diff = getRotateDiff(shrink_img, getCentroid(shrink_img));
    
    threshold(rotate_diff, rotate_diff, 0, 255, THRESH_OTSU);
    
//    showMat(rotate_diff);
    
    for (int i = 1; i < rotate_diff.rows; i ++) {
        
        if ( rotate_diff.at<uchar>(i, 0) > rotate_diff.at<uchar>(i - 1, 0)) {
            slices ++;
        }
    }
    
    
    return slices;
}


Mat repetitionExtract::getRotateDiff(const cv::Mat &img, const Point &centroid) {
    
    Mat ang_diff = Mat(360, 1, CV_64FC1);
    double diff_max = 0.0, diff_min = 0.0;
    
    for (double ang = 0; ang < 360.0; ang += 1) {
        
        Mat rotate_mat = getRotationMatrix2D(centroid, ang, 1.0);
        
        Mat rotated_img = img.clone().setTo(0);
        
        warpAffine(img, rotated_img, rotate_mat, img.size());
        
        
        Mat diff = img.clone().setTo(0);
        absdiff(img, rotated_img, diff);
        
        ang_diff.at<double>(ang, 0) = sum(diff)[0];
        
    }
    
    
    minMaxIdx(ang_diff, &diff_min, &diff_max);
    
    ang_diff = (ang_diff - diff_min)*(255/(diff_max - diff_min));
    
    ang_diff.convertTo(ang_diff, CV_8UC1);
    
    
    return ang_diff;
    
}





