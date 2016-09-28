#include "stdafx.h"
#include <iostream>
#include <sstream>
#include "opencv2/opencv.hpp"
#include "Thinning.h"
#include "Dot.h"

#define SPACESIZE 20

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

	doJob(srcImg, resultImg);
	cv::imshow("src", srcImg);
	cv::imshow("dst", resultImg);
	cv::waitKey();
	return 0;
}
