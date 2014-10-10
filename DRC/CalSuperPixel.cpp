//#include "perception_detect_segment/CalSuperPixel.h"
#include "CalSuperPixel.h"



namespace drc_perception
{

  const int dx8[8] = {-1,-1,0,1,1,1,0,-1};
  	const int dy8[8] = {0,-1,-1,-1,0,1,1,1};
  	const int dx4[4] = {-1,0,1,0};
  	const int dy4[4] = {0,-1,0,1};

CalSuperPixel::CalSuperPixel(void) :  initMaxK_(65534), resdiualThreshold_(0.5), minSuperPixelNumberinOneGroup_(40), baseLine_(100.0), ifDepthImageReady_(false), useDisparity_(false)
{

}

CalSuperPixel::CalSuperPixel(cv::Mat &intrinsicM, const float &baseLine) :  initMaxK_(65534), resdiualThreshold_(0.5), minSuperPixelNumberinOneGroup_(40), baseLine_(baseLine), ifDepthImageReady_(false), useDisparity_(false)
{
  if (! intrinsicM.empty())
    {
      intrinsic32F_ = intrinsicM;
	cx_ = ((float*)intrinsicM.data)[2];
	fx_ = ((float*)intrinsicM.data)[0];
	cy_ = ((float*)intrinsicM.data)[5];
	fy_ = ((float*)intrinsicM.data)[4];
    }
}

CalSuperPixel::~CalSuperPixel(void)
{
}

void CalSuperPixel::getSuperPixel(cv::Mat rawImage_arg, cv::Mat superImage_return, std::vector<cv::Point>  &superPixelList_return, const int m_arg, const int step_arg)
{
  useDisparity_ = false;
  const bool outputTmp = false;
	cv::TickMeter tic1;
	cv::TickMeter tic2;
	tic2.start();
	tic1.start();
	step_ = step_arg;
	m_ = m_arg;
	rawImage_ = rawImage_arg;
	cols_ = rawImage_arg.size().width;
	rows_ = rawImage_arg.size().height;
	totalPixelNumber_ = cols_ * rows_;
	std::cout<<"init time = "<<std::endl;
	initMat();

	initClusterCenter(superPixelList_return);
	cv::Mat tmpDraw;
	rawImage_.copyTo(tmpDraw);
	/*for (int ii = 0; ii < superPixelList_return.size(); ++ii)
	{
		cv::circle(tmpDraw, superPixelList_return[ii][0], 3, cv::Scalar(-1));
	}*/

	//cv::imshow("center", tmpDraw);
	//cv::waitKey(0);

	maxlabDis_.assign(superPixelList_return.size(), 100);
	maxxyDis_.assign(superPixelList_return.size(), 0.9);
	invxywt_.assign(superPixelList_return.size(), step_*step_);
if (outputTmp)
{
  tic1.stop();
  std::cout<<"init time = "<<tic1.getTimeMilli()<<std::endl;
  tic1.reset();
  tic1.start();
}
	cluster(superPixelList_return);
if (outputTmp)
{
	tic1.stop();
	std::cout<<"1st cluster time = "<<tic1.getTimeMilli()<<std::endl;
}
	//cv::imshow("center", distansImage32F_);
	//cv::waitKey(0);
int iter = 0;
tic1.reset();
	while (true)
	{
		tic1.reset();
		tic1.start();
		iter++;
		std::vector<cv::Point> newCenterPoint;
		float residualError = reinitClusterCenter(superPixelList_return, newCenterPoint);
if (outputTmp)
{
		tic1.stop();
		std::cout<<"reinCluster Time = "<<tic1.getTimeMilli()<<std::endl;
		//std::cout<<residualError<<std::endl;
}
		//residualError < resdiualThreshold_ ||
		if ( iter >=1)
		{
			break;
		}
		tic1.reset();
		tic1.start();
		reprepareSuperPixelList(superPixelList_return, newCenterPoint);
		cluster(superPixelList_return);
if (outputTmp)
{
		tic1.stop();
		std::cout<<"iter cluster Time = "<<tic1.getTimeMilli()<<std::endl;
}
		/*cv::Mat img;
		img = printClusterImage(superPixelList_return);
		std::stringstream ss;
		ss<<iter<<"print.jpg";
		cv::imwrite(ss.str(), img);*/
	}
	reGroupSuperPixel(superPixelList_return);
	printClusterImage(superPixelList_return);
	//updateSuperPixelList(superPixelList_return);

	superImage_return = superPixelImage32S_;
if (outputTmp)
  {
tic2.stop();
std::cout<<"Totel Time = "<<tic2.getTimeMilli()<<std::endl;
  }

	return;
	
}

void CalSuperPixel::initClusterCenter( std::vector<cv::Point>  &superPixelList_return )
{
	int iter = 1;
	int labelCount = 1;
	const int centerNum = (rows_-step_/2)*(cols_-step_/2);
	superPixelList_return.resize(centerNum+cols_);
	(*labelList_).resize(centerNum+cols_);
	centerl_.resize(centerNum+cols_);
	centera_.resize(centerNum+cols_);
	centerb_.resize(centerNum+cols_);
	centerx_.resize(centerNum+cols_);
	centery_.resize(centerNum+cols_);
	centerDisparity_.resize(centerNum+cols_);

	for (int ii = step_/2; ii < rows_; ii += step_ )
	{
		for (int jj = step_/2; jj < cols_; jj += step_)
		{
		    int iiLimt = 1;
		    int jjLimt = 1;
		    signed char iiSign = 1;
		    signed char jjSign = 1;
		    int iiT = ii;
		    int jjT = jj;
		    int iiLowerBound = ii - step_/2;
		    int iiUpperBound = ii + step_/2;
		    int jjLowerBound = jj - step_/2;
		    int jjUpperBound = jj + step_/2;
		    bool ifFindInitCenter = false;
		    bool ifAddii= true;
		    int Sign = iiSign;
		    int ijLimt = iiT+ iiLimt*iiSign;
		    int *ij = &iiT;
		    while (iiT > iiLowerBound && iiT < iiUpperBound
			&& jjT > jjLowerBound && jjT < jjUpperBound)
		      {
			break;
			std::cout<<iiT<<"    "<<jjT<<std::endl;

			for (; (*ij) < ijLimt; (*ij) += Sign)
			  {
			      if (((float*)disparityImage32F_.data)[iiT*cols_+jjT] > 1E-20)
				{
				  ifFindInitCenter = true;
				  break;
				}
			  }
			if (ifAddii)
			  {
			    ifAddii = false;
			    iiSign *= -1;
			    iiLimt++;
			    Sign = jjSign;
			    ijLimt = jjT + jjLimt*jjSign;
			    ij = &jjT;
			  }
			else
			  {
			    ifAddii = true;
			    jjSign *= -1;
			    jjLimt++;
			    Sign = iiSign;
			    ijLimt = iiT + iiLimt*iiSign;
			    ij = &iiT;
			  }
			 if (ifFindInitCenter)
			   break;
		      }
		    if (ifFindInitCenter)
		      {
			superPixelList_return[iter] = cv::Point(jjT, iiT);
			(*labelList_)[iter] = labelCount;
			labelCount++;
		      }
		    else
		      {
			superPixelList_return[iter] =  cv::Point(jjT, iiT);
			(*labelList_)[iter] = 0;
		      }

			centerl_[iter] = 0.0;
			centera_[iter] = 0.0;
			centerb_[iter] = 0.0;
			centerx_[iter] = 0.0;
			centery_[iter] = 0.0;
			centerDisparity_[iter] = 0.0;
			int xtmp = 0;
			int ytmp = 0;
			//static int tmps = 0;
			//std::cout<<ii*cols_+jj - tmps<<std::endl;
			//tmps = ii*cols_+jj;
			/*if (ii>=1 && ii+1 < rows_ && jj>=1 && jj+1 < cols_)
			{
				unsigned char gradientMin =255;
				
				for (int nn = -1; nn <= 1; ++nn)
				{
					for (int mm = -1; mm <= 1;++mm)
					{
						unsigned char tmp = ((unsigned char*)gradientImage8U_.data)[(ii+mm)*cols_+jj+nn];
						if ( tmp < gradientMin)
						{
							gradientMin = tmp;
							ytmp = ii + mm;
							xtmp = jj + nn;
						}
					}
				}


			}
			else
			{
				std::cout<<"initClusterCenterError!!!!!!!!!!!"<<std::endl;
			}*/
			//superPixelList_return[iter] = cv::Point(xtmp, ytmp);
			centerl_[iter] = ((unsigned char*)labImage8UC3_.data)[3*(iiT*cols_+jjT)    ];
			centera_[iter] = ((unsigned char*)labImage8UC3_.data)[3*(iiT*cols_+jjT) + 1];
			centerb_[iter] = ((unsigned char*)labImage8UC3_.data)[3*(iiT*cols_+jjT) + 2];
			centerx_[iter] = jjT;
			centery_[iter] = iiT;
			centerDisparity_[iter] = ((float*)disparityImage32F_.data)[(iiT*cols_+jjT)];
			++iter;

		}
	}
	//int a;
	//std::cout<<iter;
	//std::cin>>a ;
	superPixelList_return.resize(iter+1);
	centerl_.resize(iter+1);
	centera_.resize(iter+1);
	centerb_.resize(iter+1);
	centerx_.resize(iter+1);
	centery_.resize(iter+1);
	centerDisparity_.resize(iter+1);
	(*labelList_).resize(iter+1);
	k_ = superPixelList_return.size();
	return ;
}

void CalSuperPixel::initMat()
{
	superPixelImage32S_.create(rawImage_.size(), CV_32SC1);
	distansImage32F_.create(rawImage_.size(), CV_32FC1);

	//std::cout<<fx_<<" "<<cx_<<" "<<fy_<<" "<<cy_<<std::endl;
	static float fb = fx_*baseLine_;
	depthImage_= std::vector<Eigen::Vector3f > (totalPixelNumber_, Eigen::Vector3f(0.0, 0.0, 0.0));
	validDepthImage_ = std::vector<bool>(totalPixelNumber_,  false);
	//std::cout<<depthImage_.size()<<std::endl;
	if (useDisparity_)
	  {
	    ifDepthImageReady_ = true;
	    for (int ii = 0; ii < rows_; ++ii)
	    {

		    for (int jj = 0; jj < cols_; ++jj)
		    {

			//init all Pixels group as -1
			    ((int*)superPixelImage32S_.data)[ii*cols_+jj] = -1;
			    //std::cout<<((unsigned short*)superImage16U_.data)[ii*cols_+jj] <<std::endl;
			    ((float*)distansImage32F_.data)[ii*cols_+jj] = 1E+35;
			    //std::cout<<disparityImage32F_.size()<<std::endl;
			    //std::cout<<"   "<<ii<<"    "<<jj<<"   ";
			    if ( 0 == ((float*)disparityImage32F_.data)[ii*cols_+jj])
			      {

				((float*)disparityImage32F_.data)[ii*cols_+jj] = 1E-35;
				validDepthImage_[ii*cols_+jj] = false;

			      }
			    //std::cout<<ii<<"    "<<jj<<std::endl;
			   /* else
			      {
				((float*)disparityImage32F_.data)[ii*cols_+jj] = ((float*)disparityImage32F_.data)[ii*cols_+jj] ;

			      }*/
			    //std::cout<<((float*)distansImage32F_.data)[ii*cols_+jj] <<std::endl;
			    //z = fb / d;
			     // f*b = 3909.375
			     //  z = fb/disparity;
			    float z = fb / ((float*)disparityImage32F_.data)[ii*cols_+jj];
			    depthImage_[ii*cols_+jj](2) = z;
			    // y = z*(row -cy) / fy;
			    // x = z*(col - cx) / fx
			    depthImage_[ii*cols_+jj](1) = z * (ii- cy_) / fy_;
			    depthImage_[ii*cols_+jj](0) = z * (jj - cx_) / fx_;
			    validDepthImage_[ii*cols_+jj] = true;


		    }
	    }
	  }
	else
	  {
	    for (int ii = 0; ii < rows_; ++ii)
	    	{
	    		for (int jj = 0; jj < cols_; ++jj)
	    		{
	    			((int*)superPixelImage32S_.data)[ii*cols_+jj] = -1;
	    			//std::cout<<((unsigned short*)superImage16U_.data)[ii*cols_+jj] <<std::endl;
	    			((float*)distansImage32F_.data)[ii*cols_+jj] = 1E+35;
	    			//std::cout<<((float*)distansImage32F_.data)[ii*cols_+jj] <<std::endl;
	    		}
	    	}
	  }

	cv::cvtColor(rawImage_, labImage8UC3_, CV_RGB2Lab);
	cv::Sobel(rawImage_, lapImage8UC3_, CV_8U, 2, 2, 5, 1.2);
	cv::imshow("Lap", lapImage8UC3_);
	cv::Mat tmp;
	cv::bilateralFilter(lapImage8UC3_, tmp, 5, 150.0, 150.0);
	cv::imshow("bilateralLap", tmp);

	cv::waitKey(9999);
	lapImage8UC3_ = tmp;
	labImage8UC3_ = lapImage8UC3_;
	cv::cvtColor(rawImage_, garyImage8U_, CV_RGB2GRAY);
	cv::Laplacian(garyImage8U_, gradientImage8U_, CV_8U, 1);

	dislab.assign(totalPixelNumber_, -1.0);
	disxy.assign(totalPixelNumber_, -1.0);

	disDisparity_.assign(totalPixelNumber_, -1.0);
	calNormalwithDepth();
	return;
}

void CalSuperPixel::cluster( std::vector<cv::Point>  &superPixelList_return )
{
	int offset = 1.5*step_;
	for (std::size_t kk = 0; kk < superPixelList_return.size(); ++kk)
	{

		int iiMin = 0;
		int jjMin = 0;
		int x1 = std::max(0, superPixelList_return[kk].x-offset);
		int x2 = std::min(cols_, superPixelList_return[kk].x+offset);
		int y1 = std::max(0, superPixelList_return[kk].y-offset);
		int y2 = std::min(rows_, superPixelList_return[kk].y+offset);
		for (int ii = y1; ii < y2; ++ii)
		{
			for (int jj = x1; jj < x2; ++jj)
			{
				float dlab = 0.0;
				float dxy = 0.0;
				float dDisparity = .0;
				float tmpf = 0;
				if (useDisparity_)
				  {

				    if ( 1E-20 > ((float*)disparityImage32F_.data)[ii*cols_+jj])
				      {


					((float*)distansImage32F_.data)[ii*cols_+jj] = 1E+34;
					((int*)superPixelImage32S_.data)[ii*cols_+jj] = 0;
					dislab[ii*cols_+jj] = dlab;
					disxy[ii*cols_+jj] = dxy;
					disDisparity_[ii*cols_+jj] = dDisparity;
					continue;
				      }
				    else
				      {

					    tmpf = computeDDisparity(kk, jj, ii,
							superPixelList_return[kk],
							dlab,
							dxy,
							dDisparity);
				      }
				    float locl = ((float*)distansImage32F_.data)[ii*cols_+jj];
				    if (tmpf < locl)
				    	{
				    	  ((float*)distansImage32F_.data)[ii*cols_+jj] = tmpf;
				    	  ((int*)superPixelImage32S_.data)[ii*cols_+jj] = kk;
				    	  dislab[ii*cols_+jj] = dlab;
				    	  disxy[ii*cols_+jj] = dxy;
				    	  disDisparity_[ii*cols_+jj] = dDisparity;
				    	}
				  }
				else
				  {
				    tmpf = computeD(kk, jj, ii,
				    				superPixelList_return[kk],
				    				dlab,
				    				dxy);
				    float locl = ((float*)distansImage32F_.data)[ii*cols_+jj];
				    if (tmpf < locl)
				    	{
				    		((float*)distansImage32F_.data)[ii*cols_+jj] = tmpf;
				    		((int*)superPixelImage32S_.data)[ii*cols_+jj] = kk;
				    		dislab[ii*cols_+jj] = dlab;
				    		disxy[ii*cols_+jj] = dxy;
				    			//std::cout<<((unsigned short*)superImage16U_.data)[ii*cols_+jj]<< "   ";
				    	}
				  }

				

			}
			//std::cout<<std::endl;
		}
	}

	/*std::size_t size_1 = superPixelList_return.size();
	cutSmallSizeSuperPixel(superPixelList_return);
	if (size_1 > superPixelList_return.size())
	{
		reClusterForNoneGrouped(superPixelList_return);
	}*/
	return;
}

inline float CalSuperPixel::computeD(const int kk, const int x_arg, const int y_arg, const cv::Point &centerPoint_arg, float &dlab, float &dxy)
{
	float l0 = centerl_[kk];
	float a0 = centera_[kk]; 
	float b0 = centerb_[kk]; 

	float l1 = ((unsigned char*)labImage8UC3_.data)[3*(y_arg*cols_+x_arg)    ]; 
	float a1 = ((unsigned char*)labImage8UC3_.data)[3*(y_arg*cols_+x_arg) + 1]; 
	float b1 = ((unsigned char*)labImage8UC3_.data)[3*(y_arg*cols_+x_arg) + 2]; 

	dlab =(l1-l0)*(l1-l0) + (a1-a0)*(a1-a0) + (b1-b0)*(b1-b0);
	dxy = float(x_arg-centerx_[kk])*float(x_arg-centerx_[kk]) + float(y_arg-centery_[kk])*float(y_arg-centery_[kk]);
	return (dlab/maxlabDis_[kk]*m_ + dxy*maxxyDis_[kk]);
}

inline float CalSuperPixel::computeDDisparity(const int kk, const int x_arg, const int y_arg, const cv::Point &centerPoint_arg, float &dlab, float &dxy, float &dDisparity)
{
	float l0 = centerl_[kk];
	float a0 = centera_[kk];
	float b0 = centerb_[kk];
	float d0 = centerDisparity_[kk];
	//std::cout<<"asdfasetawet "<<std::endl;
	float l1 = ((unsigned char*)labImage8UC3_.data)[3*(y_arg*cols_+x_arg)    ];
	float a1 = ((unsigned char*)labImage8UC3_.data)[3*(y_arg*cols_+x_arg) + 1];
	float b1 = ((unsigned char*)labImage8UC3_.data)[3*(y_arg*cols_+x_arg) + 2];
	float d1 = ((float*)disparityImage32F_.data)[(y_arg*cols_+x_arg)];

	dlab =(l1-l0)*(l1-l0) + (a1-a0)*(a1-a0) + (b1-b0)*(b1-b0);
	dxy = float(x_arg-centerPoint_arg.x)*float(x_arg-centerPoint_arg.x) + float(y_arg-centerPoint_arg.y)*float(y_arg-centerPoint_arg.y);
	return (dlab/maxlabDis_[kk]*m_ + dxy/maxxyDis_[kk] + fabs(d1-d0)*10);
}

float CalSuperPixel::reinitClusterCenter( std::vector<cv::Point>  &superPixelList_return, std::vector<cv::Point> &newCenterPoint )
{
	float residualError = 0;
	std::vector<float> meanLVec(superPixelList_return.size(),0);
	std::vector<float> meanAVec(superPixelList_return.size(),0);
	std::vector<float> meanBVec(superPixelList_return.size(),0);
	std::vector<float> meanXVec(superPixelList_return.size(),0);
	std::vector<float> meanYVec(superPixelList_return.size(),0);
	std::vector<int> numCount(superPixelList_return.size(),0);
	for (int ii = 0; ii < rows_; ++ii)
	{
		for (int jj = 0; jj < cols_; ++jj)
		{
			int idxTmp = ((int*)superPixelImage32S_.data)[ii*cols_+jj];
			meanXVec[idxTmp] += jj;
			meanYVec[idxTmp] += ii;
			meanLVec[idxTmp] += ((unsigned char*)labImage8UC3_.data)[3*(ii*cols_+jj)    ]; 
			meanAVec[idxTmp] += ((unsigned char*)labImage8UC3_.data)[3*(ii*cols_+jj) +1 ]; 
			meanBVec[idxTmp] += ((unsigned char*)labImage8UC3_.data)[3*(ii*cols_+jj) +2 ]; 
			numCount[idxTmp]++;
		}
		
		//std::cout<<meanVector.x - superPixelList_return[kk][0].x<<std::endl;
		//std::cout<<meanVector.y - superPixelList_return[kk][0].y<<std::endl;
		//std::cout<<std::endl;
		
	}
	for (std::size_t ii = 0; ii < superPixelList_return.size(); ++ii)
	{
		meanXVec[ii] = meanXVec[ii] / numCount[ii];
		meanYVec[ii] = meanYVec[ii] / numCount[ii];
		centerl_[ii] = meanLVec[ii] / numCount[ii];
		centera_[ii] = meanAVec[ii] / numCount[ii];
		centerb_[ii] = meanBVec[ii] / numCount[ii];
		residualError += fabs((meanXVec[ii]-centerx_[ii]) + (meanYVec[ii]-centerx_[ii]));
		centerx_[ii] = meanXVec[ii];
		centery_[ii] = meanYVec[ii];
		newCenterPoint.push_back(cv::Point(meanXVec[ii],meanYVec[ii]));
	}
	
	return residualError/superPixelList_return.size();
}

void CalSuperPixel::reprepareSuperPixelList( std::vector<cv::Point>  &superPixelList_return, const std::vector<cv::Point> &newCenterPoint )
{
	for (std::size_t kk = 0; kk < superPixelList_return.size(); kk++)
	{
		//superPixelList_return[kk].clear();
		//superPixelList_return[kk].push_back(newCenterPoint[kk]);

	}
	return;
}

cv::Mat CalSuperPixel::printClusterImage( std::vector<cv::Point>  &superPixelList_return )
{
	cv::Mat printImage;
	rawImage_.copyTo(printImage);
	//cv::Mat maskImage(rows_, cols_, CV_8UC1);

	for (int ii = 1; ii < rows_-1; ++ii)
	{
		for (int jj = 1; jj < cols_-1; ++jj)
		{
			//((unsigned char*)maskImage.data)[ii*cols_+jj] = 1;
			if (((int*)superPixelImage32S_.data)[ii*cols_+jj] != ((int*)superPixelImage32S_.data)[ii*cols_+jj+1] )
			{
				((unsigned char*)printImage.data)[(ii*cols_+jj)*3   ] = 254;
				((unsigned char*)printImage.data)[(ii*cols_+jj)*3 +1] = 0;
				((unsigned char*)printImage.data)[(ii*cols_+jj)*3 +2] = 254;
				//((unsigned char*)maskImage.data)[ii*cols_+jj] = 1;
			}
			else if (((int*)superPixelImage32S_.data)[ii*cols_+jj] != ((int*)superPixelImage32S_.data)[(ii+1)*cols_+jj])
			{
				((unsigned char*)printImage.data)[(ii*cols_+jj)*3   ] = 254;
				((unsigned char*)printImage.data)[(ii*cols_+jj)*3 +1] = 0;
				((unsigned char*)printImage.data)[(ii*cols_+jj)*3 +2] = 254;
				//((unsigned char*)maskImage.data)[ii*cols_+jj] = 1;
			}
			//else if (((unsigned short*)superPixelImage16U_.data)[ii*cols_+jj] != ((unsigned short*)superPixelImage16U_.data)[ii*cols_+jj-1])
			{
				 
			}
			//else if (((unsigned short*)superPixelImage16U_.data)[ii*cols_+jj] != ((unsigned short*)superPixelImage16U_.data)[(ii-1)*cols_+jj])
			{
			}
			
			
			
		}
	}

	cv::imshow("ClusterReslt", printImage);
	cv::setMouseCallback("ClusterReslt", CallBackFunc, (void*)this);
	cv::waitKey(300000);
	cv::imshow("clusterImage", superPixelImage32S_*1000);
	cv::waitKey(300000);
	return printImage;
}

void CalSuperPixel::cutSmallSizeSuperPixel( std::vector<cv::Point>  &superPixelList_return )
{
	updateSuperPixelList(superPixelList_return);
	/*for (std::size_t ii = 0; ii < superPixelList_return.size(); ++ii)
	{
		if (minSuperPixelNumberinOneGroup_ > superPixelList_return[ii].size())
		{
			for (std::size_t jj = 0; jj < superPixelList_return[ii].size(); ++jj)
			{
				((int*)superPixelImage32S_.data)[superPixelList_return[ii][jj].y*cols_+superPixelList_return[ii][jj].x] = initMaxK_;
				((float*) distansImage32F_.data)[superPixelList_return[ii][jj].y*cols_+superPixelList_return[ii][jj].x] = 1E+35;
			}
			superPixelList_return.erase(superPixelList_return.begin()+ii, superPixelList_return.begin()+ii+1);
		}
	}*/
	k_ = superPixelList_return.size();
	return;
}

void CalSuperPixel::updateSuperPixelList( std::vector<cv::Point>  &superPixelList_return )
{

	for (int ii = 0; ii < rows_; ++ii)
	{
		for (int jj = 0; jj < cols_; ++jj)
		{
			unsigned short aaaa = ((int*)superPixelImage32S_.data)[ii*cols_+jj];
			//if (aaaa >= superPixelList_return.size())
			//{
			//	//std::cout<<"!!!!!!!!!!!    "<<aaaa<<std::endl;
			//	((unsigned short*)superPixelImage16U_.data)[ii*cols_+jj] = 1;
			//	 aaaa = 1;
			//	 ((float*) distansImage32F_.data)[ii*cols_+jj] = 1E+35;

			//}
			//superPixelList_return[aaaa].push_back(cv::Point(jj, ii));

			if (ii*cols_+jj >= dislab.size())
			{
				int a= 0;
			}
			if (maxlabDis_[aaaa] < dislab[ii*cols_+jj])
			{
				maxlabDis_[aaaa] = dislab[ii*cols_+jj];
			}

			if (maxxyDis_[aaaa] < disxy[ii*cols_+jj])
			{
				maxxyDis_[aaaa] = disxy[ii*cols_+jj];
				invxywt_[aaaa] = 1/(disxy[ii*cols_+jj]+1);
			}

		}
	}
	int a = 0;
	return ;

}

void CalSuperPixel::reGroupSuperPixel( std::vector<cv::Point>  &superPixelList_return )
{



	const int supsz = superPixelList_return.size();
	std::vector<int> nLable(totalPixelNumber_, -1);
	std::vector<unsigned short> xVec(step_*step_*3, 0);
	std::vector<unsigned short> yVec(step_*step_*3, 0);
	int labelTmp = 0;
	int adjLabel = 0;
	int64 oIdx = 0;
	for (int ii = 0; ii < rows_; ++ii)
	{
		for (int jj = 0; jj < cols_; jj++)
		{
			if ( 0 > nLable[oIdx])
			{
				nLable[oIdx] = labelTmp;
				xVec[0] = jj;
				yVec[0] = ii;
				for (int kk = 0; kk < 4; kk++)
				{
					int x = xVec[0] + dx4[kk];
					int y = yVec[0] + dy4[kk];
					if ( x>=0 && x<cols_ && y>=0 && y<rows_)
					{
						int64 nIdx = y*cols_ + x;
						if (nLable[nIdx] >= 0)
						{
							adjLabel = nLable[nIdx];
						}
					}
				}
				int countTmp = 1;
				for (int kk = 0; kk < countTmp; ++kk)
				{
					for (int oo = 0; oo < 4; ++oo)
					{
						int x = xVec[kk] + dx4[oo];
						int y = yVec[kk] + dy4[oo];
						if ( x>=0 && x<cols_ && y>=0 && y<rows_)
						{
							int64 nIdx = y*cols_ + x;
							if ( 0 > nLable[nIdx] && ((int*)superPixelImage32S_.data)[oIdx] == ((int*)superPixelImage32S_.data)[nIdx])
							{
								xVec[countTmp] = x;
								yVec[countTmp] = y;
								nLable[nIdx] = labelTmp;
								++countTmp;
							}
						}
					}
				}
				if (countTmp <= minSuperPixelNumberinOneGroup_)
				{
					for (int cc = 0; cc < countTmp; ++cc)
					{
						nLable[yVec[cc]*cols_+xVec[cc]] = adjLabel;
					}
					--labelTmp;
				}
				++labelTmp;
			}
			oIdx++;

		}
	}
	k_ = labelTmp;
	for (int ii = 0; ii < rows_; ++ii)
	{
		for (int jj = 0; jj < cols_; jj++)
		{
			((int*)superPixelImage32S_.data)[ii*cols_+jj] = nLable[ii*cols_+jj];
		}
	}
	return;
}


void CalSuperPixel::reClusterForNoneGrouped( std::vector<cv::Point>  &superPixelList_return )
{

		int iiMin = 0;
		int jjMin = 0;
		for (int ii = 0; ii < rows_; ++ii)
		{
			for (int jj = 0; jj < cols_; ++jj)
			{

				if (((int*)superPixelImage32S_.data)[ii*cols_+jj] != initMaxK_ )
				{
					continue;
				}
				else
				{
					((float*)distansImage32F_.data)[ii*cols_+jj] = 1E+35;
				}
				for (std::size_t kk = 0; kk < superPixelList_return.size(); ++kk)
				{
			
					float dislabTmp = 0.0;
					float disxyTmp = 0.0;
					if (fabs(float(ii-superPixelList_return[kk].y)) < 1.5*float(step_) &&
						fabs(float(jj-superPixelList_return[kk].x)) < 1.5*float(step_)  )
					{
						float tmpf = computeD(kk, jj, ii, superPixelList_return[kk], dislabTmp, disxyTmp);
						float locl = ((float*)distansImage32F_.data)[ii*cols_+jj];
						if (tmpf < locl)
						{
							((float*)distansImage32F_.data)[ii*cols_+jj] = tmpf;
							((int*)superPixelImage32S_.data)[ii*cols_+jj] = kk;
							dislab[ii*cols_+jj] = dislabTmp;
							disxy[ii*cols_+jj] = disxyTmp;
						}
					}
				}
			}
			//std::cout<<std::endl;
		}

		std::size_t size_1 = superPixelList_return.size();
		return;
}

void CalSuperPixel::normalize( cv::Mat &srcImage, cv::Mat &dstImage )
{
	if (srcImage.channels() == 1)
	{

	}
	if (srcImage.channels() == 3)
	{

	}
}

void CalSuperPixel::getSuperPixelwithDisparity(cv::Mat rawImage8U_arg,
					   cv::Mat &disparityImg32F_in,
					   cv::Mat superImage_out,
					   std::vector<cv::Point>  &superPixelList_return ,
					   std::vector <int> viLabels_out,
					   std::vector<std::vector<float> > &interestingPoint_out,
					   const int m_arg ,
					   const int step_arg)

{
  useDisparity_ = true;
  const bool outputTmp = true;
  viLabels_out.clear();
  labelList_ = &viLabels_out;
  interestingPoint_out.clear();

  cv::TickMeter tic1;
  	cv::TickMeter tic2;
  	tic2.start();
  	tic1.start();
  	step_ = step_arg;
  	m_ = m_arg;
  	rawImage_ = rawImage8U_arg;

  	disparityImage32F_.create(disparityImg32F_in.size(), CV_32FC1);
  	disparityImg32F_in.copyTo(disparityImage32F_);
  	//std::cout<<disparityImage32F_.type()<<std::endl;
  	cols_ = rawImage8U_arg.size().width;
  	rows_ = rawImage8U_arg.size().height;
  	totalPixelNumber_ = cols_ * rows_;

  	initMat();

  	initClusterCenter(superPixelList_return);
  	cv::Mat tmpDraw;
  	rawImage_.copyTo(tmpDraw);
  	/*for (int ii = 0; ii < superPixelList_return.size(); ++ii)
  	{
  		cv::circle(tmpDraw, superPixelList_return[ii][0], 3, cv::Scalar(-1));
  	}*/

  	//cv::imshow("center", tmpDraw);
  	//cv::waitKey(0);

  	maxlabDis_.assign(superPixelList_return.size(), 100);
  	maxxyDis_.assign(superPixelList_return.size(), 0.9);
  	maxDisparityDis_.assign(superPixelList_return.size(), 0.9);
  	invxywt_.assign(superPixelList_return.size(), step_*step_);
if (outputTmp)
{
  	tic1.stop();
  	std::cout<<"init time = "<<tic1.getTimeMilli()<<std::endl;
  	tic1.reset();
  	tic1.start();

}
	cluster(superPixelList_return);
if (outputTmp)
{
  	tic1.stop();
  	std::cout<<"1st cluster time = "<<tic1.getTimeMilli()<<std::endl;
}
  	//cv::imshow("center", distansImage32F_);
  	//cv::waitKey(0);
  int iter = 0;
  tic1.reset();
  	while (true)
  	{
  		tic1.reset();
  		tic1.start();
  		iter++;
  		std::vector<cv::Point> newCenterPoint;
  		float residualError = reinitClusterCenter(superPixelList_return, newCenterPoint);
  		tic1.stop();
if (outputTmp)
{
    std::cout<<"reinCluster Time = "<<tic1.getTimeMilli()<<std::endl;
}
  		//std::cout<<residualError<<std::endl;
  		//residualError < resdiualThreshold_ ||
  		if ( iter >=1)
  		{
  			break;
  		}
  		tic1.reset();
  		tic1.start();
  		//reprepareSuperPixelList(superPixelList_return, newCenterPoint);
  		cluster(superPixelList_return);

if (outputTmp)
{
    tic1.stop();

std::cout<<"iter cluster Time = "<<tic1.getTimeMilli()<<std::endl;
}
  		/*cv::Mat img;
  		img = printClusterImage(superPixelList_return);
  		std::stringstream ss;
  		ss<<iter<<"print.jpg";
  		cv::imwrite(ss.str(), img);*/
  	}
  	//std::cout<<"sdfasdfasdfaser"<<std::endl;
  	//reGroupSuperPixel(superPixelList_return);
  	printClusterImage(superPixelList_return);
  	//updateSuperPixelList(superPixelList_return);

  	superImage_out = superPixelImage32S_;
if (!selectedPoint_.empty())
  {
    std::ofstream fout("/home/cuixiongyi/bagfiles/selectedPoint.txt");
std::map<int, int> map1;
    for (int ii=0; ii < selectedPoint_.size(); ++ii)
          {
	    map1[((int*)superPixelImage32S_.data)[selectedPoint_[ii].y*cols_+selectedPoint_[ii].x]] = 1;
          }
    interestingPoint_out.push_back(std::vector<float>());
    interestingPoint_out.push_back(std::vector<float>());
    interestingPoint_out.push_back(std::vector<float>());
    interestingPoint_out.push_back(std::vector<float>());
    for (int ii = 0; ii < rows_; ++ii)
      {
	for (int jj = 0; jj < cols_; ++jj)
	  {
	      if (map1.find(((int*)superPixelImage32S_.data)[ii*cols_+jj])  != map1.end())
		{
		  //fout<<selectedPoint_[ii].x<<"  "<<selectedPoint_[ii].y<<std::endl;
		  fout<<depthImage_[ii*cols_+jj](0)<<"  "<<depthImage_[ii*cols_+jj](1)<<"  "<<depthImage_[ii*cols_+jj](2)<<"  "<<((int*)superPixelImage32S_.data)[ii*cols_+jj]<<std::endl;

		  interestingPoint_out[0].push_back(ii);
		  interestingPoint_out[1].push_back(jj);
		  interestingPoint_out[2].push_back(((float*)disparityImage32F_.data)[ii*cols_+jj]);
		  interestingPoint_out[3].push_back(((int*)superPixelImage32S_.data)[ii*cols_+jj]);
		}
	  }
      }
     }
if (outputTmp)
{
tic2.stop();
std::cout<<"Totel Time = "<<tic2.getTimeMilli()<<std::endl;
}
  	return;

  return ;
}

inline bool CalSuperPixel::calNormalwithDepth()
{


    normalImage_ = std::vector<Eigen::Vector3f>  (totalPixelNumber_, Eigen::Vector3f(0.0, 0.0, 0.0));
    validNormalImage_ = std::vector<bool>  (totalPixelNumber_, false);
/*   for (int ii = 1; ii < rows_-1; ++ii)
      {
	std::cout<<ii<<std::endl;
	for (int jj = 1; jj < cols_-1; ++jj)
*/	  for (int ii = 200; ii < 250; ii+=2)
	        {
	  	std::cout<<ii<<std::endl;
	  	for (int jj = 100; jj < 800; jj+=2)

	  {
	  	  normalImage_[ii*cols_+jj](0) = 0.0;
	  	normalImage_[ii*cols_+jj](1) = 0.0;
	  	normalImage_[ii*cols_+jj](2) = 0.0;
	    if ( ! validDepthImage_[ii*cols_+jj])
	      {
		continue;
	      }
	    int iiT, jjT;
	    int validNeighborCount = 0;
	    std::vector<int> iiRecord(8);
	    std::vector<int> jjRecord(8);
	    for (int kk = 0; kk < 8; ++kk)
	      {
		iiT = dx8[kk] + ii;
		jjT = dy8[kk] + jj;
		if (validDepthImage_[iiT*cols_+jjT])
		  {

		    jjRecord[validNeighborCount] = jjT;
		    iiRecord[validNeighborCount] = iiT;
		    ++validNeighborCount;
		  }
	      }

	    if (validNeighborCount >= 6)
	      {
		Eigen::Matrix3f covariance;
		covariance.setZero();
		Eigen::Vector3f depthMean(0.0, 0.0, 0.0);
		for (int kk = 0; kk < validNeighborCount; ++kk)
		  {
		    depthMean += depthImage_[iiRecord[kk]*cols_+jjRecord[kk]];
		    //std::cout<<depthImage_[iiRecord[kk]][jjRecord[kk]]<<std::endl;
		  }
		depthMean = depthMean / validNeighborCount;

		for (int kk = 0; kk < validNeighborCount; ++kk)
		  {
		    covariance += (( depthImage_[iiRecord[kk]*cols_+jjRecord[kk]]-depthMean)*(Eigen::Vector3f( depthImage_[iiRecord[kk]*cols_+jjRecord[kk]]-depthMean).transpose()));
		  }
		//std::cout<<covariance<<std::endl;
		//std::cin>>validNeighborCount;
		Eigen::EigenSolver<Eigen::Matrix3f> eigenSolver(covariance);
		Eigen::JacobiSVD<Eigen::Matrix3f> svd(covariance, Eigen::ComputeFullV);

		double lambda0 = eigenSolver.eigenvalues()(0).real();
		double lambda1 = eigenSolver.eigenvalues()(1).real();
		double lambda2 = eigenSolver.eigenvalues()(2).real();
		int minIdx = lambda0 < lambda1 ? (lambda0 < lambda2 ? 0 : 2) : (lambda1 < lambda2 ? 1 : 2);
		//std::cout<<maxIdx<<std::endl;
		//std::cout<<eigenSolver.eigenvectors()<<std::endl;
		//std::cout<<eigenSolver.eigenvectors().col(minIdx)<<std::endl;
		//std::cout<<std::endl;
		//std::cin>>validNeighborCount;
		normalImage_[ii*cols_+jj](0) = eigenSolver.eigenvectors().col(minIdx)(0).real();
		normalImage_[ii*cols_+jj](1) = eigenSolver.eigenvectors().col(minIdx)(1).real();
		normalImage_[ii*cols_+jj](2) = eigenSolver.eigenvectors().col(minIdx)(2).real();

		double lambda3 = svd.singularValues()(0);
		double lambda4 = svd.singularValues()(1);
		double lambda5 = svd.singularValues()(2);
		std::cout<<lambda0<<"  "<<lambda1<<"  "<<lambda2<<std::endl;
		std::cout<<lambda3<<"  "<<lambda4<<"  "<<lambda5<<std::endl;
		//std::cout<<normalImage_[ii*cols_+jj]<<std::endl;
		  //normalImage_[ii][jj] =
	      }
	  }
      }

cv::Mat normalColorImage(rows_, cols_, CV_32FC3);
for (int ii = 0; ii < rows_; ++ii)
      {
	for (int jj = 0; jj < cols_; ++jj)
	  {
	    float factor = 100.;
	    ((float*)normalColorImage.data)[(ii*cols_+jj)*3   ] = factor * normalImage_[ii*cols_+jj](0);
	    ((float*)normalColorImage.data)[(ii*cols_+jj)*3 +1] = factor * normalImage_[ii*cols_+jj](1);
	    ((float*)normalColorImage.data)[(ii*cols_+jj)*3 +2] = factor * normalImage_[ii*cols_+jj](2);


	  }
      }
cv::imshow("normalColorImage", normalColorImage);
cv::waitKey(999999);
  return true;
}

 void CalSuperPixel::CallBackFunc(int event, int x, int y, int flags, void* userdata)
{
   CalSuperPixel* sp = (CalSuperPixel*)userdata;
   static bool ifPressL = false;
     if  ( event == cv::EVENT_LBUTTONDOWN )
     {
	 ifPressL = true;
          std::cout << "Left button of the mouse is clicked - position (" << x << ", " << y << ")" << std::endl;
     }
     else if  ( event == cv::EVENT_RBUTTONDOWN )
     {
	 std::cout << "Right button of the mouse is clicked - position (" << x << ", " << y << ")" << std::endl;
     }
     else if  ( event == cv::EVENT_MBUTTONDOWN )
     {
	 std::cout << "Middle button of the mouse is clicked - position (" << x << ", " << y << ")" << std::endl;
     }
     else if ( event == cv::EVENT_MOUSEMOVE )
     {
	 if (ifPressL)
	   {
	     sp->selectedPoint_.push_back(cv::Point(x,y));
	   }
	 std::cout << "Mouse move over the window - position (" << x << ", " << y << ")" << std::endl;

     }
     else if( event == cv::EVENT_LBUTTONUP )
       {
	 ifPressL = false;
       }
}




// end namespace
}



