#pragma once
#include<memory>
#include<memory>
#include <stdexcept>

#include <random>
#include <opencv2/opencv.hpp>
#include "opencv/cv.h"
#include "map"
#include "sstream"
#include "string"
#include <math.h>
#include <Eigen/SparseCore>
#include <Eigen/Dense>
#include <time.h>

class HoughEllipse
{
private:
	cv::Mat _rawImg;
	cv::Mat _drawImg;
	cv::Mat _cannyImg;
	int _cols;
	int _rows;
	std::vector<cv::Point> _contoursAll;
	const float PI = 3.14159;

	//parameter Space in 5-Dimention
	//1st is x0 center of ellipse
	//2nd is y0
	//3rd - 5th are a,b,c
	//or r1, r2, theta
	std::map<std::string, unsigned int> paraMap;
	cv::SparseMat paraSpace;
	cv::SparseMat paraMaximin;

	int _sampleSize;
	cv::Point  _pt1;
	cv::Point  _pt2;
	cv::Point  _pt3;
	cv::Point  _pt4;
	int _pt1ii;
	int _pt2ii;
	int _pt3ii;
	int _pt4ii;
	cv::Point  *_pt1Tmp;
	cv::Point  *_pt2Tmp;
	cv::Point  *_pt3Tmp;
	cv::Point  *_pt4Tmp;
	cv::Point  *_pt5Tmp;

	double _centerX;
	double _centerY;
public:
	HoughEllipse();
	~HoughEllipse();

	void doHoughEllipse(cv::Mat rawImg);

	void randomPickPoints(const int &ii_in);
	void computeEllipse(const int &ii_in, cv::RotatedRect &rr_out);
	void computeEllipseCenter();

	void findK(const cv::Point &pt_in, double &b, double &m);
	void computeEllipseabc(cv::RotatedRect &rr_out);
	bool computeEllipse2();
	cv::RotatedRect fitEllipse(const int &ii_in);
	bool fitEllipse(const int &ii_in, cv::RotatedRect &ellipse_out);
	
	bool isEllipse2(const cv::RotatedRect &canEll);
	bool isEllipse(const std::vector<cv::Point> &contours, const cv::RotatedRect &canEll);
	void randomPickPoints2(std::vector<cv::Point> &points_out);
	void voteForEllipse(const cv::RotatedRect &rr_in);

};

