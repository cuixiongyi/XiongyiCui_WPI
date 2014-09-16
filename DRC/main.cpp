/*
Reference
http://ivrg.epfl.ch/research/superpixels
*/
#include "opencv2/opencv.hpp"
#include <vector>
#include <iostream>
#include <ctime>

#include "CalSuperPixel.h"

int main(int argc, char** argv)
{
	cv::Mat rawImage = cv::imread("a1.jpg");
	cv::Mat depthImage = cv::imread("a1d.jpg");

	cv::Mat cannyImage;
	int sizeTmp = 10000;
	cv::Mat imgTmp(sizeTmp, sizeTmp, CV_32FC1);
	std::vector<std::vector<float> > vvTmp;
	std::clock_t timer;
	cv::TickMeter tic1;
	//timer = std::clock();
	//for (int ii = 0; ii < sizeTmp; ++ii)
	//{
	//	for (int jj = 0; jj < sizeTmp; ++jj)
	//	{
	//		((float*)imgTmp.data)[ii*sizeTmp+jj] = 1.0;
	//	}
	//}
	//tic1.start();
	//for (int ii = 0; ii < sizeTmp; ++ii)
	//{
	//	for (int jj = 0; jj < sizeTmp; ++jj)
	//	{
	//		((float*)imgTmp.data)[ii*sizeTmp+jj] = 0.0;
	//	}
	//}
	//tic1.stop();

	////std::cout<<"Mat = "<<(std::clock()-timer) / (double) CLOCKS_PER_SEC<<std::endl;
	//std::cout<<"Mat = "<<tic1.getTimeMilli()<<std::endl;
	//for (int ii = 0; ii < sizeTmp; ++ii)
	//{
	//	vvTmp.push_back(std::vector<float>());
	//	for (int jj = 0; jj < sizeTmp; ++jj)
	//	{
	//		vvTmp[ii].push_back(0.0);
	//	}
	//}
	//tic1.reset();
	//tic1.start();
	//for (int ii = 0; ii < sizeTmp; ++ii)
	//{
	//	for (int jj = 0; jj < sizeTmp; ++jj)
	//	{
	//		vvTmp[ii][jj] = 5555.0;
	//	}
	//}
	//tic1.stop();
	//std::cout<<"VV = "<<tic1.getTimeMilli()<<std::endl;
	//
	cv::Mat bilaImage;
	//cv::bilateralFilter(rawImage, bilaImage, -1, 10.0, 2.0);
	//cv::adaptiveBilateralFilter(rawImage, bilaImage, cv::Size(5,5), 10.0);

	CalSuperPixel Sample;
	std::vector<std::vector<cv::Point> > superPixelList;
	tic1.reset();
	tic1.start();
	Sample.getSuperPixel(rawImage, cannyImage , superPixelList, 15, 30);
	tic1.stop();
	std::cout<<"SuperPixel = "<<tic1.getTimeMilli()<<std::endl;
	int a ;
	std::cin>>a;
	/*cv::Mat bilaImage;
	cv::bilateralFilter(rawImage, bilaImage, -1, 10.0, 2.0);
	cv::adaptiveBilateralFilter(rawImage, bilaImage, cv::Size(5,5), 10.0);
	cv::imshow("canny", bilaImage);
	cv::waitKey(0);*/


	return 0;
}