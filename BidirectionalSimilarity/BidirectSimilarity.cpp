//
//  BidirectSimilarity.cpp
//  BidirectSimilarity
//
//  Created by Rosani Lin on 12/9/20.
//  Copyright (c) 2012年 Rosani Lin. All rights reserved.
//

#include "BidirectSimilarity.h"

BidirectSimilarity::BidirectSimilarity() {
    
    patchTool = patchTools(PATCHWIDTH, PATCHWIDTH);
    
}


void BidirectSimilarity::summerization(const string &img_path, double resize_ratio) {
    
    src_img = imread(img_path.c_str());
    
    src_img.convertTo(src_img, CV_64FC3);
    src_img /= 255;
    
    resizeMat(src_img, 0.4);
    
    
    Mat dst = src_img.clone();
//    resizeMat(dst, 0.95);
    Mat temp_src = src_img.clone();
    
    for (int k = 0; k <= 10; k ++) {
        
        

        
        resizeMat(dst, 0.95);
        
        for (int i = 0; i < 10; i ++) {
            gradResize(temp_src, dst);
        }
        
        resizeMat(temp_src, 0.95);
        
        showMat(dst, "d", 1);
        
    }

    
    showMat(src_img, "s", 1);S
    showMat(dst);
    
    
    
    PatchMatch patch_match = PatchMatch(PATCHWIDTH, PATCHWIDTH);
    
    dynamicArray2D<Point> Near = patch_match.matchImage(src_img, dst);
    
    patch_match.distanceDispaly(Near);
    
    patch_match.errorDisplay(src_img, dst, Near);
    
    
    
    dynamicArray2D<Point> NearTwo = cohereFindSimilarity(src_img, dst);
    
    patch_match.distanceDispaly(NearTwo);
    
    patch_match.errorDisplay(src_img, dst, NearTwo);
    
    

    


    
    
    
}


void BidirectSimilarity::gradResize (const cv::Mat &src, cv::Mat &dst) {
    
    PatchMatch patch_match = PatchMatch(PATCHWIDTH, PATCHWIDTH);

    
    double NumPatch_src = (src.rows - PATCHWIDTH + 1)*(src.cols - PATCHWIDTH + 1),
           NumPatch_dst = (dst.rows - PATCHWIDTH + 1)*(dst.cols - PATCHWIDTH + 1);
    
    dynamicArray2D<Point> reconst_dst = patch_match.matchImage(src, dst);
    dynamicArray2D<Point> reconst_src = patch_match.matchImage(dst, src);
    
    Mat re_dst = dst.clone().setTo(0);
    
    
    
    for (int i = 0; i < dst.rows; i ++) {
        for (int j = 0; j < dst.cols; j ++) {
            
            
            Vec3d temp_pix = Vec3d(0, 0, 0);
            int valid_count = 0;
            
            for (int m = -PATCHWIDTH; m <= PATCHWIDTH; m ++) {
                for (int n = -PATCHWIDTH; n <= PATCHWIDTH; n ++) {
                    
                    Point Q = Point(j + n, i + m);
                    
                    if ( withinTest(dst, Q) ) {
                        
                        Point P = Point(j, i) + reconst_dst.at(Q);
                        
                        if ( withinTest(src, P) ) {
                            
                            temp_pix += src.at<Vec3d>(P);
                            valid_count ++;
                            
                        }
                        
                    }
                    
                }
            }
            
            
            temp_pix /= valid_count;
//            cout<<temp_pix;
            
            re_dst.at<Vec3d>(i, j) = temp_pix;
            
        }
    }
    
//    showMat(re_dst);    
    
}


dynamicArray2D<Point> BidirectSimilarity::cohereFindSimilarity(const cv::Mat &src, const cv::Mat &dst) {
    
    cout<<"Finding similarity  ";
    
    dynamicArray2D<Point> NearNei = dynamicArray2D<Point>(dst.rows, dst.cols);
    
    int cohereSearchWidth = 11;

    for (int i = 0; i < dst.rows; i ++) {
        
        if ( i % (dst.rows/10) == 0 ) cout<<"|";
        
        for (int j = 0; j < dst.cols; j ++) {
            
            Point min_offset(0);
            double min_cost = INFINITY;
            
            for (int m = -cohereSearchWidth; m <= cohereSearchWidth; m ++) {
                for (int n = -cohereSearchWidth; n <= cohereSearchWidth; n ++) {
                    
                    Point search_loc = Point(j + n, i + m);
                    
                    if ( withinTest(src, search_loc) ) {
                        
                        double local_cost = patchTool.SSD(src, search_loc, dst, Point(j, i));
                        
                        if ( local_cost < min_cost ) {
                            
                            min_cost = local_cost;
                            min_offset = Point(n, m);
                        }
                    }
                    
                    

                    
                    
                }
            }
            
//            Mat temp_s = src.clone().setTo(0);
//            Mat temp_d = dst.clone().setTo(0);
//            
//            patchTool.fill(temp_s, min_offset + Point(j, i), 1);
//            patchTool.fill(temp_d, Point(j, i), 1);
//            
//            showMat(temp_s, "s", 1);
//            showMat(temp_d, "d", 0);
//            
            NearNei.at(i, j) = min_offset;
            
        }
    }
    
    cout<<"   finish.\n";
    
    return NearNei;
}


bool BidirectSimilarity::withinTest (const cv::Mat &img, const Point &pt) {
    
    bool test = true;
    
    test = pt.x < 0 ? false : test;
    test = pt.y < 0 ? false : test;
    
    test = pt.x >= img.cols ? false : test;
    test = pt.y >= img.rows ? false : test;
    
    return test;
}






















