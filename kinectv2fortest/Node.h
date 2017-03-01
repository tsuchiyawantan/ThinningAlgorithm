#pragma once
#include <opencv2/opencv.hpp>
#include "Edge.h"

using namespace std;

class Node{
private:
	cv::Point node;
	vector<Edge *> edge_array;
public:

	Node(cv::Point mynode, int has_edge){
		node = mynode;
		if (has_edge) edge_array.push_back(new Edge(this));
	}
	~Node(){}

	//�G�b�W��node2�𖄂߂�
	void addEdge(Node *node, int i){
		(*edge_array.at(i)).setNode(node);
	}

	void setEdge(Edge *edge){
		edge_array.push_back(edge);
	}

	Edge *getEdge(int n){
		return edge_array.at(n);
	}

	int getNodeY(){
		return node.y;
	}

	int getNodeX(){
		return node.x;
	}

	void getMyNode(cv::Point &mynode){
		mynode.y = node.y;
		mynode.x = node.x;
	}
	
	int getMyEdgeNum(){
		return edge_array.size();
	}

	int hasEdge(Node *node){
		Edge *edge;
		for (int i = 0; i < edge_array.size(); i++){
			edge = edge_array.at(i);
			if ((*edge).getNode() == node) return i;
		}
		return -1;
	}
};
