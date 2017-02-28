#include "stdafx.h"
#include <iostream>
#include <sstream>
#include "opencv2/opencv.hpp"
#include "Thinning.h"
#include "Dot.h"
#include "Voronoi.h"
#include "CatmullSpline.h"
#include "Node.h"

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
	//黒から白へ
	//cv::threshold(result_img, result_img, 0, 255, CV_THRESH_BINARY_INV);
}

void doCatmull(cv::Mat &resultImg, vector<vector<cv::Point>> &approximationLine){
	catmull.init();
	for (int i = 0; i < approximationLine.size(); i++){
		catmull.drawLine(resultImg, approximationLine[i], HUE);
	}
	catmull.drawInline(resultImg, HUE);
}
//pointsは角
void doVoronoi(cv::Mat &src_img, cv::Mat &result_img, vector<vector<cv::Point2f>> &points, vector<vector<pair<int, int>>> &contours){
	vor.mkVoronoiDelaunay(src_img, result_img, points, contours);
}

void doNodeEdge(vector<vector<cv::Point>> divcon, vector<vector<Node *>> &node_array){
	//ノードの用意
	for (int i = 0; i < divcon.size(); i++){
		vector<Node *> node_array_child;
		cv::Point node;

		//エッジがない場合＝ノードが右隣にいない
		if (divcon[i].size() == 1) {
			node = divcon[i].at(0);
			node_array_child.push_back(new Node(node, 0));
			continue;
		}

		//ノードの生成
		//エッジを一個持ってる状態
		for (int j = 0; j < divcon[i].size(); j++){
			node = divcon[i].at(j);
			node_array_child.push_back(new Node(node, 1));
		}
		//終点はノードが右隣にいない
		node = divcon[i].at(divcon[i].size()-1);
		node_array_child.push_back(new Node(node, 0));

		//ノードの連結操作
		Node *this_node;
		Node *prev_node;
		Node *next_node;

		for (int l = 0; l < node_array_child.size(); l++){
			if (l == 0){ //始点
				this_node = node_array_child.at(l);
				next_node = node_array_child.at(l + 1);
				(*this_node).addEdge(next_node);
			}
			else if (l == node_array_child.size() - 1){ //終点
				this_node = node_array_child.at(l);
				prev_node = node_array_child.at(l - 1);
				int edgearray_num = (*prev_node).hasEdge(this_node);
				if (edgearray_num >= 0){
					Edge *edge = (*prev_node).getEdge(edgearray_num);
					(*this_node).setEdge(edge);
				}
			}
			else {
				this_node = node_array_child.at(l);
				prev_node = node_array_child.at(l - 1);
				next_node = node_array_child.at(l + 1);
				(*this_node).addEdge(next_node);
				int edgearray_num = (*prev_node).hasEdge(this_node);
				if (edgearray_num >= 0){
					Edge *edge = (*prev_node).getEdge(edgearray_num);
					(*this_node).setEdge(edge);
				}
			}
		}
		node_array.push_back(node_array_child);
	}
}

void doDot(cv::Mat &src_img){
	Dot dot;
	//Node vectorを作成する。動的のが今後便利
	vector<vector<Node *>> node_array;
	dot.setWhiteDots(src_img);
	dot.findStart(src_img);
	dot.makeLine(src_img);
	dot.divideCon(SPACESIZE);
	dot.setCornerResult(src_img);

	cv::Mat dot_img = cv::Mat(src_img.rows, src_img.cols, CV_8UC3, cv::Scalar(0, 0, 0));
	cv::Mat dot_divcon_img = cv::Mat(src_img.rows, src_img.cols, CV_8UC3, cv::Scalar(0, 0, 0));
	cv::Mat dot_corner_img = cv::Mat(src_img.rows, src_img.cols, CV_8UC3, cv::Scalar(0, 0, 0));
	cv::Mat voronoi_corner_img = cv::Mat::zeros(src_img.cols, src_img.rows, CV_8UC3);
	cv::Mat result_img = cv::Mat::zeros(src_img.cols, src_img.rows, CV_8UC3);
	cv::Mat catmull_img = cv::Mat::zeros(src_img.cols, src_img.rows, CV_8UC3);
	cv::Mat voronoi_dot_img = cv::Mat::zeros(src_img.cols, src_img.rows, CV_8UC3);

	//描画　contoursなので全点列
	for (int i = 0; i < dot.contours.size(); i++){
		for (int j = 0; j < dot.contours[i].size() - 1; j++){
			int y1 = dot.contours[i].at(j).first;
			int x1 = dot.contours[i].at(j).second;
			int y2 = dot.contours[i].at(j + 1).first;
			int x2 = dot.contours[i].at(j + 1).second;
			if (j == 0){
				line(dot_img, cv::Point(x1, y1), cv::Point(x2, y2), cv::Scalar(0, 0, 255), .5);
			}
			else if (j == dot.contours[i].size() - 2)
				circle(dot_img, cv::Point(x2, y2), .5, cv::Scalar(0, 255, 255), -1, 4);
			else
				line(dot_img, cv::Point(x1, y1), cv::Point(x2, y2), cv::Scalar(255, 255, 255), .5);
		}
	}
	cv::imshow("line_img", dot_img);

	//描画　dividecontours
	for (int i = 0; i < dot.divideContours.size(); i++){
		for (int j = 0; j < dot.divideContours[i].size(); j++){
			int y = dot.divideContours[i].at(j).y;
			int x = dot.divideContours[i].at(j).x;
			circle(dot_img, cv::Point(x, y), 2, cv::Scalar(0, 255, 0), -1, 4);

		}
	}
	//描画　divcon=3点続いたら間引くやつ
	/*for (int i = 0; i < dot.divcon.size(); i++){
		for (int j = 0; j < dot.divcon[i].size(); j++){
			int y = dot.divcon[i].at(j).y;
			int x = dot.divcon[i].at(j).x;
			circle(dot_img, cv::Point(x, y), 2, cv::Scalar(0, 255, 0), -1, 4);
		}
	}*/

	//描画　corner
	/*for (int i = 0; i < dot.corners.size(); i++){
		for (int j = 0; j < dot.corners[i].size(); j++){
			int y = dot.corners[i].at(j).y;
			int x = dot.corners[i].at(j).x;
			circle(dot_img, cv::Point(x, y), 2, cv::Scalar(0, 0, 255), -1, 4);

		}
	}*/

	cv::imshow("dot_img", dot_img);
	//cv::imshow("dot_divcon_img", dot_divcon_img);
	cv::imwrite("dot_img.png", dot_img);
	//doVoronoi(dot_img, result_img, dot.corners, dot.divcon);
	//doVoronoi(src_img, voronoi_dot_img, dot.contours);
	cv::imshow("dot_corner_img", dot_corner_img);
	cv::imshow("result_img", result_img);


	doNodeEdge(dot.divcon, node_array);
	//	doCatmull(catmull_img, node_array);
	cv::imshow("catmull_img", catmull_img);
	/*cv::imshow("dot_img", dot_img);
	cv::imshow("dot_corner_img", dot_corner_img);
	*/
}

//背景黒、線が白の場合、sample.jpgみたいな場合
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
	clock_t start = clock();
	//doJob(src_img, result_img);
	clock_t end = clock();

	cout << end - start << endl;
	cv::waitKey();
	return 0;
}
