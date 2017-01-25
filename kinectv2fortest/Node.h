#pragma once
#include <opencv2/opencv.hpp>
#include "Edge.h"

using namespace std;

class Node{
private:
public:
	cv::Point node;
	vector<Edge *> edge_array;
	
	//�G�b�W���Ȃ��ꍇ
	Node(cv::Point mynode){
		node = mynode;
	}
	//�G�b�W��1����ꍇ
	Node(cv::Point mynode, cv::Point node_edge1){
		node = mynode;
		edge_array.push_back(new Edge(node_edge1));
	}
	//�G�b�W��2����ꍇ
	Node(cv::Point mynode, cv::Point node_edge1, cv::Point node_edge2){
		node = mynode;
		edge_array.push_back(new Edge(node_edge1));
		edge_array.push_back(new Edge(node_edge2));
	}
	~Node(){}

};