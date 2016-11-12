#include "stdafx.h"
#include <iostream>
#include <sstream>
#include "opencv2/opencv.hpp"
#include "Thinning.h"
#include "Dot.h"
#include <unordered_map>

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
	//黒から白へ
	//cv::threshold(result_img, result_img, 0, 255, CV_THRESH_BINARY_INV);
}
void makeEdgeListMap(vector<cv::Vec4f> &edge_list, unordered_map<string, vector<cv::Point>> &edge_map){
	for (auto itr = edge_list.begin(); itr != edge_list.end(); ++itr) {
		//スタートのxy
		string key = to_string((int)(*itr).val[0]) + ',' + to_string((int)(*itr).val[1]);
		edge_map[key].push_back(cv::Point((*itr).val[2], (*itr).val[3]));
		//上の逆
		key = to_string((int)(*itr).val[2]) + ',' + to_string((int)(*itr).val[3]);
		edge_map[key].push_back(cv::Point((*itr).val[0], (*itr).val[1]));
	}
}
void connectNearest(cv::Mat &src_img, cv::Mat &result_img, cv::Subdiv2D &subdiv, vector<vector<cv::Point2f>> &points){
	cv::Mat test_mat = src_img.clone();
	vector<cv::Vec4f> edge_list;
	unordered_map<string, vector<cv::Point>> edge_map;
	subdiv.getEdgeList(edge_list);
	makeEdgeListMap(edge_list, edge_map);
	
	for (int i = 0; i < points.size(); i++){

		int start_y = points[i].at(0).y;
		int start_x = points[i].at(0).x;

	}

	//circle(test_mat, subdiv.getVertex(vertexID, 0), 4, cv::Scalar(0, 100, 255), -1, 4);
	//cout << subdiv.getVertex(vertexID, 0) << endl;
	//circle(test_mat, cv::Point2f(points[0].at(0).x, points[0].at(0).y), 2, cv::Scalar(255, 100, 0), -1, 4);
	cv::imshow("test_mat", test_mat);
}

void doVoronoi(cv::Mat &src_img, cv::Mat &result_img, vector<vector<cv::Point2f>> &points){
	cv::Subdiv2D subdiv;
	subdiv.initDelaunay(cv::Rect(0, 0, 600, 600));
	for (int i = 0; i < points.size(); i++){
			subdiv.insert(points[i]);
	}
	vector<int> idx;
	vector<std::vector<cv::Point2f>> facetLists;
	vector<cv::Point2f> facetCenters;
	subdiv.getVoronoiFacetList(idx, facetLists, facetCenters);

	for (int i = 0; i < points.size(); i++){
		for (int j = 0; j < points[i].size(); j++){
			int y = points[i].at(j).y;
			int x = points[i].at(j).x;
			circle(result_img, cv::Point(x, y), 2, cv::Scalar(0, 0, 255), -1, 4);
		}
	}


	// Voronoi図を描画
	for (auto list = facetLists.begin(); list != facetLists.end(); list++)
	{
		cv::Point2f before = list->back();
		for (auto pt = list->begin(); pt != list->end(); pt++)
		{
			cv::Point p1((int)before.x, (int)before.y);
			cv::Point p2((int)pt->x, (int)pt->y);
			cv::line(result_img, p1, p2, cv::Scalar(64, 255, 128));
			before = *pt;
		}
	}

	// 辺のリストを取得
	vector<cv::Vec4f> edgeList;
	cv::Mat imgEdges;
	subdiv.getEdgeList(edgeList);


	// 描画
	for (auto edge = edgeList.begin(); edge != edgeList.end(); edge++)
	{
		cv::Point p1(edge->val[0], edge->val[1]);
		cv::Point p2(edge->val[2], edge->val[3]);
		cv::line(imgEdges, p1, p2, cv::Scalar(48, 128, 48));
	}

	// ドロネー三角形のリストを取得
	std::vector<cv::Vec6f> triangles;
	subdiv.getTriangleList(triangles);

	// 描画
	for (auto it = triangles.begin(); it != triangles.end(); it++)
	{
		cv::Vec6f &vec = *it;
		cv::Point p1(vec[0], vec[1]);
		cv::Point p2(vec[2], vec[3]);
		cv::Point p3(vec[4], vec[5]);
		cv::line(result_img, p1, p2, cv::Scalar(255, 0, 0));
		cv::line(result_img, p2, p3, cv::Scalar(255, 0, 0));
		cv::line(result_img, p3, p1, cv::Scalar(255, 0, 0));
	}

	connectNearest(src_img, result_img, subdiv, points);
	
	//connectNearest();
}

void doDot(cv::Mat &src_img){
	Dot dot;
	dot.setWhiteDots(src_img);
	dot.findStart(src_img);
	dot.makeLine(src_img);
	//dot.mergeLineAll(src_img);
	dot.divideCon(SPACESIZE);
	dot.setCorner(src_img);
	cv::imshow("forward", src_img);

	cv::Mat dot_img = cv::Mat(src_img.rows, src_img.cols, CV_8UC3, cv::Scalar(0, 0, 0));
	cv::Mat dot_corner_img = cv::Mat(src_img.rows, src_img.cols, CV_8UC3, cv::Scalar(0, 0, 0));
	cv::Mat voronoi_corner_img = cv::Mat::zeros(src_img.cols, src_img.rows, CV_8UC3);
	cv::Mat voronoi_dot_img = cv::Mat::zeros(src_img.cols, src_img.rows, CV_8UC3);

	for (int i = 0; i < dot.contours.size(); i++){
		for (int j = 0; j < dot.contours[i].size()-1; j++){
			int y1 = dot.contours[i].at(j).first;
			int x1 = dot.contours[i].at(j).second;	
			int y2 = dot.contours[i].at(j+1).first;
			int x2 = dot.contours[i].at(j+1).second;
			if (j == 0){
				line(dot_img, cv::Point(x1, y1), cv::Point(x2, y2), cv::Scalar(0, 0, 255), .5);
				cv::imshow("dot_img"+to_string(i), dot_img);
			}
			else if (j == dot.contours[i].size() - 1)
				line(dot_img, cv::Point(x1, y1), cv::Point(x2, y2), cv::Scalar(255, 0, 0), .5);
			else
				line(dot_img, cv::Point(x1, y1), cv::Point(x2, y2), cv::Scalar(255, 255, 255), .5);
		}
	}	
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
	doVoronoi(dot_img, voronoi_corner_img, dot.corners);
	//doVoronoi(src_img, voronoi_dot_img, dot.contours);
	cv::imshow("dot_img", dot_img);
	cv::imshow("dot_corner_img", dot_corner_img);
	cv::imshow("voronoi_corner_img", voronoi_corner_img);
	/*cv::imshow("dot_img", dot_img);
	cv::imshow("dot_corner_img", dot_corner_img);
	*/
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
