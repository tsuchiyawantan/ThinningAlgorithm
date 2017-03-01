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
	//�����甒��
	//cv::threshold(result_img, result_img, 0, 255, CV_THRESH_BINARY_INV);
}

void doCatmull(cv::Mat &result_img, vector<vector<Node *>> node_array){
	catmull.init();
	catmull.drawLine(result_img, node_array, HUE);
	catmull.drawInline(result_img, HUE);
}
//points�͊p
void doVoronoi(cv::Mat &src_img, cv::Mat &result_img, vector<vector<cv::Point2f>> &points, vector<vector<pair<int, int>>> &contours){
	vor.mkVoronoiDelaunay(src_img, result_img, points, contours);
}

void doNodeEdge(cv::Mat& src_img, vector<vector<cv::Point>> divcon, vector<vector<Node *>> &node_array){
	//�m�[�h�̗p��
	for (int i = 0; i < divcon.size(); i++){
		vector<Node *> node_array_child;
		cv::Point node;

		//�G�b�W���Ȃ��ꍇ���m�[�h���E�ׂɂ��Ȃ�
		if (divcon[i].size() == 1) {
			node = divcon[i].at(0);
			node_array_child.push_back(new Node(node, 0));
			continue;
		}

		//�m�[�h�̐���
		//�G�b�W��������Ă���
		for (int j = 0; j < divcon[i].size(); j++){
			node = divcon[i].at(j);
			node_array_child.push_back(new Node(node, 1));
		}
		//�I�_�̓m�[�h���E�ׂɂ��Ȃ�
		node = divcon[i].at(divcon[i].size()-1);
		node_array_child.push_back(new Node(node, 0));

		//�m�[�h�̘A������
		Node *this_node;
		Node *prev_node;
		Node *next_node;

		for (int l = 0; l < node_array_child.size(); l++){
			if (l == 0){ //�n�_
				this_node = node_array_child.at(l);
				next_node = node_array_child.at(l + 1);
				(*this_node).addEdge(next_node, 0);
			}
			else if (l == node_array_child.size() - 1){ //�I�_
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
				(*this_node).addEdge(next_node, 0);
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

//8�ߖT��forward�������true
boolean dotExist(cv::Mat& src_img, cv::Point mid, cv::Point forward){
	int n[8][2] = { { 0, 1 }, { 1, 1 }, { 1, 0 }, { 1, -1 }, { 0, -1 }, { -1, -1 }, { -1, 0 }, { -1, 1 } };
	int count = 0;
	int y = mid.y;
	int x = mid.x;
	//���_��forward�̏ꍇ
	if (y == forward.y && x == forward.x) return true;
	for (int i = 0; i < 8; i++) {
		int dy = y + n[i][0];
		int dx = x + n[i][1];
		if (dy < 0 || dy >= src_img.rows || dx < 0 || dx >= src_img.cols) continue;
		if (dy == forward.y && dx == forward.x) return true;
	}
	return false;
}

//�_��̊p�ł��낤�_������set
void setCorner(cv::Mat& src_img, vector<vector<Node *>> &node_array, vector<vector<Node *>> &newnode_array){
	cv::Point start;
	cv::Point goal;
	cv::Point mid;
	cv::Point forward;
	vector<cv::Point2f> corner;
	vector<Node *> node_array_child;
	Node *start_node;
	Node *goal_node;
	Node *forward_node;
	int di = 0;
	int j = 0;

	for (int i = 0; i < node_array.size(); i++){
		corner.clear();
		node_array_child.clear();
		//�X�^�[�g�̓_
		//corner.push_back(cv::Point2f(divideContours[i].at(0).x, divideContours[i].at(0).y));
		di = 1;
		//2��̓_�ƒ���������
		//�����̒��_��8�ߖT��1��̓_�����Ȃ���΁A1��̓_�͊p�̉\������
		for (j = 0; j < node_array[i].size() - 2; j++){
			start_node = node_array[i].at(j);
			goal_node = node_array[i].at(j+2);
			forward_node = node_array[i].at(j+1);
			start.y = (*start_node).getNodeY();
			start.x = (*start_node).getNodeX();
			goal.y = (*goal_node).getNodeY();
			goal.x = (*goal_node).getNodeX();
			forward.y = (*forward_node).getNodeY();
			forward.x = (*forward_node).getNodeX();
			mid.y = (start.y + goal.y) / 2;
			mid.x = (start.x + goal.x) / 2;

			if (!dotExist(src_img, mid, forward)){
				corner.push_back(cv::Point2f(forward.x, forward.y));
				di = 1;
			}
			//�p����Ȃ��_��3�񑱂�����A1�_�Ԉ���
			//�l�܂������ł͂Ȃ��A�V���b�Ƃ������ɂȂ�
			if (di % 3 == 0){
				di = 0;
				node_array_child.pop_back();
			}
			node_array_child.push_back(start_node);
			di++;
		}
		//�c����2�m�[�h
		while (j < node_array[i].size()){
			node_array_child.push_back(node_array[i].at(j));
			j++;
		}
		newnode_array.push_back(node_array_child);
	}
}

void drawNode(cv::Mat &result_img, vector<vector<Node *>> node_array){
	for (int i = 0; i < node_array.size(); i++){
		for (int j = 0; j < node_array[i].size(); j++){
			Node *node = node_array[i].at(j);
			int y = (*node).getNodeY();
			int x = (*node).getNodeX();
			circle(result_img, cv::Point(x, y), 2, cv::Scalar(0, 255, 0), -1, 4);
		}
	}
}

void doDot(cv::Mat &src_img){
	Dot dot;
	vector<vector<Node *>> prenode_array;
	vector<vector<Node *>> node_array;
	dot.setWhiteDots(src_img);
	dot.findStart(src_img);
	dot.makeLine(src_img);
	dot.divideCon(SPACESIZE);
	doNodeEdge(src_img, dot.divideContours, prenode_array);
	setCorner(src_img, prenode_array, node_array);

	cv::Mat dot_img = cv::Mat(src_img.rows, src_img.cols, CV_8UC3, cv::Scalar(0, 0, 0));
	cv::Mat dot_divcon_img = cv::Mat(src_img.rows, src_img.cols, CV_8UC3, cv::Scalar(0, 0, 0));
	cv::Mat dot_corner_img = cv::Mat(src_img.rows, src_img.cols, CV_8UC3, cv::Scalar(0, 0, 0));
	cv::Mat result_img = cv::Mat::zeros(src_img.cols, src_img.rows, CV_8UC3);
	cv::Mat catmull_img = cv::Mat::zeros(src_img.cols, src_img.rows, CV_8UC3);

	//�`��@contours�Ȃ̂őS�_��
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
	
	drawNode(result_img, node_array);
	doCatmull(catmull_img, node_array);

	cv::imshow("dot_img", dot_img);
	cv::imwrite("dot_img.png", dot_img);
	cv::imshow("dot_corner_img", dot_corner_img);
	cv::imshow("result_img", result_img);


	doNodeEdge(src_img, dot.divcon, node_array);
	//	doCatmull(catmull_img, node_array);
	cv::imshow("catmull_img", catmull_img);
}

//�w�i���A�������̏ꍇ�Asample.jpg�݂����ȏꍇ
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
