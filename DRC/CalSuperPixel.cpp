#include "CalSuperPixel.h"


CalSuperPixel::CalSuperPixel(void) :  initMaxK_(65534), resdiualThreshold_(0.5), minSuperPixelNumberinOneGroup_(40)
{

}


CalSuperPixel::~CalSuperPixel(void)
{
}

void CalSuperPixel::getSuperPixel(cv::Mat rawImage_arg, cv::Mat superImage_return, std::vector<std::vector<cv::Point> > &superPixelList_return, const int m_arg, const int step_arg)
{
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
	tic1.stop();
	std::cout<<"init time = "<<tic1.getTimeMilli()<<std::endl;
	tic1.reset();
	tic1.start();
	cluster(superPixelList_return);
	tic1.stop();
	std::cout<<"1st cluster time = "<<tic1.getTimeMilli()<<std::endl;
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
		std::cout<<"reinCluster Time = "<<tic1.getTimeMilli()<<std::endl;
		//std::cout<<residualError<<std::endl;
		//residualError < resdiualThreshold_ ||
		if ( iter >=1)
		{
			break;
		}
		tic1.reset();
		tic1.start();
		reprepareSuperPixelList(superPixelList_return, newCenterPoint);
		cluster(superPixelList_return);
		tic1.stop();
		std::cout<<"iter cluster Time = "<<tic1.getTimeMilli()<<std::endl;
		/*cv::Mat img;
		img = printClusterImage(superPixelList_return);
		std::stringstream ss;
		ss<<iter<<"print.jpg";
		cv::imwrite(ss.str(), img);*/
	}
	reGroupSuperPixel(superPixelList_return);
	printClusterImage(superPixelList_return);
	//updateSuperPixelList(superPixelList_return);

	superImage_return = superPixelImage16U_;
	tic2.stop();
	std::cout<<"Totel Time = "<<tic2.getTimeMilli()<<std::endl;

	return;
	
}

void CalSuperPixel::initClusterCenter( std::vector<std::vector<cv::Point> > &superPixelList_return )
{
	int iter = 0;
	for (int ii = step_/2; ii < rows_; ii += step_ )
	{
		for (int jj = step_/2; jj < cols_; jj += step_)
		{
			superPixelList_return.push_back(std::vector<cv::Point> ());
			superPixelList_return[iter].push_back(cv::Point(5,5));
			centerl_.push_back(0.0);
			centera_.push_back(0.0);
			centerb_.push_back(0.0);
			int xtmp = 0;
			int ytmp = 0;
			//static int tmps = 0;
			//std::cout<<ii*cols_+jj - tmps<<std::endl;
			//tmps = ii*cols_+jj;
			if (ii>=1 && ii+1 < rows_ && jj>=1 && jj+1 < cols_)
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
			}
			superPixelList_return[iter][0] = cv::Point(xtmp, ytmp);
			centerl_[iter] = ((unsigned char*)labImage8UC3_.data)[3*(ii*cols_+jj)    ];
			centera_[iter] = ((unsigned char*)labImage8UC3_.data)[3*(ii*cols_+jj) + 1];
			centerb_[iter] = ((unsigned char*)labImage8UC3_.data)[3*(ii*cols_+jj) + 2];
			++iter;

		}
	}
	//int a;
	//std::cout<<iter;
	//std::cin>>a ;
	k_ = superPixelList_return.size();
	return ;
}

void CalSuperPixel::initMat()
{
	cv::cvtColor(rawImage_, labImage8UC3_, CV_RGB2Lab);
	cv::cvtColor(rawImage_, garyImage8U_, CV_RGB2GRAY);
	superPixelImage16U_.create(rawImage_.size(), CV_16UC1);
	distansImage32F_.create(rawImage_.size(), CV_32FC1);
	for (int ii = 0; ii < rows_; ++ii)
	{
		for (int jj = 0; jj < cols_; ++jj)
		{
			((unsigned short*)superPixelImage16U_.data)[ii*cols_+jj] = initMaxK_;
			//std::cout<<((unsigned short*)superImage16U_.data)[ii*cols_+jj] <<std::endl;
			((float*)distansImage32F_.data)[ii*cols_+jj] = 1E+35;
			//std::cout<<((float*)distansImage32F_.data)[ii*cols_+jj] <<std::endl;
			//((unsigned char*)labImage8UC3_.data)[3*(ii*cols_+jj)+0] = 200;
		}
	}

	
	cv::Laplacian(garyImage8U_, gradientImage8U_, CV_8U, 1);

	dislab.assign(totalPixelNumber_, -1.0);
	disxy.assign(totalPixelNumber_, -1.0);
	return;
}

void CalSuperPixel::cluster( std::vector<std::vector<cv::Point> > &superPixelList_return /*= std::vector<std::vector<cv::Point> > ()*/)
{
	int offset = 1.6*step_;
	for (std::size_t kk = 0; kk < superPixelList_return.size(); ++kk)
	{
		
		int iiMin = 0;
		int jjMin = 0;
		int x1 = std::max(0, superPixelList_return[kk][0].x-offset);
		int x2 = std::min(cols_, superPixelList_return[kk][0].x+offset);
		int y1 = std::max(0, superPixelList_return[kk][0].y-offset);
		int y2 = std::min(rows_, superPixelList_return[kk][0].y+offset);
		for (int ii = y1; ii < y2; ++ii)
		{
			for (int jj = x1; jj < x2; ++jj)
			{
				float dlab = 0.0;
				float dxy = 0.0;
				float tmpf = computeD(kk, jj, ii,
									superPixelList_return[kk][0],
										dlab, dxy);
				float locl = ((float*)distansImage32F_.data)[ii*cols_+jj];
				if (tmpf < locl)
				{
					((float*)distansImage32F_.data)[ii*cols_+jj] = tmpf;
					((unsigned short*)superPixelImage16U_.data)[ii*cols_+jj] = kk;
					dislab[ii*cols_+jj] = dlab;
					disxy[ii*cols_+jj] = dxy;
					//std::cout<<((unsigned short*)superImage16U_.data)[ii*cols_+jj]<< "   ";
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
	dxy = float(x_arg-centerPoint_arg.x)*float(x_arg-centerPoint_arg.x) + float(y_arg-centerPoint_arg.y)*float(y_arg-centerPoint_arg.y);


	return (dlab/maxlabDis_[kk]*m_ + dxy*maxxyDis_[kk] + ((unsigned char*)gradientImage8U_.data)[y_arg*cols_+x_arg]*000);
}

float CalSuperPixel::reinitClusterCenter( std::vector<std::vector<cv::Point> > &superPixelList_return, std::vector<cv::Point> &newCenterPoint )
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
			int idxTmp = ((unsigned short*)superPixelImage16U_.data)[ii*cols_+jj];
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
		residualError += fabs((meanXVec[ii]-superPixelList_return[ii][0].x) + (meanYVec[ii]-superPixelList_return[ii][0].y));
		newCenterPoint.push_back(cv::Point(meanXVec[ii],meanYVec[ii]));
	}
	
	return residualError/superPixelList_return.size();
}

void CalSuperPixel::reprepareSuperPixelList( std::vector<std::vector<cv::Point> > &superPixelList_return, const std::vector<cv::Point> &newCenterPoint )
{
	for (std::size_t kk = 0; kk < superPixelList_return.size(); kk++)
	{
		superPixelList_return[kk].clear();
		superPixelList_return[kk].push_back(newCenterPoint[kk]);

	}
	return;
}

cv::Mat CalSuperPixel::printClusterImage( std::vector<std::vector<cv::Point> > &superPixelList_return )
{
	cv::Mat printImage;
	rawImage_.copyTo(printImage);
	cv::Mat maskImage(rows_, cols_, CV_8UC1);

	for (int ii = 1; ii < rows_-1; ++ii)
	{
		for (int jj = 1; jj < cols_-1; ++jj)
		{
			((unsigned char*)maskImage.data)[ii*cols_+jj] = 1;
			if (((unsigned short*)superPixelImage16U_.data)[ii*cols_+jj] != ((unsigned short*)superPixelImage16U_.data)[ii*cols_+jj+1] )
			{
				((unsigned char*)printImage.data)[(ii*cols_+jj)*3   ] = 254;
				((unsigned char*)printImage.data)[(ii*cols_+jj)*3 +1] = 254;
				((unsigned char*)printImage.data)[(ii*cols_+jj)*3 +2] = 254;
				((unsigned char*)maskImage.data)[ii*cols_+jj] = 1;
			}
			else if (((unsigned short*)superPixelImage16U_.data)[ii*cols_+jj] != ((unsigned short*)superPixelImage16U_.data)[(ii+1)*cols_+jj])
			{
				((unsigned char*)printImage.data)[(ii*cols_+jj)*3   ] = 254;
				((unsigned char*)printImage.data)[(ii*cols_+jj)*3 +1] = 254;
				((unsigned char*)printImage.data)[(ii*cols_+jj)*3 +2] = 254;
				((unsigned char*)maskImage.data)[ii*cols_+jj] = 1;
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
	cv::waitKey(0);
	return printImage;
}

void CalSuperPixel::cutSmallSizeSuperPixel( std::vector<std::vector<cv::Point> > &superPixelList_return )
{
	updateSuperPixelList(superPixelList_return);
	for (std::size_t ii = 0; ii < superPixelList_return.size(); ++ii)
	{
		if (minSuperPixelNumberinOneGroup_ > superPixelList_return[ii].size())
		{
			for (std::size_t jj = 0; jj < superPixelList_return[ii].size(); ++jj)
			{
				((unsigned short*)superPixelImage16U_.data)[superPixelList_return[ii][jj].y*cols_+superPixelList_return[ii][jj].x] = initMaxK_;
				((float*) distansImage32F_.data)[superPixelList_return[ii][jj].y*cols_+superPixelList_return[ii][jj].x] = 1E+35;
			}
			superPixelList_return.erase(superPixelList_return.begin()+ii, superPixelList_return.begin()+ii+1);
		}
	}
	k_ = superPixelList_return.size();
	return;
}

void CalSuperPixel::updateSuperPixelList( std::vector<std::vector<cv::Point> > &superPixelList_return )
{
	std::vector<std::vector<cv::Point> > vecTmp;
	for (std::size_t ii = 0; ii < superPixelList_return.size(); ++ii)
	{
		vecTmp.push_back(std::vector<cv::Point>());
		vecTmp[ii].push_back(superPixelList_return[ii][0]);
		//superPixelList_return[ii].erase(superPixelList_return[ii].begin()+1, superPixelList_return[ii].end());
	}
	superPixelList_return.clear();
	superPixelList_return.assign(vecTmp.begin(), vecTmp.end());
	for (int ii = 0; ii < rows_; ++ii)
	{
		for (int jj = 0; jj < cols_; ++jj)
		{
			unsigned short aaaa = ((unsigned short*)superPixelImage16U_.data)[ii*cols_+jj];
			//if (aaaa >= superPixelList_return.size())
			//{
			//	//std::cout<<"!!!!!!!!!!!    "<<aaaa<<std::endl;
			//	((unsigned short*)superPixelImage16U_.data)[ii*cols_+jj] = 1;
			//	 aaaa = 1;
			//	 ((float*) distansImage32F_.data)[ii*cols_+jj] = 1E+35;

			//}
			superPixelList_return[aaaa].push_back(cv::Point(jj, ii));

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

void CalSuperPixel::reGroupSuperPixel( std::vector<std::vector<cv::Point> > &superPixelList_return )
{

	const int dx8[8] = {-1,-1,0,1,1,1,0,-1};
	const int dy8[8] = {0,-1,-1,-1,0,1,1,1};
	const int dx4[4] = {-1,0,1,0};
	const int dy4[4] = {0,-1,0,1};
	
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
							if ( 0 > nLable[nIdx] && ((unsigned short*)superPixelImage16U_.data)[oIdx] == ((unsigned short*)superPixelImage16U_.data)[nIdx])
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
			((unsigned short*)superPixelImage16U_.data)[ii*cols_+jj] = nLable[ii*cols_+jj];
		}
	}
	return;
}


void CalSuperPixel::reClusterForNoneGrouped( std::vector<std::vector<cv::Point> > &superPixelList_return /*= std::vector<std::vector<cv::Point> > ()*/ )
{

		int iiMin = 0;
		int jjMin = 0;
		for (int ii = 0; ii < rows_; ++ii)
		{
			for (int jj = 0; jj < cols_; ++jj)
			{

				if (((unsigned short*)superPixelImage16U_.data)[ii*cols_+jj] != initMaxK_ )
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
					if (fabs(float(ii-superPixelList_return[kk][0].y)) < 1.5*float(step_) &&
						fabs(float(jj-superPixelList_return[kk][0].x)) < 1.5*float(step_)  )
					{
						float tmpf = computeD(kk, jj, ii, superPixelList_return[kk][0], dislabTmp, disxyTmp);
						float locl = ((float*)distansImage32F_.data)[ii*cols_+jj];
						if (tmpf < locl)
						{
							((float*)distansImage32F_.data)[ii*cols_+jj] = tmpf;
							((unsigned short*)superPixelImage16U_.data)[ii*cols_+jj] = kk;
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

