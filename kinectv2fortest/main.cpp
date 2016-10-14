#include "stdafx.h"
#include <iostream>
#include <sstream>
#include "opencv2/opencv.hpp"
#include "Thinning.h"
#include "Dot.h"

#define SPACESIZE 10

using namespace std;

/**
* This is an example on how to call the thinning funciton above
*/
void doThinning(cv::Mat &srcImg, cv::Mat &resultImg){
	Thinning thin;
	cv::cvtColor(srcImg, resultImg, CV_BGR2GRAY);
	cv::threshold(resultImg, resultImg, 10, 255, CV_THRESH_BINARY_INV);

	thin.thinning(resultImg, resultImg);
	//çïÇ©ÇÁîíÇ÷
	//cv::threshold(resultImg, resultImg, 0, 255, CV_THRESH_BINARY_INV);
}

void doDot(cv::Mat &srcImg){
	Dot dot;
	dot.setWhiteDots(srcImg);
	dot.findStart(srcImg);
	dot.makeLine(srcImg);
	dot.mergeLine(srcImg);
	dot.divideCon(SPACESIZE);

	cv::Mat dotImg = cv::Mat(srcImg.rows, srcImg.cols, CV_8UC3, cv::Scalar(0, 0, 0));
	for (int i = 0; i < dot.contours.size(); i++){
		for (int j = 0; j < dot.contours[i].size(); j++){
			int y = dot.contours[i].at(j).first;
			int x = dot.contours[i].at(j).second;
			if (j == 0)
				circle(dotImg, cv::Point(x, y), .5, cv::Scalar(0, 0, 255), -1, 4);
			else if (j == dot.contours[i].size() - 1)
				circle(dotImg, cv::Point(x, y), .5, cv::Scalar(255, 0, 0), -1, 4);
			else
				circle(dotImg, cv::Point(x, y), .5, cv::Scalar(255, 255, 255), -1, 4);

		}
	}	
	for (int i = 0; i < dot.divideContours.size(); i++){
		for (int j = 0; j < dot.divideContours[i].size(); j++){
			int y = dot.divideContours[i].at(j).first;
			int x = dot.divideContours[i].at(j).second;
			circle(dotImg, cv::Point(x, y), 2, cv::Scalar(0, 255, 0), -1, 4);

		}
	}
	cv::imshow("dotImg", dotImg);
	cv::imwrite("dot1005.png", dotImg);
}

void doFast(cv::Mat &srcImg){
	vector<cv::KeyPoint> fastKeyPoints;
	int th = 35;
	cv::FAST(srcImg, fastKeyPoints, th);
	// keypointÇï`âÊ
	for (auto k : fastKeyPoints){
		cv::circle(srcImg, k.pt, 3, cv::Scalar(255, 0, 0));
	}
	cv::imshow("fast", srcImg);
}
void doHarris(cv::Mat &srcImg){
	vector<cv::Point2f> corners;
	cv::goodFeaturesToTrack(srcImg, corners, 80, 0.01, 3, cv::Mat(), 3, true);
	vector<cv::Point2f>::iterator it_corner = corners.begin();
	it_corner = corners.begin();
	for (; it_corner != corners.end(); ++it_corner) {
		cv::circle(srcImg, cv::Point(it_corner->x, it_corner->y), 1, cv::Scalar(0, 255, 0), -1);
		cv::circle(srcImg, cv::Point(it_corner->x, it_corner->y), 8, cv::Scalar(0, 255, 0));
	}
	cv::imshow("harris ", srcImg);
}
void doAgast(cv::Mat &srcImg){
	vector<cv::KeyPoint> points;
	int th = 20;
	cv::AGAST(srcImg, points, th);
	// keypointÇï`âÊ
	for (auto k : points){
		cv::circle(srcImg, k.pt, 3, cv::Scalar(255, 0, 0));
	}
	cv::imshow("agast", srcImg);
}
void doNormal(cv::Mat &srcImg){
	cv::Mat grayImg;
	cv::cvtColor(srcImg, grayImg, CV_BGR2GRAY);
	//doFast(grayImg);
	//doHarris(grayImg);
	doAgast(grayImg);
}
void doJob(cv::Mat &srcImg, cv::Mat &resultImg){
	doThinning(srcImg, resultImg);
	doDot(resultImg);
}
int main()
{
	cv::Mat srcImg = cv::imread("cup.png");
	if (!srcImg.data)
		return -1;

	cv::Mat resultImg;
	//doNormal(srcImg);
	doJob(srcImg, resultImg);
	cv::imshow("src", srcImg);
	//cv::imshow("dst", resultImg);
	cv::waitKey();
	return 0;
}
