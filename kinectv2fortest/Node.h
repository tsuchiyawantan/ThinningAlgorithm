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

	//ƒGƒbƒW‚ª1ŒÂ‚Ìê‡
	void addEdge(Node *node){
		(*edge_array.at(0)).setNode(node);
	}

	void setEdge(Edge *edge){
		edge_array.push_back(edge);
	}

	Edge *getEdge(int n){
		return edge_array.at(n);
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
