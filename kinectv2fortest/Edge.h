#pragma once
#include <opencv2/opencv.hpp>

using namespace std;

//���ݎQ�Ƒ΍�
class Node;

class Edge{
private:
	Node *node1;
	Node *node2;
public:
	//�G�b�W�̃m�[�h
	Edge(Node *mynode){
		node1 = mynode;
	}

	void setNode(Node *node){
		node2 = node;
	}

	//�|�C���^�Ԃ�
	Node *getNode(){
		return node2;
	}
};