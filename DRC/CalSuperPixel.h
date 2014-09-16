#pragma once
#include "opencv2/opencv.hpp"
#include <vector>
#include <iostream>
#include <cmath>
#include <map>
#include <sstream>


class CalSuperPixel
{

private:
	cv::Mat superPixelImage16U_;
	cv::Mat distansImage32F_;
	cv::Mat labImage8UC3_;
	cv::Mat garyImage8U_;
	cv::Mat gradientImage8U_;
	cv::Mat rawImage_;
	std::vector<float> maxlabDis_;
	std::vector<float> maxxyDis_;
	std::vector<float> invxywt_;
	std::vector<float> centerl_;
	std::vector<float> centera_;
	std::vector<float> centerb_;
	std::vector<int> centery_;
	std::vector<int> centerx_;
	std::vector<float> dislab;
	std::vector<float> disxy;


	int step_;
	int k_;
	int m_;
	int cols_;
	int rows_;
	int totalPixelNumber_;
	const float resdiualThreshold_;
	///Represent the unclustered points, who has no super pixel group
	const int initMaxK_;
	const int minSuperPixelNumberinOneGroup_;
public:
	CalSuperPixel(void);
	~CalSuperPixel(void);
	void initClusterCenter(std::vector<std::vector<cv::Point> > &superPixelList_return);
	void initMat();
	void getSuperPixel(cv::Mat rawImage_arg, cv::Mat superImage_return, std::vector<std::vector<cv::Point> > &superPixelList_return = std::vector<std::vector<cv::Point> > (), const int m_arg = 0, const int step_arg = 300);

	inline float computeD(const int kk, const int x_arg, const int y_arg, const cv::Point &point_arg, float &dlab, float &dxy);
	void cluster(std::vector<std::vector<cv::Point> > &superPixelList_return = std::vector<std::vector<cv::Point> > ());

	float reinitClusterCenter(std::vector<std::vector<cv::Point> > &superPixelList_return, std::vector<cv::Point> &newCenterPoint);

	cv::Mat printClusterImage(std::vector<std::vector<cv::Point> > &superPixelList_return);
	//Delete the old SuperPixel List and use the new Cluster Center
	void reprepareSuperPixelList(std::vector<std::vector<cv::Point> > &superPixelList_return, const std::vector<cv::Point> &newCenterPoint);
	void cutSmallSizeSuperPixel(std::vector<std::vector<cv::Point> > &superPixelList_return);

	//add points to SuperPixelList_return, which before this step may content cluster center only(superPixelList_return[kk][0])
	void updateSuperPixelList(std::vector<std::vector<cv::Point> > &superPixelList_return);
	void reGroupSuperPixel(std::vector<std::vector<cv::Point> > &superPixelList_return);
	void reClusterForNoneGrouped(std::vector<std::vector<cv::Point> > &superPixelList_return = std::vector<std::vector<cv::Point> > ());

	void normalize(cv::Mat &srcImage, cv::Mat &dstImage);
};

