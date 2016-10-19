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
void doThinning(cv::Mat &src_img, cv::Mat &result_img){
	Thinning thin;
	cv::cvtColor(src_img, result_img, CV_BGR2GRAY);
	cv::threshold(result_img, result_img, 50, 255, CV_THRESH_BINARY_INV);


	thin.thinning(result_img, result_img);
	//çïÇ©ÇÁîíÇ÷
	//cv::threshold(result_img, result_img, 0, 255, CV_THRESH_BINARY_INV);
}

void doDot(cv::Mat &src_img){
	Dot dot;
	dot.setWhiteDots(src_img);
	dot.findStart(src_img);
	dot.makeLine(src_img);
	dot.mergeLine(src_img);
	dot.divideCon(SPACESIZE);
	dot.setCorner(src_img);

	cv::Mat dot_img = cv::Mat(src_img.rows, src_img.cols, CV_8UC3, cv::Scalar(0, 0, 0));
	for (int i = 0; i < dot.contours.size(); i++){
		for (int j = 0; j < dot.contours[i].size(); j++){
			int y = dot.contours[i].at(j).first;
			int x = dot.contours[i].at(j).second;
			if (j == 0)
				circle(dot_img, cv::Point(x, y), .5, cv::Scalar(0, 0, 255), -1, 4);
			else if (j == dot.contours[i].size() - 1)
				circle(dot_img, cv::Point(x, y), .5, cv::Scalar(255, 0, 0), -1, 4);
			else
				circle(dot_img, cv::Point(x, y), .5, cv::Scalar(255, 255, 255), -1, 4);

		}
	}	
	for (int i = 0; i < dot.divideContours.size(); i++){
		for (int j = 0; j < dot.divideContours[i].size(); j++){
			int y = dot.divideContours[i].at(j).first;
			int x = dot.divideContours[i].at(j).second;
			circle(dot_img, cv::Point(x, y), 2, cv::Scalar(0, 255, 0), -1, 4);

		}
	}
	for (int i = 0; i < dot.corners.size(); i++){
		for (int j = 0; j < dot.corners[i].size(); j++){
			int y = dot.corners[i].at(j).first;
			int x = dot.corners[i].at(j).second;
			circle(dot_img, cv::Point(x, y), 2, cv::Scalar(0, 0, 255), -1, 4);

		}
	}
	cv::imshow("dot_img", dot_img);
	cv::imwrite("dot1005.png", dot_img);
}

void doNormal(cv::Mat &src_img){
	cv::Mat grayImg;
	cv::cvtColor(src_img, grayImg, CV_BGR2GRAY);
}
void doJob(cv::Mat &src_img, cv::Mat &result_img){
	doThinning(src_img, result_img);
	doDot(result_img);
}
int main()
{
	cv::Mat src_img = cv::imread("cup.png");
	if (!src_img.data)
		return -1;

	cv::Mat result_img;
	//doNormal(src_img);
	doJob(src_img, result_img);
	cv::imshow("src", src_img);
	//cv::imshow("dst", result_img);
	cv::waitKey();
	return 0;
}
