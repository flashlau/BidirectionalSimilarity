/**
* 2009 DIP Final Project Code
* 
* The code is part of Fantasy Illustrator project.
* Author: 翁郁婷(Yu-Ting Wong), 張明旭(Ming-Hsu Chang), 羅聖傑(Sheng-Jie Luo)
* Email: forestking@cmlab.csie.ntu.edu.tw
* 
* Any usage of the code is not allowed without the authorization by the authors.
*/

#include "Color_Transfer.h"

IplImage* color_transfer(IplImage* img_src, IplImage* img_tar)
{

	CvSize size_src = cvGetSize(img_src);
	CvSize size_tar = cvGetSize(img_tar);

	// For debuging
	/*cv::namedWindow("Source image", CV_WINDOW_AUTOSIZE); //創建視窗
	cv::imshow("Source image", img_src); //顯示圖像

	cv::namedWindow("Target image", CV_WINDOW_AUTOSIZE);
	cv::imshow("Target image", img_tar);*/
 
	// Convert RGB to CIELab color space
	IplImage* lab_src = cvCreateImage(size_src, IPL_DEPTH_32F, 3);
	cvCvtColor(img_src, lab_src, CV_BGR2Lab);

	IplImage* lab_tar = cvCreateImage(size_tar, IPL_DEPTH_32F, 3);
	cvCvtColor(img_tar, lab_tar, CV_BGR2Lab);

	// Calculate the mean & std of each channel
	CvScalar mean_sor, std_sor, mean_tar, std_tar;
	cvAvgSdv(lab_src, &mean_sor, &std_sor);
	cvAvgSdv(lab_tar, &mean_tar, &std_tar);

	// For debuging
	/*cout << "source mean : " << mean_sor.val[0] << "\t" << mean_sor.val[1] << "\t" << mean_sor.val[2] << endl;
	cout << "source std : " << std_sor.val[0] << "\t" << std_sor.val[1] << "\t" << std_sor.val[2] << endl;
	cout << "target mean : " << mean_tar.val[0] << "\t" << mean_tar.val[1] << "\t" << mean_tar.val[2] << endl;
	cout << "target std : " << std_tar.val[0] << "\t" << std_tar.val[1] << "\t" << std_tar.val[2] << endl;*/
	
	CvScalar scalar1;
	// For debuging
	/*scalar1 = cvGet2D(lab_src, 0, 0);
	cout << scalar1.val[0] << "\t" << scalar1.val[1] << "\t" << scalar1.val[2]<< endl;
	scalar1 = cvGet2D(lab_src, 1, 0);
	cout << scalar1.val[0] << "\t" << scalar1.val[1] << "\t" << scalar1.val[2]<< endl;*/

	cvSubS(lab_src, mean_sor, lab_src, NULL);
	// For debuging
	/*scalar1 = cvGet2D(lab_src, 0, 0);
	cout << scalar1.val[0] << "\t" << scalar1.val[1] << "\t" << scalar1.val[2]<< endl;
	scalar1 = cvGet2D(lab_src, 1, 0);
	cout << scalar1.val[0] << "\t" << scalar1.val[1] << "\t" << scalar1.val[2]<< endl;*/

	
	double ratio[3];
	for(int i = 0; i < 3; i++)
		ratio[i] = std_tar.val[i]/std_sor.val[i];

	
	for(int i = 0; i < lab_src->height; i++)
	{
		for(int j = 0; j < lab_src->width; j++)
		{
			scalar1 = mycvGet2D_float3(lab_src, i, j);
			for(int t = 0; t < 3; t++)
				scalar1.val[t] *= ratio[t];
			mycvSet2D_float3(lab_src, i, j, scalar1);
		}
	}


	// For debuging
	/*scalar1 = cvGet2D(lab_src, 0, 0);
	cout << scalar1.val[0] << "\t" << scalar1.val[1] << "\t" << scalar1.val[2]<< endl;
	scalar1 = cvGet2D(lab_src, 1, 0);
	cout << scalar1.val[0] << "\t" << scalar1.val[1] << "\t" << scalar1.val[2]<< endl;*/
	
	cvAddS(lab_src, mean_tar, lab_src, NULL);

	// For debuging
	/*double lab_max[3] = {100.0, 127.0, 127.0};
	double lab_min[3] = {0.0, -127.0, -127.0};
	for(int i = 0; i < lab_src->height; i++)
	{
		for(int j = 0; j < lab_src->width; j++)
		{
			scalar1 = cvGet2D(lab_src, i, j);
			for(int t = 0; t < 3; t++)
			{
				double tmp = scalar1.val[t];
				scalar1.val[t] = min(max(scalar1.val[t], lab_min[t]), lab_max[t]);
				if(tmp != scalar1.val[t])
					cout << i << "\t" << j << "\t" << t << "\t" << tmp << endl;
			}
			cvSet2D(lab_src, i, j, scalar1);
		}
	}

	scalar1 = cvGet2D(lab_src, 0, 0);
	cout << scalar1.val[0] << "\t" << scalar1.val[1] << "\t" << scalar1.val[2]<< endl;
	scalar1 = cvGet2D(lab_src, 1, 0);
	cout << scalar1.val[0] << "\t" << scalar1.val[1] << "\t" << scalar1.val[2]<< endl;*/
	
	
	// Result 1 : without normalizarion
	IplImage* img_res = cvCreateImage(size_src, IPL_DEPTH_32F, 3);
	cvCvtColor(lab_src, img_res, CV_Lab2BGR);

	//// Result 2 : with CV_C normalization
	//IplImage* img_res2 = cvCreateImage(size_src, IPL_DEPTH_32F, 3);
	//cvNormalize(img_res1, img_res2, 1.0, 0.0, CV_C, NULL);

	//// Result 3 : with CV_MAXMIN normalization
	//IplImage* lab_src2 = cvCreateImage(size_src, IPL_DEPTH_32F, 3);
	//IplImage* src_l = cvCreateImage(size_src, IPL_DEPTH_32F, 1);
	//IplImage* src_a = cvCreateImage(size_src, IPL_DEPTH_32F, 1);
	//IplImage* src_b = cvCreateImage(size_src, IPL_DEPTH_32F, 1);
	//cvSplit(lab_src, src_l, src_a, src_b, 0);
	//cvNormalize(src_l, src_l, 100.0, 0.0, CV_MINMAX, NULL);
	//cvNormalize(src_l, src_l, -127.0, 127.0, CV_MINMAX, NULL);
	//cvNormalize(src_l, src_l, -127.0, 127.0, CV_MINMAX, NULL);
	//cvMerge(src_l, src_a, src_b, 0, lab_src2);
	//IplImage* img_res3 = cvCreateImage(size_src, IPL_DEPTH_32F, 3);
	//cvCvtColor(lab_src2, img_res3, CV_Lab2BGR);

	//// Result 4 : with CV_C normalization
	//IplImage* img_res4 = cvCreateImage(size_src, IPL_DEPTH_32F, 3);
	//cvCvtColor(lab_src, img_res4, CV_Lab2BGR);
	//IplImage* src_r = cvCreateImage(size_src, IPL_DEPTH_32F, 1);
	//IplImage* src_g = cvCreateImage(size_src, IPL_DEPTH_32F, 1);
	////IplImage* src_b = cvCreateImage(size_src, IPL_DEPTH_32F, 1);
	//cvSplit(img_res4, src_b, src_g, src_r, 0);
	//cvNormalize(src_r, src_r, 1.0, 0.0, CV_MINMAX, NULL);
	//cvNormalize(src_g, src_g, 1.0, 0.0, CV_MINMAX, NULL);
	//cvNormalize(src_b, src_b, 1.0, 0.0, CV_MINMAX, NULL);
	//cvMerge(src_b, src_g, src_r, 0, img_res4);

	//namedWindow("Result image1", CV_WINDOW_AUTOSIZE); //創建視窗
	//imshow("Result image1", img_res1); //顯示圖像
	//namedWindow("Result image2", CV_WINDOW_AUTOSIZE); //創建視窗
	//imshow("Result image2", img_res2); //顯示圖像
	//namedWindow("Result image3", CV_WINDOW_AUTOSIZE); //創建視窗
	//imshow("Result image3", img_res3); //顯示圖像
	//namedWindow("Result image4", CV_WINDOW_AUTOSIZE); //創建視窗
	//imshow("Result image4", img_res4); //顯示圖像
	

	// For debuging
	/*cout << "transfer1" << endl;
	for(int i = 0; i < lab_src->height; i++)
	{
		for(int j = 0; j < lab_src->width; j++)
		{
			scalar1 = cvGet2D(img_res, i, j);
			for(int t = 0; t < 3; t++)
			{
				if(scalar1.val[t] > 1.0 || scalar1.val[t] < 0.0)
					cout << i << "\t" << j << "\t" << t << "\t" << scalar1.val[t] << endl;
			}
		}
	}

	namedWindow("Result image", CV_WINDOW_AUTOSIZE); //創建視窗
	imshow("Result image", img_res); //顯示圖像

	system("pause");
	cv::waitKey(0);*/

	cvReleaseImage(&lab_src);
	cvReleaseImage(&lab_tar);

	return img_res;
}


IplImage* color_transfer2(IplImage* img_src, IplImage* img_tar)
{

	CvSize size_src = cvGetSize(img_src);
	CvSize size_tar = cvGetSize(img_tar);

	// For debuging
	/*cv::namedWindow("Source image", CV_WINDOW_AUTOSIZE); //創建視窗
	cv::imshow("Source image", img_src); //顯示圖像

	cv::namedWindow("Target image", CV_WINDOW_AUTOSIZE);
	cv::imshow("Target image", img_tar);*/
 
	// Convert RGB to CIELab color space
	IplImage* lab_src = myBGR2Lab(img_src);

	IplImage* lab_tar = myBGR2Lab(img_tar);

	// Calculate the mean & std of each channel
	CvScalar mean_sor, std_sor, mean_tar, std_tar;
	cvAvgSdv(lab_src, &mean_sor, &std_sor);
	cvAvgSdv(lab_tar, &mean_tar, &std_tar);

	// For debuging
	/*cout << "source mean : " << mean_sor.val[0] << "\t" << mean_sor.val[1] << "\t" << mean_sor.val[2] << endl;
	cout << "source std : " << std_sor.val[0] << "\t" << std_sor.val[1] << "\t" << std_sor.val[2] << endl;
	cout << "target mean : " << mean_tar.val[0] << "\t" << mean_tar.val[1] << "\t" << mean_tar.val[2] << endl;
	cout << "target std : " << std_tar.val[0] << "\t" << std_tar.val[1] << "\t" << std_tar.val[2] << endl;
	
	scalar1 = cvGet2D(lab_src, 0, 0);
	cout << scalar1.val[0] << "\t" << scalar1.val[1] << "\t" << scalar1.val[2]<< endl;
	scalar1 = cvGet2D(lab_src, 1, 0);
	cout << scalar1.val[0] << "\t" << scalar1.val[1] << "\t" << scalar1.val[2]<< endl;*/
	
	CvScalar scalar1;
	cvSubS(lab_src, mean_sor, lab_src, NULL);

	double ratio[3];
	for(int i = 0; i < 3; i++)
		ratio[i] = std_tar.val[i]/std_sor.val[i];

	
	for(int i = 0; i < lab_src->height; i++)
	{
		for(int j = 0; j < lab_src->width; j++)
		{
			scalar1 = mycvGet2D_float3(lab_src, i, j);
			for(int t = 0; t < 3; t++)
				scalar1.val[t] *= ratio[t];
			mycvSet2D_float3(lab_src, i, j, scalar1);
		}
	}
	
	cvAddS(lab_src, mean_tar, lab_src, NULL);
	
	IplImage* img_res = myLab2BGR(lab_src);
	//cvNormalize(img_res, img_res, 1.0, 0.0, CV_C, NULL);

	// For debuging
	/*cout << "transfer2" << endl;
	for(int i = 0; i < lab_src->height; i++)
	{
		for(int j = 0; j < lab_src->width; j++)
		{
			scalar1 = cvGet2D(img_res, i, j);
			for(int t = 0; t < 3; t++)
			{
				if(scalar1.val[t] > 1.0 || scalar1.val[t] < 0.0)
					cout << i << "\t" << j << "\t" << t << "\t" << scalar1.val[t] << endl;
			}
		}
	}

	cv::namedWindow("Result image2", CV_WINDOW_AUTOSIZE); //創建視窗
	cv::imshow("Result image2", img_res); //顯示圖像

	system("pause");
	cv::waitKey(0);*/

	cvReleaseImage(&lab_src);
	cvReleaseImage(&lab_tar);

	return img_res;
}
 
//IplImage* myBGR2Lab(IplImage* img_src, IplImage* img_res)
IplImage* myBGR2Lab(IplImage* img_src)
{
	CvSize size_src = cvGetSize(img_src);
	IplImage* img_res = cvCreateImage(size_src, IPL_DEPTH_32F, 3);
 
	double m1[3][3] = {{0.3811, 0.5783, 0.0402}, {0.1967, 0.7244, 0.0782}, {0.0241, 0.1288, 0.8444}};
	double m2[3][3] = {{0.5774, 0.5774, 0.5774}, {0.4082, 0.4082, -0.8165}, {0.7071, -0.7071, 0.0}};

	CvScalar scalar1, scalar2, scalar3;
	for(int i = 0; i < img_src->height; i++)
	{
		for(int j = 0; j < img_src->width; j++)
		{
			scalar1 = mycvGet2D_float3(img_src, i, j);
			scalar2 = cvScalar(0.0, 0.0, 0.0);
			scalar3 = cvScalar(0.0, 0.0, 0.0);

			for(int t1 = 0; t1 < 3; t1++)
			{
				for(int t2 = 0; t2 < 3; t2++)
					scalar2.val[t1] += m1[t1][t2] * scalar1.val[2-t2];

				scalar2.val[t1] = log10(scalar2.val[t1]+0.000000001);
			}

			for(int t1 = 0; t1 < 3; t1++)
			{
				for(int t2 = 0; t2 < 3; t2++)
					scalar3.val[t1] += m2[t1][t2] * scalar2.val[t2];
			}

			mycvSet2D_float3(img_res, i, j, scalar3);
		}
	}

	// For debuging
	/*scalar1 = cvGet2D(img_src, 0, 0);
	cout << scalar1.val[0] << "\t" << scalar1.val[1] << "\t" << scalar1.val[2]<< endl;

	scalar1 = cvGet2D(img_res, 0, 0);
	cout << scalar1.val[0] << "\t" << scalar1.val[1] << "\t" << scalar1.val[2]<< endl;
	system("pause");*/

	return img_res;
}

IplImage* myLab2BGR(IplImage* img_src)
{
	CvSize size_src = cvGetSize(img_src);
	IplImage* img_res = cvCreateImage(size_src, IPL_DEPTH_32F, 3);
 
	double m1[3][3] = {{0.5774, 0.4082, 0.7071}, {0.5774, 0.4082, -0.7071}, {0.5774, -0.8165, 0.0}};
	double m2[3][3] = {{4.4679, -3.5873, 0.1193}, {-1.2186, 2.3809, -0.1624}, {0.0497, -0.2439, 1.2045}};

	CvScalar scalar1, scalar2, scalar3;
	for(int i = 0; i < img_src->height; i++)
	{
		for(int j = 0; j < img_src->width; j++)
		{
			scalar1 = mycvGet2D_float3(img_src, i, j);
			scalar2 = cvScalar(0.0, 0.0, 0.0);
			scalar3 = cvScalar(0.0, 0.0, 0.0);

			for(int t1 = 0; t1 < 3; t1++)
			{
				for(int t2 = 0; t2 < 3; t2++)
					scalar2.val[t1] += m1[t1][t2] * scalar1.val[t2];

				scalar2.val[t1] = pow(10, scalar2.val[t1]);
			}


			for(int t1 = 0; t1 < 3; t1++)
			{
				for(int t2 = 0; t2 < 3; t2++)
					scalar3.val[2-t1] += m2[t1][t2] * scalar2.val[t2];
			}

			mycvSet2D_float3(img_res, i, j, scalar3);
		}
	}

	// For debuging
	/*scalar1 = cvGet2D(img_src, 0, 0);
	cout << scalar1.val[0] << "\t" << scalar1.val[1] << "\t" << scalar1.val[2]<< endl;

	scalar1 = cvGet2D(img_res, 0, 0);
	cout << scalar1.val[0] << "\t" << scalar1.val[1] << "\t" << scalar1.val[2]<< endl;
	system("pause");*/

	return img_res;
}