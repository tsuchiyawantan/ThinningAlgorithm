#pragma once
#include <opencv2/opencv.hpp>

#include <iostream>
#include <sstream>
#include <Windows.h>
#include <time.h>
#include <set>
#include <unordered_map>


using namespace std;

class Voronoi{
private:
public:
	set<pair<int, int>> usedDots;
	set<pair<int, int>> whiteDots;
	vector<pair<int, pair<int, int>>> priorityStart;
	vector<vector<pair<int, int>>> contours;
	vector<vector<cv::Point2f>> corners;
	vector<cv::Point> start_goal;

	Voronoi(){
	}
	~Voronoi(){}


	//Contourのdotならばtrue, それ以外false
	boolean isContourDot(vector<pair<int, int>> &contour, int x, int y){
		vector<pair<int, int>>::iterator itr = find(contour.begin(), contour.end(), make_pair(y, x));
		if (itr != contour.end()) return true;
		return false;
	}


	//x,yに最も近い点かつ自身の点列ではない点をスタート前に入れる
	void getNearestLength(unordered_map<string, vector<cv::Point>> &edge_map, vector<cv::Point2f> &points, vector<pair<int, int>> &contour, float x, float y, int start_or_end){
		string key = to_string(x) + ',' + to_string(y);
		float min = INFINITY;
		float min_x = 0;
		float min_y = 0;
		//min_x, min_yに該当する点がある場合=true, ない場合=false;
		boolean nodot = false;
		for (int i = 0; i < edge_map[key].size(); i++){
			float end_x = edge_map[key].at(i).x;
			float end_y = edge_map[key].at(i).y;
			float length = sqrt((x - end_x)*(x - end_x) + (y - end_y)*(y - end_y));
			if (min > length && !isContourDot(contour, end_x, end_y)) {
				min_x = end_x;
				min_y = end_y;
				min = length;
				nodot = true;
			}
			//		else if (min==length)
		}

		//min_x, min_yに該当する点がない場合
		if (!nodot) return;

		//スタート点ならばstart_or_end=1
		if (start_or_end){
			vector<pair<int, int>>::iterator itr_con;
			itr_con = contour.begin();
			itr_con = contour.insert(itr_con, make_pair((int)min_y, (int)min_x));
		}
		//ゴール点はstart_or_end=0
		else{
			contour.push_back(make_pair((int)min_y, (int)min_x));
		}
	}

	void makeEdgeListMap(cv::Mat &src_img, vector<cv::Vec4f> &edge_list, unordered_map<string, vector<cv::Point>> &edge_map){
		for (auto itr = edge_list.begin(); itr != edge_list.end(); ++itr) {
			if ((*itr).val[0] < 0 || (*itr).val[1] < 0 || (*itr).val[2] < 0 || (*itr).val[3] < 0) continue;
			if ((int)(*itr).val[0] > src_img.cols || (int)(*itr).val[1] > src_img.rows || (int)(*itr).val[2] > src_img.cols || (int)(*itr).val[3] > src_img.rows) continue;
			//スタートのxyをキーとして、ゴールのxyを値とする
			string key = to_string((*itr).val[0]) + ',' + to_string((*itr).val[1]);
			edge_map[key].push_back(cv::Point2f((*itr).val[2], (*itr).val[3]));
			//ゴールのxyをキーとして、スタートのxyを値とする
			key = to_string((*itr).val[2]) + ',' + to_string((*itr).val[3]);
			edge_map[key].push_back(cv::Point2f((*itr).val[0], (*itr).val[1]));
		}
	}

	void connectNearest(cv::Mat &src_img, cv::Mat &result_img, cv::Subdiv2D &subdiv, vector<vector<cv::Point2f>> &points, vector<vector<pair<int, int>>> &contours){
		vector<cv::Vec4f> edge_list;
		unordered_map<string, vector<cv::Point>> edge_map;
		subdiv.getEdgeList(edge_list);
		makeEdgeListMap(src_img, edge_list, edge_map);

		for (int i = 0; i < points.size(); i++){

			float start_y = points[i].at(0).y;
			float start_x = points[i].at(0).x;
			float end_y = points[i].back().y;
			float end_x = points[i].back().x;
			getNearestLength(edge_map, points[i], contours[i], start_x, start_y, 1);
			getNearestLength(edge_map, points[i], contours[i], end_x, end_y, 0);
		}

		//circle(test_mat, subdiv.getVertex(vertexID, 0), 4, cv::Scalar(0, 100, 255), -1, 4);
		//cout << subdiv.getVertex(vertexID, 0) << endl;
		//circle(test_mat, cv::Point2f(points[0].at(0).x, points[0].at(0).y), 2, cv::Scalar(255, 100, 0), -1, 4);
		for (int i = 0; i < contours.size(); i++){
			for (int j = 0; j < contours[i].size() - 1; j++){
				int y1 = contours[i].at(j).first;
				int x1 = contours[i].at(j).second;
				int y2 = contours[i].at(j + 1).first;
				int x2 = contours[i].at(j + 1).second;
				/*if (j == 0){
				line(test_mat, cv::Point(x1, y1), cv::Point(x2, y2), cv::Scalar(0, 0, 255), .5);

				}
				else if (j == contours[i].size() - 1)
				line(test_mat, cv::Point(x1, y1), cv::Point(x2, y2), cv::Scalar(255, 0, 0), .5);
				else*/
				line(result_img, cv::Point(x1, y1), cv::Point(x2, y2), cv::Scalar(0, 255, 255), .5);
			}
		}
	}

	void mkVoronoiDelaunay(cv::Mat &src_img, cv::Mat &result_img, vector<vector<cv::Point2f>> &points, vector<vector<pair<int, int>>> &contours){
		cv::Subdiv2D subdiv;
		subdiv.initDelaunay(cv::Rect(0, 0, 600, 600));
		for (int i = 0; i < points.size(); i++){
			subdiv.insert(points[i]);
		}
		vector<int> idx;
		vector<std::vector<cv::Point2f>> facetLists;
		vector<cv::Point2f> facetCenters;
		subdiv.getVoronoiFacetList(idx, facetLists, facetCenters);

		/*for (int i = 0; i < points.size(); i++){
			for (int j = 0; j < points[i].size(); j++){
				int y = points[i].at(j).y;
				int x = points[i].at(j).x;
				circle(result_img, cv::Point(x, y), 2, cv::Scalar(0, 0, 255), -1, 4);
			}
		}*/

		//// Voronoi図を描画
		//for (auto list = facetLists.begin(); list != facetLists.end(); list++)
		//{
		//	cv::Point2f before = list->back();
		//	for (auto pt = list->begin(); pt != list->end(); pt++)
		//	{
		//		cv::Point p1((int)before.x, (int)before.y);
		//		cv::Point p2((int)pt->x, (int)pt->y);
		//		cv::line(result_img, p1, p2, cv::Scalar(64, 255, 128));
		//		before = *pt;
		//	}
		//}

		// 辺のリストを取得
		vector<cv::Vec4f> edgeList;
		cv::Mat imgEdges;
		subdiv.getEdgeList(edgeList);


		// 描画
		for (auto edge = edgeList.begin(); edge != edgeList.end(); edge++)
		{
			cv::Point p1(edge->val[0], edge->val[1]);
			cv::Point p2(edge->val[2], edge->val[3]);
			cv::line(imgEdges, p1, p2, cv::Scalar(48, 128, 48));
		}

		// ドロネー三角形のリストを取得
		std::vector<cv::Vec6f> triangles;
		subdiv.getTriangleList(triangles);

		// 描画
		/*for (auto it = triangles.begin(); it != triangles.end(); it++)
		{
			cv::Vec6f &vec = *it;
			cv::Point p1(vec[0], vec[1]);
			cv::Point p2(vec[2], vec[3]);
			cv::Point p3(vec[4], vec[5]);
			cv::line(result_img, p1, p2, cv::Scalar(255, 0, 0));
			cv::line(result_img, p2, p3, cv::Scalar(255, 0, 0));
			cv::line(result_img, p3, p1, cv::Scalar(255, 0, 0));
		}*/

		connectNearest(src_img, result_img, subdiv, points, contours);

	}
};