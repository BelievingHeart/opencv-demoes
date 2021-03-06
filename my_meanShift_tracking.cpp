// my_meanShift_tracking.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include<iostream>
#include<opencv2\opencv.hpp>

using namespace std;
using namespace cv;

Mat frame, image, roiHist, backProject, imageRoi, hsv, hsvRoi, hsvImage;


int channels[] = { 0, 1 };
int histSize[] = { 30, 32 };
float HRanges[] = { 0, 180 };
float SRanges[] = { 0, 256 };
const float *ranges[] = { HRanges, SRanges };

bool leftButtonDown = false;
bool backProjectMode = false;
bool pause = false;
Rect roi;
Point origin;
bool config = false;
bool readyToCalcBackProject = false;
char key = 0;

void onMouse(int event, int x, int y, int flags,  void*)
{
	if (leftButtonDown == true)
	{   
		roi.x = min(x, origin.x);
		roi.y = min(y, origin.y);
		roi.width = abs(x - origin.x);
		roi.height = abs(y - origin.y);
		rectangle(image, roi, Scalar(0, 0, 255), 3);
		imshow("meanShiftTracker", image);
	}

	switch (event)
	{
	    case EVENT_LBUTTONDOWN:
        readyToCalcBackProject = false;
		origin.x = x;
		origin.y = y;
		roi = Rect(0, 0, x, y);
        leftButtonDown = true;
		backProjectMode = false;
			break;
		case EVENT_LBUTTONUP:
		leftButtonDown = false;
		if(roi.area()>0)
		{ 
		config = true;
		
		}
		break;
	}
}

void calcRoiHist(Rect &rect)

{
	if (config)
	{
		cvtColor(image, hsv, COLOR_BGR2HSV);
		Mat hsvRoi(hsv, roi);
		calcHist(&hsvRoi, 1, channels, Mat(), roiHist, 2, histSize, ranges, true, false);
		normalize(roiHist, roiHist, 0, 255, NORM_MINMAX);
		config = false;
		readyToCalcBackProject = true;
	}
}

int main()
{
	VideoCapture cap(0);
	if (!cap.isOpened())
	{
		cout << "Failed to open the camera." << endl;
		return 1;
	}

	namedWindow("meanShiftTracker", WINDOW_AUTOSIZE);
	setMouseCallback("meanShiftTracker", onMouse, 0);

	while (key!=27)
	{
		if(!pause)
		{
			float Start = getTickCount() / getTickFrequency();
			cap >> image;
			flip(image, image, 1);

			calcRoiHist(roi);
			if (readyToCalcBackProject)
			{
				cvtColor(image, hsvImage, COLOR_BGR2HSV);
				calcBackProject(&hsvImage, 1, channels, roiHist, backProject, ranges);
				TermCriteria criteria(TermCriteria::COUNT || TermCriteria::EPS, 10, 0.1);
				meanShift(backProject, roi, criteria);
				rectangle(image, roi, Scalar(0, 255, 0), 2);
			}
			if (backProjectMode)
			{
				normalize(backProject, backProject, 0, 255, NORM_MINMAX, CV_8U);
				imshow("meanShiftTracker", backProject);
			}
			else imshow("meanShiftTracker", image);
			
			float end = getTickCount() / getTickFrequency();
			cout << "Computational cost: " << end - Start << endl;
		}
		key = waitKey(10);
			switch (key)
			{
			case 'b':backProjectMode = !backProjectMode;
				break;
			case 'p':pause = !pause;
				break;
			}
	}
	return 0;
}