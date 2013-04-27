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
    
    resizeMat(src_img, 0.4);
    Mat dst = src_img.clone();
//    resizeMat(dst, 0.97);
    

    Mat old_dst = dst.clone().setTo(0);
    int i = 0;
    

    for (int i = 0; i < 10; i ++) {
        
        resizeMat(dst, 0.96);
        
        for (int k = 0; k < 40; k ++) {
            
//            old_dst = dst.clone();
            gradResize(src_img, dst);
            
            showMat(dst, "d", 1);
            
//            cout<<i++<<", ";
            
        }
        
    }
    

    
    

    


    
    
    
}


void BidirectSimilarity::gradResize (const cv::Mat &src, cv::Mat &dst) {
    
    PatchMatch patch_match = PatchMatch();

    
    double NumPatch_src = (src.rows - PATCHWIDTH + 1)*(src.cols - PATCHWIDTH + 1),
           NumPatch_dst = (dst.rows - PATCHWIDTH + 1)*(dst.cols - PATCHWIDTH + 1);
    
    dynamicArray2D<Point> reconst_dst = patch_match.matchImage(src, dst);
    dynamicArray2D<Point> reconst_src = patch_match.matchImage(dst, src);

    
    
    Mat update_dst = dst.clone().setTo(0);
    
    
    for (int i = 0; i < dst.rows; i ++) {
        for (int j = 0; j < dst.cols; j ++) {
            
            Vec3d d_cohere(0, 0, 0), d_complete(0, 0, 0);
            double d_cohere_count = 0, d_complete_count = 0;
            
            for (int m = -PATCHWIDTH/2; m <= PATCHWIDTH/2; m ++) {
                for (int n = -PATCHWIDTH/2; n <= PATCHWIDTH/2; n ++) {
                    
                    if ( i + m >= 0 && i + m < dst.rows
                        && j + n >= 0 && j + n < dst.cols ) {
                        
                        Point cohere_loc = Point(j, i) + reconst_dst.at(i + m, j + n);
                        
                        if ( withinTest(src, cohere_loc) ) {
                            
                            for (int k = 0; k < 3; k ++) {
                               
                                d_cohere[k] += (double)src.at<Vec3b>(cohere_loc)[k];
                                
                            }
//                            d_cohere += (Vec3d)src.at<Vec3b>(cohere_loc);
                            d_cohere_count ++;
                            
                        }
                        
                        

                        
                    }
                    
                }
            }
            
            
//            cout<<Point(j, i)<<endl;
            for (int p = 0; p < src.rows; p ++) {
                for (int q = 0; q < src.cols; q ++) {
                    
                    if ( abs(q + reconst_src.at(p, q).x - j) <= PATCHWIDTH/2 && abs(p + reconst_src.at(p, q).y - i) <= PATCHWIDTH/2 ) {
                        
//                        cout<<Point(q, p)<<", "<<Point(q, p) + Point(j, i) - reconst_src.at(p, q)<<endl;;
                        

                        
                        Point complete_loc = Point(j, i) - reconst_src.at(p, q);
                        
//                        cout<<Point(q, p) + reconst_src.at(p, q)<<", "<<complete_loc<<endl;;
                        
                        if ( withinTest(src, complete_loc) ) {
                        
                            d_complete += (Vec3d)src.at<Vec3b>(complete_loc);
                            d_complete_count ++;
                            
                        }
                        
//                        update_dst.at<Vec3b>(Point(q, p) + reconst_src.at(p, q)) = Vec3b(255, 255, 255);
                        
                    }
                    
                }
            }
            
//            cout<<d_complete_count<<endl;
//            cout<<endl;
//            showMat(update_dst);
//            update_dst.setTo(0);
            
//            cout<<d_cohere<<", "<<d_complete<<endl;
            
            Vec3d temp = d_cohere/NumPatch_dst + d_complete/NumPatch_src;
            temp = temp / ( d_cohere_count/NumPatch_dst + d_complete_count/NumPatch_src );
            
//            Vec3d temp = d_cohere/d_cohere_count;
//            Vec3d temp = d_complete/d_complete_count;
            
//            Vec3d temp = d_cohere/d_cohere_count + d_complete/d_complete_count;
            
//            cout<<temp;
            
            update_dst.at<Vec3b>(i, j) = (Vec3b)temp;
            
        }
    }
    
    
//    showMat(src, "s", 1);
//    showMat(dst, "d", 1);
//    showMat(update_dst);
    
    dst = update_dst.clone();
    
}


bool BidirectSimilarity::withinTest (const cv::Mat &img, const Point &pt) {
    
    bool test = true;
    
    test = pt.x < 0 ? false : true;
    test = pt.y < 0 ? false : true;
    
    test = pt.x >= img.cols ? false : true;
    test = pt.y >= img.rows ? false : true;
    
    return test;
}






















