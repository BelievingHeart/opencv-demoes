// calcHist_demo.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
/**
* @function calcHist_Demo.cpp
* @brief Demo code to use the function calcHist
* @author
*/

#include<opencv2/opencv.hpp>  
#include<iostream>  
#include<vector>  

using namespace cv;
using namespace std;

int main()
{
	Mat srcImage = imread("F://fruits.jpg`");
	imshow("【原图】", srcImage);

	Mat hsvImage;
	//因为要计算H-S的直方图，所以需要得到一个HSV空间的图像  
	cvtColor(srcImage, hsvImage, CV_BGR2HSV);
	imshow("【HSV空间的原图】", hsvImage);

	//为计算直方图配置变量  
	//首先是需要计算的图像的通道，就是需要计算图像的哪个通道（bgr空间需要确定计算 b或g货r空间）  
	int channels[] = { 0, 1 };
	//然后是配置输出的结果存储的 空间 ，用MatND类型来存储结果  
	MatND dstHist;
	//接下来是直方图的每一个维度的 柱条的数目（就是将数值分组，共有多少组）  
	//如果这里写成int histSize = 256;   那么下面调用计算直方图的函数的时候，该变量需要写 &histSize  
	int histSize[] = { 30, 32 };
	//最后是确定每个维度的取值范围，就是横坐标的总数  
	//首先得定义一个变量用来存储 单个维度的 数值的取值范围  
	float HRanges[] = { 0, 180 };
	float SRanges[] = { 0, 256 };
	const float *ranges[] = { HRanges, SRanges };

	calcHist(&hsvImage, 1, channels, Mat(), dstHist, 2, histSize, ranges, true, false);

	//calcHist  函数调用结束后，dstHist变量中将储存了 直方图的信息  用dstHist的模版函数 at<Type>(i)得到第i个柱条的值  
	//at<Type>(i, j)得到第i个并且第j个柱条的值  

	//开始直观的显示直方图——绘制直方图  
	//首先先创建一个黑底的图像，为了可以显示彩色，所以该绘制图像是一个8位的3通道图像  
	Mat drawImage = Mat::zeros(Size(300, 320), CV_8UC3);
	//因为任何一个图像的某个像素的总个数，都有可能会有很多，会超出所定义的图像的尺寸，针对这种情况，先对个数进行范围的限制  
	//先用 minMaxLoc函数来得到计算直方图后的像素的最大个数  
	double g_dHistMaxValue;
	minMaxLoc(dstHist, 0, &g_dHistMaxValue, 0, 0);
	//将像素的个数整合到 图像的最大范围内  
	//遍历直方图得到的数据  
	for (int i = 0; i < 30; i++)
	{
		for (int j = 0; j < 32; j++)
		{
			int value = cvRound(dstHist.at<float>(i, j) * 256 / g_dHistMaxValue);

			rectangle(drawImage, Point(10 * i, j * 10), Point((i + 1) * 10 - 1, (j + 1) * 10 - 1), Scalar(value), -1);
		}
	}

	imshow("【直方图】", drawImage);

	waitKey(0);

	return 0;
}
