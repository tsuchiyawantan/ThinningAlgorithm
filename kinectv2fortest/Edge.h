#pragma once
#include <opencv2/opencv.hpp>

using namespace std;

class Edge{
private:

public:
	cv::Point edge_node;

	//�G�b�W�̃m�[�h
	Edge(cv::Point edge_node_first){
		edge_node = edge_node_first;
	}
};