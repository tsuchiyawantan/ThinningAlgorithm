#pragma once
#include <opencv2/opencv.hpp>
#include "Edge.h"

using namespace std;

class Node{
private:
public:
	cv::Point node;
	vector<Edge> edge_array;
	Node(int edge_num){
		for (int i = 0; i < edge_num; i++){
			Edge edge;
			edge_array.emplace_back(edge);
		}
	}
	~Node(){}

	void setEdgeNode(int edge_num, cv::Point node1, cv::Point node2){
		edge_array.at(edge_num).setNode(node1, node2);
	}
};