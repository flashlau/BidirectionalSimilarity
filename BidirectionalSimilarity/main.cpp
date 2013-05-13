//
//  main.cpp
//  BidirectionalSimilarity
//
//  Created by Rosani Lin on 13/4/28.
//  Copyright (c) 2013年 Rosani Lin. All rights reserved.
//

#include <iostream>
#include "BidirectSimilarity.h"


RosMat<int> kk (RosMat<int> T);


int main(int argc, const char * argv[])
{
    
    Mat img = imread("/Users/xup6qup3/Dropbox/code/Dev.temp/BidirectionalSimilarity/BidirectionalSimilarity/rcircle.jpg");
    
    img.convertTo(img, CV_64FC3);
    img /= 255;
    
    
    BidirectSimilarity bi_similarity = BidirectSimilarity();
    bi_similarity.retargeting(img, 0.3);
    
    



    
//    Mat im = img.clone().setTo(0);
//    
//    r_time time;
//    
//    time.start();
//    cout<<norm(img, im, NORM_L2);
//    time.end();
//    double t_a = time.show();
//    
//    double temp = 0.0;
//    
//    time.start();
//
//        
//        double local = 0.0;
//        
//        for (int i = 0; i < img.rows; i ++) {
//            for (int j = 0; j < img.cols; j ++) {
//                
//                temp += Vec3dDiff(img.at<Vec3d>(i, j), im.at<Vec3d>(i, j));
//                
//            }
//        }
//    
//    
//    cout<<sqrt(temp)<<endl;
//
//    time.end();
//    double t_b = time.show();
//    cout<<t_a<<", "<<t_b<<", "<<t_b/t_a;
    
    
    


    return 0;
}

