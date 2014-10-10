#ifndef CALSUPERPIXEL_H_
#define CALSUPERPIXEL_H_
#include "opencv2/opencv.hpp"


//#include "pcl/common/common_headers.h"
#include <vector>
#include <iostream>
#include <cmath>
#include <map>
#include <sstream>
#include <fstream>

namespace drc_perception {

class CalSuperPixel
{

private:
	cv::Mat superPixelImage32S_;
	cv::Mat distansImage32F_;
	cv::Mat labImage8UC3_;
	cv::Mat garyImage8U_;
	cv::Mat gradientImage8U_;
	cv::Mat rawImage_;
	cv::Mat disparityImage32F_;
	cv::Mat lapImage8UC3_;
	cv::Mat intrinsic32F_;
	std::vector<float> maxlabDis_;
	std::vector<float> maxxyDis_;
	std::vector<float> maxDisparityDis_;
	std::vector<float> invxywt_;
	std::vector<float> centerl_;
	std::vector<float> centera_;
	std::vector<float> centerb_;
	std::vector<float> centerDisparity_;
	std::vector<int> centery_;
	std::vector<int> centerx_;
	std::vector<float> dislab;
	std::vector<float> disxy;
	std::vector<float> disDisparity_;
	std::vector <cv::Point> selectedPoint_;
	std::vector<int> *labelList_;
	/// rows_*cols_*3(x,y,z)
	std::vector<Eigen::Vector3f>  depthImage_;
	std::vector<Eigen::Vector3f>  normalImage_;
	std::vector<bool>  validDepthImage_;
	std::vector<bool>  validNormalImage_;


	bool useDisparity_;
	bool ifDepthImageReady_;
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
	float cx_;
	float cy_;
	float fx_;
	float fy_;
	const float baseLine_;
public:
	CalSuperPixel(void);
	CalSuperPixel(cv::Mat &intrinsicM, const float &baseLine);
	~CalSuperPixel(void);
	void initClusterCenter(std::vector<cv::Point>  &superPixelList_return);
	void initMat();
	void getSuperPixel(cv::Mat rawImage_arg, 
					   cv::Mat superImage_return,
					   std::vector<cv::Point>  &superPixelList_return ,
					   const int m_arg = 0,
					   const int step_arg = 300);

	//outPut superImage_out is 32SC1
	//interestingPoint_out is a 4 channels 2D vector
	//interestingPoint_out[0] store u
	//interestingPoint_out[1] store v
	//interestingPoint_out[2] store disparity
	//interestingPoint_out[3] store group
	void getSuperPixelwithDisparity(cv::Mat rawImage_arg,
						   cv::Mat &disparityImg_in,
						   cv::Mat superImage_out,
						   std::vector<cv::Point>  &superPixelList_return ,
						   std::vector <int> viLabels,
						   std::vector<std::vector<float> > &interestingPoint_out,
						   const int m_arg = 0,
						   const int step_arg = 300) ;
	inline float computeD(const int kk, 
						  const int x_arg, 
						  const int y_arg, 
						  const cv::Point &point_arg, 
						  float &dlab, 
						  float &dxy );

	inline float computeDDisparity(const int kk,
							  const int x_arg,
							  const int y_arg,
							  const cv::Point &point_arg,
							  float &dlab,
							  float &dxy,
							  float &dDisparity);

	void cluster(std::vector<cv::Point>  &superPixelList_return);

	float reinitClusterCenter(std::vector<cv::Point>  &superPixelList_return, std::vector<cv::Point> &newCenterPoint);

	cv::Mat printClusterImage(std::vector<cv::Point>  &superPixelList_return);
	//Delete the old SuperPixel List and use the new Cluster Center
	void reprepareSuperPixelList(std::vector<cv::Point>  &superPixelList_return, const std::vector<cv::Point> &newCenterPoint);
	void cutSmallSizeSuperPixel(std::vector<cv::Point>  &superPixelList_return);

	//add points to SuperPixelList_return, which before this step may content cluster center only(superPixelList_return[kk][0])
	void updateSuperPixelList(std::vector<cv::Point>  &superPixelList_return);
	void reGroupSuperPixel(std::vector<cv::Point>  &superPixelList_return);
	void reClusterForNoneGrouped(std::vector<cv::Point>  &superPixelList_return);

	void normalize(cv::Mat &srcImage, cv::Mat &dstImage);

	static void CallBackFunc(int event, int x, int y, int flags, void* userdata);

	inline bool calNormalwithDepth();



};

}
#endif
