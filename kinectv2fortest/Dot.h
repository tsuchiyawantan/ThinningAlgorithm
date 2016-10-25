#pragma once
#include <opencv2/opencv.hpp>

#include <iostream>
#include <sstream>
#include <Windows.h>
#include <time.h>
#include <set>

using namespace std;

class Dot{
private:
public:
	set<pair<int, int>> usedDots;
	set<pair<int, int>> whiteDots;
	vector<pair<int, pair<int, int>>> priorityStart;
	vector<vector<pair<int, int>>> contours;
	vector<vector<cv::Point2f>> divideContours;
	vector<vector<cv::Point2f>> corners;

	Dot(){
		init();
	}
	~Dot(){}
	void init(){
		usedDots.clear();
		whiteDots.clear();
		priorityStart.clear();
		contours.clear();
		divideContours.clear();
	}
	void scalable(int scaleSize){
		for (int i = 0; i < divideContours.size(); i++){
			for (int j = 0; j < divideContours[i].size(); j++){
				int y = (divideContours[i].at(j).y)*scaleSize;
				int x = (divideContours[i].at(j).x)*scaleSize;
				divideContours[i].at(j) = cv::Point2f(x, y);
			}
		}
	}

	/*void makeSpace(int spaceSize){
		for (int i = 0; i < contours.size(); i++){
			vector<pair<int, int>> ctr;
			int j = 0;
			ctr.push_back(make_pair(contours[i].at(0).first, contours[i].at(0).second));
			for (j = spaceSize; j < contours[i].size(); j = j + spaceSize){
				ctr.push_back(make_pair(contours[i].at(j).first, contours[i].at(j).second));
			}
			if (j > contours[i].size()) ctr.push_back(make_pair(contours[i].back().first, contours[i].back().second));
			divideContours.push_back(ctr);
		}
	}*/

	void setWhiteDots(cv::Mat &srcImg){
		unsigned char *p;
		for (int y = 0; y < srcImg.rows; y++){
			p = &srcImg.at<uchar>(y, 0);
			for (int x = 0; x < srcImg.cols; x++){
				if (*p == 255){
					whiteDots.insert(make_pair(y, x));
				}
				p++;
			}
		}
	}
	int countW8(cv::Mat& srcImg, int y, int x) {
		int n[8][2] = { { 0, 1 }, { 1, 1 }, { 1, 0 }, { 1, -1 }, { 0, -1 }, { -1, -1 }, { -1, 0 }, { -1, 1 } };
		int count = 0;
		for (int i = 0; i < 8; i++) {
			int dy = y + n[i][0];
			int dx = x + n[i][1];
			if (dy < 0 || dy >= srcImg.rows || dx < 0 || dx >= srcImg.cols) continue;
			if (srcImg.at<uchar>(dy, dx) == 255) count++;
		}
		return count;
	}
	//���������true
	boolean countW8(cv::Mat& srcImg, pair<int, int> point){
		int n[8][2] = { { 0, 1 }, { 1, 1 }, { 1, 0 }, { 1, -1 }, { 0, -1 }, { -1, -1 }, { -1, 0 }, { -1, 1 } };
		int count = 0;
		int y = point.first;
		int x = point.second;
		for (int i = 0; i < 8; i++) {
			int dy = y + n[i][0];
			int dx = x + n[i][1];
			if (dy < 0 || dy >= srcImg.rows || dx < 0 || dx >= srcImg.cols) continue;
			if (srcImg.at<uchar>(dy, dx) == 255) return true;
		}
		return false;
	}
	void findStart(cv::Mat &srcImg){
		for (auto itr = whiteDots.begin(); itr != whiteDots.end(); ++itr) {
			int y = (*itr).first;
			int x = (*itr).second;
			priorityStart.push_back(make_pair(countW8(srcImg, y, x), make_pair(y, x)));
		}
		sort(priorityStart.begin(), priorityStart.end());
	}
	bool isExistS(int y, int x, set<pair<int, int>> &s){
		if (s.find(make_pair(y, x)) == s.end()) return 0;
		return 1;
	}
	bool isExistV(int y, int x, vector<pair<int, int>> &v){
		auto itr = find(v.begin(), v.end(), make_pair(y, x));
		if (itr == v.end()) return 0;
		return 1;
	}
	bool insertYX(cv::Mat &srcImg, vector<pair<int, int>> &ctr, int y, int x, vector<int> &dir){
		vector<pair<int, int>> n = { { 0, 1 }, { 1, 1 }, { 1, 0 }, { 1, -1 }, { 0, -1 }, { -1, -1 }, { -1, 0 }, { -1, 1 } };
		vector<int> j = { 0, 1, -1, 2, -2, 3, -3, 4, -4, 5, -5, 6, -6, 7, -7 };
		int i = 0;
		while (i < j.size()){
			int d = dir.back();
			d = d + j.at(i);
			if (d < 0) d = d + 8;
			if (d > 7) d = d % 8;
			int dy = y + n.at(d).first;
			int dx = x + n.at(d).second;
			if (dy < 0 || dy >= srcImg.rows || dx < 0 || dx >= srcImg.cols);
			else if (srcImg.at<uchar>(dy, dx) == 255 && !isExistS(dy, dx, usedDots)) {
				ctr.push_back(make_pair(dy, dx));
				usedDots.insert(make_pair(dy, dx));
				dir.push_back(d);
				return 1;
			}
			i++;
		}
		return 0;
	}
	void checkUsed8(cv::Mat &srcImg, vector<pair<int, int>> &ctr, vector<int> &dir, int y, int x){
		vector<pair<int, int>> n = { { 0, 1 }, { 1, 1 }, { 1, 0 }, { 1, -1 }, { 0, -1 }, { -1, -1 }, { -1, 0 }, { -1, 1 } };
		vector<int> j = { 0, 1, -1, 2, -2, 3, -3, 4, -4, 5, -5, 6, -6, 7, -7 };
		int i = 0;
		while (i < j.size()) {
			int d = dir.back();
			d = d + j.at(i);
			if (d < 0) d = d + 8;
			if (d > 7) d = d % 8;
			int dy = y + n.at(d).first;
			int dx = x + n.at(d).second;
			if (dy < 0 || dy >= srcImg.rows || dx < 0 || dx >= srcImg.cols);
			else if (srcImg.at<uchar>(dy, dx) == 255 && isExistS(dy, dx, usedDots) && !isExistV(dy, dx, ctr)){
				ctr.push_back(make_pair(dy, dx));
				break;
			}
			i++;
		}
	}
	void makeLine(cv::Mat &srcImg){
		vector<pair<int, int>> ctr;//��̓_��̓��ꕨ
		for (auto itr = priorityStart.begin(); itr != priorityStart.end(); ++itr){
			//start�����łɎg�����_�łȂ����
			if (!isExistS((*itr).second.first, (*itr).second.second, usedDots)){
				vector<int> dir;
				int y = (*itr).second.first;
				int x = (*itr).second.second;
				//�[�_�ł��낤x, y
				ctr.push_back(make_pair(y, x));
				usedDots.insert(make_pair(y, x));
				dir.push_back(0);
				//������8�ߖT����A���̓_��̓_������΁A�����ctr�̃X�^�[�g�̑O�ɓ����
				checkUsed8(srcImg, ctr, dir, y, x);
				while (insertYX(srcImg, ctr, y, x, dir)){
					y = ctr.back().first;
					x = ctr.back().second;
				}
				//�����ł�����B���̓_�������ctr�̍Ō�ɓ����
				checkUsed8(srcImg, ctr, dir, y, x);
				if (ctr.size() > 10)
					contours.push_back(ctr);
				ctr.clear();
				dir.clear();
			}
		}
	}
	//�I�_�i���_�j���߂���΃}�[�W����
	void mergeLine(cv::Mat &srcImg){
		for (int i = 0; i < contours.size(); i++){
			int lastY = contours[i].at(contours[i].size() - 1).first;
			int lastX = contours[i].at(contours[i].size() - 1).second;
			int n[24][2] = { { 0, 1 }, { 1, 1 }, { 1, 0 }, { 1, -1 }, { 0, -1 }, { -1, -1 }, { -1, 0 }, { -1, 1 },
			{ -1, 2 }, { 0, 2 }, { 1, 2 }, { 2, 2 }, { 2, 1 }, { 2, 0 }, { 2, -1 }, { 2, -2 },
			{ 1, -2 }, { 0, -2 }, { -1, -2 }, { -2, -2 }, { -2, -1 }, { -2, 0 }, { -2, 1 }, { -2, 2 } };
			int count = 0;
			for (int l = 0; l < 24; l++) {
				int dy = lastY + n[l][0];
				int dx = lastX + n[l][1];
				if (dy < 0 || dy >= srcImg.rows || dx < 0 || dx >= srcImg.cols) continue;
				if (srcImg.at<uchar>(dy, dx) == 255){
					for (int j = 0; j < contours.size(); j++){
						int apxY = contours[j].at(contours[j].size() - 1).first;
						int apxX = contours[j].at(contours[j].size() - 1).second;
						if (dy == apxY && dx == apxX){
							//insert�Ԃ̓_�Œ�1��
							int insY = lastY;
							int insX = lastX;

							if (lastY < apxY){
								insY = ++lastY;
							}
							else if (lastY > apxY){
								insY = --lastY;
							}
							if (lastX < apxX){
								insX = ++lastX;
							}
							else if (lastX > apxX){
								insX = --lastX;
							}
							contours[j].push_back(make_pair(insY, insX));
							reverse(contours[j].begin(), contours[j].end());
							copy(contours[j].begin(), contours[j].end(), back_inserter(contours[i]));


							//�q����ꂽ�v�f�̍폜
							contours.erase(remove(contours.begin(), contours.end(), contours[j]), contours.end());
						}
					}
				}
			}
		}
	}
	void divideCon(int spaceSize){
		for (int i = 0; i < contours.size(); i++){
			vector<cv::Point2f> ctr;
			int j = 0;
			ctr.push_back(cv::Point2f(contours[i].at(0).second, contours[i].at(0).first));
			for (j = spaceSize; j < contours[i].size(); j = j + spaceSize){
				ctr.push_back(cv::Point2f(contours[i].at(j).second, contours[i].at(j).first));
			}
			if (j > contours[i].size()) ctr.push_back(cv::Point2f(contours[i].back().second, contours[i].back().first));
			divideContours.push_back(ctr);
		}
	}
	void setCorner(cv::Mat &src_img){
		pair<int, int> start;
		pair<int, int> goal;
		pair<int, int> mid;
		vector<cv::Point2f> corner;


		for (int i = 0; i < divideContours.size(); i++){
			//�ŏ��̓_�͋}���ɕω�����_�H
			corner.push_back(cv::Point2f(divideContours[i].at(0).x, divideContours[i].at(0).y));
			//2��̓_�ƒ���������
			//�����̒��_��8�ߖT�����ׂĐ^�����Ȃ�΁A�p�̉\����
			for (int j = 0; j < divideContours[i].size() - 2; j = j + 2){
				start.first = divideContours[i].at(j).y;
				start.second = divideContours[i].at(j).x;
				goal.first = divideContours[i].at(j+2).y;
				goal.second = divideContours[i].at(j+2).x;
				mid.first = (start.first + goal.first) / 2;
				mid.second = (start.second + goal.second) / 2;
				//8�ߖT���^�������p�̉\������
				if (!countW8(src_img, mid)){
					corner.push_back(cv::Point2f(divideContours[i].at(j + 1).x, divideContours[i].at(j + 1).y));
				}
			}
			//�Ō�̓_�͋}���ɕω�����_�H
			corner.push_back(cv::Point2f(divideContours[i].back().x, divideContours[i].back().y));

			corners.push_back(corner);
		}
	}
};