#pragma once
#include <opencv2/opencv.hpp>
#include "Edge.h"

using namespace std;

class Node{
private:
	cv::Point node;
	vector<Edge *> edge_array;
public:
	Node(cv::Point mynode, int edge_num){
		node = mynode;

		if (edge_num == 0);
		else if(edge_num == 1){
			edge_array.push_back(new Edge(this));
		}
		else if (edge_num == 2){
			edge_array.push_back(new Edge(this));
			edge_array.push_back(new Edge(this));
		}
	}
	~Node(){}

	//�G�b�W��1�̏ꍇ
	void addNode2Edge(Node *node){
		(*edge_array.at(0)).setNode(node);
	}

	//�G�b�W��2�̏ꍇ
	void addNode2Edge(Node *node1, Node *node2){
		(*edge_array.at(0)).setNode(node1);
		(*edge_array.at(1)).setNode(node2);
	}

	void addEdge(){}

	void getMyNode(cv::Point &mynode){
		mynode.y = node.y;
		mynode.x = node.x;
	}
	
	int getMyEdgeNum(){
		return edge_array.size();
	}
};
