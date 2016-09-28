#include "stdafx.h"
#include <iostream>
#include <sstream>
#include "opencv2/opencv.hpp"
#include "Thinning.h"
#include "Dot.h"

#define SPACESIZE 1

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
	dot.makeSpace(SPACESIZE);
	dot.mergeLine(srcImg);

	cv::Mat dotImg = cv::Mat(srcImg.rows, srcImg.cols, CV_8UC3, cv::Scalar(0, 0, 0));

	for (int i = 0; i < dot.approximationLine.size(); i++){
		for (int j = 0; j < dot.approximationLine[i].size(); j++){
			int y = dot.approximationLine[i].at(j).first;
			int x = dot.approximationLine[i].at(j).second;
			if (j == 0) 
				circle(dotImg, cv::Point(x, y), .5, cv::Scalar(0, 0, 255), -1, 4);
			else if (j == dot.approximationLine[i].size() - 1)
				circle(dotImg, cv::Point(x, y), .5, cv::Scalar(255, 0, 0), -1, 4);
			else
				circle(dotImg, cv::Point(x, y), .5, cv::Scalar(255, 255, 255), -1, 4);

		}
	}	
	cv::imshow("dotImg", dotImg);
	cv::imwrite("dotMerge.png", dotImg);
}
void doJob(cv::Mat &srcImg, cv::Mat &resultImg){
	doThinning(srcImg, resultImg);
	doDot(resultImg);
}
int main()
{
	cv::Mat srcImg = cv::imread("example1.png");
	if (!srcImg.data)
		return -1;

	cv::Mat resultImg;

	doJob(srcImg, resultImg);
	cv::imshow("src", srcImg);
	cv::imshow("dst", resultImg);
	cv::waitKey();
	return 0;
}
