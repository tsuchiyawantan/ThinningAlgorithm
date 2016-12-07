#include "stdafx.h"
#include <iostream>
#include <sstream>
#include "opencv2/opencv.hpp"
#include "Thinning.h"
#include "Dot.h"
#include "Voronoi.h"
#include "CatmullSpline.h"

#define SPACESIZE 10
#define HUE 20

using namespace std;

Voronoi vor;
cv::Subdiv2D subdiv;
CatmullSpline catmull;


void doThinning(cv::Mat &src_img, cv::Mat &result_img){
	Thinning thin;
	cv::cvtColor(src_img, result_img, CV_BGR2GRAY);
	cv::threshold(result_img, result_img, 50, 255, CV_THRESH_BINARY_INV);
	thin.thinning(result_img, result_img);
	//çïÇ©ÇÁîíÇ÷
	//cv::threshold(result_img, result_img, 0, 255, CV_THRESH_BINARY_INV);
}

void doCatmull(cv::Mat &resultImg, vector<vector<pair<int, int>>> &approximationLine){
	catmull.init();
	for (int i = 0; i < approximationLine.size(); i++){
		catmull.drawLine(resultImg, approximationLine[i], HUE);
	}
	catmull.drawInline(resultImg, HUE);
}
//pointsÇÕäp
void doVoronoi(cv::Mat &src_img, cv::Mat &result_img, vector<vector<cv::Point2f>> &points, vector<vector<pair<int, int>>> &contours){
	vor.mkVoronoiDelaunay(src_img, result_img, points, contours);
	vor.connectNearest(src_img, result_img, subdiv, points, contours);
}

void doDot(cv::Mat &src_img){
	Dot dot;
	dot.setWhiteDots(src_img);
	dot.findStart(src_img);
	dot.makeLine(src_img);
	dot.divideCon(SPACESIZE);
	dot.setCorner(src_img);

	cv::Mat dot_img = cv::Mat(src_img.rows, src_img.cols, CV_8UC3, cv::Scalar(0, 0, 0));
	cv::Mat dot_corner_img = cv::Mat(src_img.rows, src_img.cols, CV_8UC3, cv::Scalar(0, 0, 0));
	cv::Mat voronoi_corner_img = cv::Mat::zeros(src_img.cols, src_img.rows, CV_8UC3);
	cv::Mat result_img = cv::Mat::zeros(src_img.cols, src_img.rows, CV_8UC3);
	cv::Mat catmull_img = cv::Mat::zeros(src_img.cols, src_img.rows, CV_8UC3);
	cv::Mat voronoi_dot_img = cv::Mat::zeros(src_img.cols, src_img.rows, CV_8UC3);

	for (int i = 0; i < dot.contours.size(); i++){
		for (int j = 0; j < dot.contours[i].size()-1; j++){
			int y1 = dot.contours[i].at(j).first;
			int x1 = dot.contours[i].at(j).second;	
			int y2 = dot.contours[i].at(j+1).first;
			int x2 = dot.contours[i].at(j+1).second;
			if (j == 0){
				line(dot_img, cv::Point(x1, y1), cv::Point(x2, y2), cv::Scalar(0, 0, 255), .5);
			}
			else if (j == dot.contours[i].size() - 1)
				line(dot_img, cv::Point(x1, y1), cv::Point(x2, y2), cv::Scalar(255, 0, 0), .5);
			else
				line(dot_img, cv::Point(x1, y1), cv::Point(x2, y2), cv::Scalar(255, 255, 255), .5);
		}
	}	
	cv::imshow("line_img", dot_img);

	for (int i = 0; i < dot.divideContours.size(); i++){
		for (int j = 0; j < dot.divideContours[i].size(); j++){
			int y = dot.divideContours[i].at(j).y;
			int x = dot.divideContours[i].at(j).x;
			circle(dot_img, cv::Point(x, y), 2, cv::Scalar(0, 255, 0), -1, 4);

		}
	}
	for (int i = 0; i < dot.corners.size(); i++){
		for (int j = 0; j < dot.corners[i].size(); j++){
			int y = dot.corners[i].at(j).y;
			int x = dot.corners[i].at(j).x;
			circle(dot_img, cv::Point(x, y), 2, cv::Scalar(0, 0, 255), -1, 4);

		}
	}
	cv::imshow("dot_img", dot_img);

	doVoronoi(dot_img, result_img, dot.corners, dot.contours);
	//removeInRowDots();
	//doVoronoi(src_img, voronoi_dot_img, dot.contours);
	cv::imshow("dot_corner_img", dot_corner_img);
	cv::imshow("result_img", result_img);

	doCatmull(catmull_img, dot.contours);
	cv::imshow("catmull_img", catmull_img);

	/*cv::imshow("dot_img", dot_img);
	cv::imshow("dot_corner_img", dot_corner_img);
	*/
}

//îwåiçïÅAê¸Ç™îíÇÃèÍçáÅAsample.jpgÇ›ÇΩÇ¢Ç»èÍçá
void forMyJob(cv::Mat &src_img, cv::Mat &result_img){
	cv::cvtColor(src_img, result_img, CV_BGR2GRAY);
	cv::threshold(result_img, result_img, 50, 255, CV_THRESH_BINARY);
	//cv::imshow("forme", result_img);
	doDot(result_img);
}

void doJob(cv::Mat &src_img, cv::Mat &result_img){
	doThinning(src_img, result_img);
	doDot(result_img);
}

int main()
{
	cv::Mat src_img = cv::imread("sample.jpg");
	if (!src_img.data)
		return -1;

	cv::Mat result_img;
	forMyJob(src_img, result_img);
	//doJob(src_img, result_img);
	//cv::imshow("src", src_img);
//	cv::imshow("dst", result_img);
	cv::waitKey();
	return 0;
}
