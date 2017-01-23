#pragma once
#include <opencv2/opencv.hpp>

using namespace std;

class Edge{
private:

public:
	cv::Point edge_node1;
	cv::Point edge_node2;
	Edge(){}
	~Edge(){}

	void setNode(cv::Point node1, cv::Point node2){
		edge_node1 = node1;
		edge_node2 = node2;
	}
};