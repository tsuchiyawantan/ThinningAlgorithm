#pragma once
#include <opencv2/opencv.hpp>
#include "Edge.h"

using namespace std;

class Node{
private:
public:
	cv::Point node;
	vector<Edge *> edge_array;
	
	//エッジがない場合
	Node(cv::Point mynode){
		node = mynode;
	}
	//エッジが1つある場合
	Node(cv::Point mynode, cv::Point node_edge1){
		node = mynode;
		edge_array.push_back(new Edge(node_edge1));
	}
	//エッジが2つある場合
	Node(cv::Point mynode, cv::Point node_edge1, cv::Point node_edge2){
		node = mynode;
		edge_array.push_back(new Edge(node_edge1));
		edge_array.push_back(new Edge(node_edge2));
	}
	~Node(){}

};