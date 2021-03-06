// my_Kalman.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "opencv2/video/tracking.hpp"
#include "opencv2/highgui.hpp"
#include<iostream>

#include <stdio.h>

using namespace cv;

static inline Point calcPoint(Point2f center, double R, double angle)
{
	return center + Point2f((float)cos(angle), (float)-sin(angle))*(float)R;
}

static void help()
{
	printf("\nExample of c calls to OpenCV's Kalman filter.\n"
		"   Tracking of rotating point.\n"
		"   Rotation speed is constant.\n"
		"   Both state and measurements vectors are 1D (a point angle),\n"
		"   Measurement is the real point angle + gaussian noise.\n"
		"   The real and the estimated points are connected with yellow line segment,\n"
		"   the real and the measured points are connected with red line segment.\n"
		"   (if Kalman filter works correctly,\n"
		"    the yellow segment should be shorter than the red one).\n"
		"\n"
		"   Pressing any key (except ESC) will reset the tracking with a different speed.\n"
		"   Pressing ESC will stop the program.\n"
	);
}

int main(int, char**)
{
	help();
	Mat img(500, 500, CV_8UC3);
	KalmanFilter KF(2, 1, 0);
	Mat state(2, 1, CV_32F); /* (phi, delta_phi) */
	Mat processNoise(2, 1, CV_32F);
	Mat measurement = Mat::zeros(1, 1, CV_32F);
	char code = (char)-1;

	for (;;)
	{
		//randn(KF.statePre, Scalar::all(0), Scalar::all(0.1));
		KF.transitionMatrix = (Mat_<float>(2, 2) << 1, 1, 0, 1);

		Mat mimicState = KF.statePre.clone();

		setIdentity(KF.measurementMatrix);
		setIdentity(KF.processNoiseCov, Scalar::all(1e-5));
		setIdentity(KF.measurementNoiseCov, Scalar::all(1e-1));
		setIdentity(KF.errorCovPost, Scalar::all(0.1));

		randn(KF.statePost, Scalar::all(0), Scalar::all(1));

		for (;;)
		{
			Point2f center(img.cols*0.5f, img.rows*0.5f);
			float R = img.cols / 3.f;
			

			Mat prediction = KF.predict();
			double predictAngle = prediction.at<float>(0);
			Point predictPt = calcPoint(center, R, predictAngle);

			mimicState = KF.transitionMatrix*mimicState;
			double mimicAngle = mimicState.at<float>(0);
			Point mimicPt = calcPoint(center, R, mimicAngle);

			std::cout << predictAngle << "\t"<<mimicAngle<<std::endl;

			randn(measurement, Scalar::all(0), Scalar::all(KF.measurementNoiseCov.at<float>(0)));

			if (theRNG().uniform(0, 4) != 0)
				KF.correct(measurement);

            Mat Prior = KF.statePost;
			double priorAngle = Prior.at<float>(0);
			Point priorPt = calcPoint(center, R, priorAngle);


			// generate "real" measurement
			measurement += KF.measurementMatrix*prediction;

			double measAngle = measurement.at<float>(0);
			Point measPt = calcPoint(center, R, measAngle);

			// plot points
#define drawCross( center, color, d )                                        \
                line( img, Point( center.x - d, center.y - d ),                          \
                             Point( center.x + d, center.y + d ), color, 1, LINE_AA, 0); \
                line( img, Point( center.x + d, center.y - d ),                          \
                             Point( center.x - d, center.y + d ), color, 1, LINE_AA, 0 )

			img = Scalar::all(0);
			drawCross(priorPt, Scalar(255, 255, 255), 3);
			drawCross(measPt, Scalar(0, 0, 255), 3);
			drawCross(predictPt, Scalar(0, 255, 0), 3);
			line(img, predictPt, measPt, Scalar(0, 0, 255), 3, LINE_AA, 0);
			line(img, predictPt, priorPt, Scalar(0, 255, 255), 3, LINE_AA, 0);

			


			//std::cout << KF.processNoiseCov.at<float>(0, 0) << "  " << KF.measurementNoiseCov.at<float>(0,0) << "  " << KF.errorCovPost.at<float>(0, 0) << std::endl;

			

			imshow("Kalman", img);
			code = (char)waitKey(100);

			if (code > 0)
				break;
		}
		if (code == 27 || code == 'q' || code == 'Q')
			break;
	}

	return 0;
}

