//
//  BidirectSimilarity.cpp
//  BidirectSimilarity
//
//  Created by Rosani Lin on 12/9/20.
//  Copyright (c) 2012年 Rosani Lin. All rights reserved.
//

#include "BidirectSimilarity.h"

BidirectSimilarity::BidirectSimilarity() {}


Mat BidirectSimilarity::retargeting (const cv::Mat &input_img, double resize_ratio) {
    
    Mat src = input_img.clone();
    
    if ( src.type() != CV_64FC3 ){
        
        src.convertTo(src, CV_64FC3);
        src /= 255;
    }
    
    
    Mat dst = src.clone();
    
    int shrink_itr = 0;
    do {
        
        cout<<"shrink level = "<<shrink_itr<<endl;
        
        Mat temp_src = dst.clone();
        resizeMat(dst, 0.95);
                
        
        vector<Mat> src_pyr = constructPyramid(temp_src, PYRAMID_LEVEL);
        vector<Mat> dst_size = constructPyramid(dst, PYRAMID_LEVEL);
        
        
        for (int scale_itr = 0; scale_itr <= PYRAMID_LEVEL; scale_itr ++) {
            
            if ( dst.rows < PATCHWIDTH + 2 && dst.cols < PATCHWIDTH + 5 ) {
                continue;
            }
            
            resize(dst, dst, dst_size[scale_itr].size());
            
            cout<<src_pyr[scale_itr].size()<<", "<<dst.size()<<endl;
            
            cout<<"scale level = "<<scale_itr<<endl<<"itr = ";
            
            RosMat<Point> reconst_src = RosMat<Point>(src_pyr[scale_itr].size());
            RosMat<Point> reconst_dst = RosMat<Point>(dst.size());
            
            Mat temp_dst = Mat::zeros(dst.size(), dst.type());
            
            for (int itr = 0; itr < MAX_ITERATION && !compareMat(temp_dst, dst); itr ++) {
                
                cout<<itr<<": ";
                
                temp_dst = dst.clone();
                
                gradualResize(src_pyr[scale_itr], dst, reconst_src, reconst_dst, itr);
                
            }cout<<endl;
            
            
            
        }
        
        Mat write_dst = dst.clone();
        write_dst = write_dst*255;
        write_dst.convertTo(write_dst, CV_8UC3);
        
        stringstream path;
        path << "/Users/xup6qup3/Dropbox/code/Dev.temp/BidirectionalSimilarity/BidirectionalSimilarity/dst" << shrink_itr << ".jpg";
        
        shrink_itr ++;
        imwrite(path.str().c_str(), write_dst);
        path.clear();
        
        
    } while (dst.rows > src.rows*resize_ratio && dst.cols > src.cols*resize_ratio);
    
    
    src.release();

    return dst;
}


Mat BidirectSimilarity::reshuffling(const cv::Mat &src_img, const cv::Mat &shuffled_img) {
    
    
    Mat src = type_CV64FC3(src_img), shuffled = type_CV64FC3(shuffled_img);
    
    
    
    
    return shuffled_img;
}


double BidirectSimilarity::gradualResize (const cv::Mat &src, cv::Mat &dst, RosMat<Point>& reconst_src, RosMat<Point>& reconst_dst, int itr ) {

    
    double NumPatch_src = src.rows*src.cols, NumPatch_dst = dst.rows*dst.cols;
    
    bool fst_itr = !itr;
    
    #pragma omp parallel sections
    {
        #pragma omp section
        {
            PatchMatch patch_match = PatchMatch(PATCHWIDTH, PATCHWIDTH);
            patch_match.matchImageWithPrior(src, dst, reconst_dst, fst_itr);
            patch_match.~PatchMatch();
        }
        
        #pragma omp section
        {
            PatchMatch patch_match = PatchMatch(PATCHWIDTH, PATCHWIDTH);
            patch_match.matchImageWithPrior(dst, src, reconst_src, fst_itr);
            patch_match.~PatchMatch();
        }
    }
            
    Mat cohere_mat = Mat::zeros(dst.size(), dst.type());
    Mat cohere_count = Mat(dst.size(), CV_64FC1).setTo(0.000001);
    
    Mat complete_mat = Mat::zeros(dst.size(), dst.type());
    Mat complete_count = Mat(dst.size(), CV_64FC1).setTo(0.000001);
    
    double total_error = 0.0, cohere_error = 0.0, complete_error = 0.0;
    
    
    
    #pragma omp parallel sections
    {
        #pragma omp section
        {
            cohere_error = cohereTerm(src, dst, reconst_dst, cohere_mat, cohere_count)/NumPatch_dst;
        }
            
        #pragma omp section
        {
            complete_error = completeTerm(src, dst, reconst_src, complete_mat, complete_count)/NumPatch_src;
        }
    }
    
    total_error = cohere_error + complete_error;
    cout<<total_error<<", ";
    
//    Mat coh = Mat(dst.size(), CV_64FC3).setTo(0), com = Mat(dst.size(), CV_64FC3).setTo(0);;
//    
//    for (int i = 0; i < dst.rows; i ++) {
//        for (int j = 0; j < dst.cols; j ++) {
//            
//            coh.at<Vec3d>(i, j) = cohere_mat.at<Vec3d>(i, j);///cohere_count.at<double>(i, j);
//            com.at<Vec3d>(i, j) = complete_mat.at<Vec3d>(i, j);///complete_count.at<double>(i, j);
//            
//        }
//    }
//    
//    cout<<cohere_count<<endl;
//    cout<<complete_count;
//    
//    showMat(coh, "coh", 1);
//    showMat(com, "com", 0);

    
    for (int i = 0; i < cohere_mat.rows; i ++) {
        for (int j = 0; j < cohere_mat.cols; j ++) {
            
//            dst.at<Vec3d>(i, j) = (cohere_mat.at<Vec3d>(i, j)/NumPatch_dst + complete_mat.at<Vec3d>(i, j)/NumPatch_src)/( cohere_count.at<double>(i, j)/NumPatch_dst + complete_count.at<double>(i, j)/NumPatch_src );
            
            dst.at<Vec3d>(i, j) = (NumPatch_dst*complete_mat.at<Vec3d>(i, j) + NumPatch_src*cohere_mat.at<Vec3d>(i, j))/(NumPatch_dst*complete_count.at<double>(i, j) + NumPatch_src*cohere_count.at<double>(i, j));
            
        }
    }
    
//    showMat(src, "s", 1);
//    showMat(dst);
    
    
    cohere_mat.release();
    cohere_count.release();
    complete_mat.release();
    complete_count.release();
    
    return total_error;
    
}


double BidirectSimilarity::cohereTerm(const cv::Mat &src, const cv::Mat &dst, RosMat<Point> &reconst_dst, cv::Mat &cohere_mat, cv::Mat &cohere_count) {
    
    double cohere_error = 0.0;
    
    
    for (int i = 0; i < dst.rows; i ++) {
        for (int j = 0; j < dst.cols; j ++) {
            
            for (int m = -PATCHWIDTH/2; m <= PATCHWIDTH/2; m ++) {
                for (int n = -PATCHWIDTH/2; n <= PATCHWIDTH/2; n ++) {
                    
                    Point Q = Point(j + n, i + m);
                    
                    if ( boundaryTest(dst, Q) ) {
                        
                        Point P = Point(j, i) + reconst_dst.at(Q);
                        
                        if ( boundaryTest(src, P) ) {
                            
                            cohere_mat.at<Vec3d>(i, j) += src.at<Vec3d>(P);
                            cohere_count.at<double>(i, j) += 1;
                            
                            cohere_error += Vec3dDiff(dst.at<Vec3d>(i, j), src.at<Vec3d>(P));
                            
                        }
                        
                    }
                }
            }
            
            
        }
    }

    
    return cohere_error;
    
}


double BidirectSimilarity::completeTerm(const cv::Mat &src, const cv::Mat &dst, RosMat<Point> &reconst_src, cv::Mat &complete_mat, cv::Mat &complete_count) {
    
    double complete_error = 0.0;
    
    
    for (int i = 0; i < reconst_src.rows; i ++) {
        for (int j = 0; j < reconst_src.cols; j ++) {
            
            for (int m = -PATCHWIDTH/2; m <= PATCHWIDTH/2; m ++) {
                for (int n = -PATCHWIDTH/2; n <= PATCHWIDTH/2; n ++) {
                        
                    Point Q = Point(j, i) + reconst_src.at(i, j) + Point(n, m);
                    Point P = Point(j, i) + Point(n, m);
                    
                    if ( boundaryTest(dst, Q) && boundaryTest(src, P)) {
                        
                        complete_mat.at<Vec3d>(Q) += src.at<Vec3d>(P);
                        complete_count.at<double>(Q) += 1.0;
                        
                        complete_error += Vec3dDiff(dst.at<Vec3d>(Q), src.at<Vec3d>(P));
                    }
                        
                }
            }
            
        }
    }


    return complete_error;
    
}



RosMat<Point> BidirectSimilarity::cohereFindSimilarity(const cv::Mat &src, const cv::Mat &dst) {
    
    cout<<"Finding similarity  ";
    
    RosMat<Point> NearNei = RosMat<Point>(dst.rows, dst.cols);
    
    int cohereSearchWidth = 11;

    for (int i = 0; i < dst.rows; i ++) {
        
        if ( i % (dst.rows/10) == 0 ) cout<<"|";
        
        for (int j = 0; j < dst.cols; j ++) {
            
            Point min_offset(0);
            double min_cost = INFINITY;
            
            for (int m = -cohereSearchWidth; m <= cohereSearchWidth; m ++) {
                for (int n = -cohereSearchWidth; n <= cohereSearchWidth; n ++) {
                    
                    Point search_loc = Point(j + n, i + m);
                    
                    if ( boundaryTest(src, search_loc) ) {
                        
                        double local_cost = patchNorm(src, search_loc, dst, Point(j, i), PATCHWIDTH);
                        
                        if ( local_cost < min_cost ) {
                            
                            min_cost = local_cost;
                            min_offset = Point(n, m);
                        }
                    }
                    
                    search_loc.~Point_();
                    
                }
            }
            
            NearNei.at(i, j) = min_offset;
            min_offset.~Point_();
        }
    }
    
    cout<<"   finish.\n";
    
    return NearNei;
}


























