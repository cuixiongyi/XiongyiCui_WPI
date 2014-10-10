#include "HoughEllipse.h"


HoughEllipse::HoughEllipse()
{
}


HoughEllipse::~HoughEllipse()
{
}


const int PickNumber = 5;

void HoughEllipse::doHoughEllipse(cv::Mat rawImg)
{
	_cols = rawImg.cols;
	_rows = rawImg.rows;
	_rawImg = rawImg;
	rawImg.copyTo(_drawImg);
	int siz[] = { 10, 10, 10, 10, 10 };
	//int dims[] = { rawImg.cols, rawImg.rows, 1000, 1000, 362 };
	paraSpace = cv::SparseMat(5, siz, CV_64F);
	paraMaximin = cv::SparseMat(5, siz, CV_64F);
	cv::Mat garyImg;
	cv::Mat cannyImg;
	cv::Scalar color(255, 0, 255);
	//cv::cvtColor(imRaw, imGary, CV_RGB2GRAY);

	cv::cvtColor(rawImg, garyImg, CV_BGR2GRAY);

	cv::Mat binaryImg;
	cv::threshold(garyImg, binaryImg, 50, 200, CV_ADAPTIVE_THRESH_GAUSSIAN_C);

	cv::GaussianBlur(binaryImg, binaryImg, cv::Size(3, 3), 0.5, 0.5);

	cv::Canny(binaryImg, cannyImg, 150, 80, 3);
	_cannyImg = cannyImg;

	//cv::Sobel(imGary, imCanny, CV_8UC1, 1, 1);
	#if _DEBUG
	//cv::namedWindow("THRESH_OTSU");
	cv::imshow("THRESH_OTSU", binaryImg);
	///cv::waitKey(999999);


	cv::namedWindow("canny");
	cv::imshow("canny", cannyImg);
	cv::waitKey(9999);
	//cv::destroyWindow("canny");
	cv::destroyWindow("THRESH_OTSU");
	#endif


	
	std::vector < std::vector < cv::Point > > contoursTmp;
	cv::findContours(cannyImg, contoursTmp, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
	_contoursAll.clear();
	int cc = 0;
	for (int ii = 0; ii < contoursTmp.size(); ++ii)
	for (int jj = 0; jj < contoursTmp[ii].size(); ++jj)
	{
		cc++;
	}
	_contoursAll.resize(cc);
	cc = 0;
	for (int ii = 0; ii < contoursTmp.size(); ++ii)
	for (int jj = 0; jj < contoursTmp[ii].size(); ++jj)
	{
		_contoursAll[cc] = contoursTmp[ii][jj];
		cc++;
	}
	#if _DEBUG
	cv::Mat contoursIm;
	rawImg.copyTo(contoursIm);
	//cv::drawContours(contoursIm, contoursTmp, -1, cv::Scalar(250, 0, 0), 1);
	//cv::imshow("contours", contoursIm);
	//imwrite("contours.bmp", contoursIm);

	//cv::waitKey(9999);
	//cv::destroyWindow("contours");
	#endif
	cv::Point center2(200, 200);
	int r1 = 100;
	cv::Size s1(r1, r1*0.5);
	cv::RotatedRect tmpRR2(center2, s1, 0);

	/*cv::ellipse(_drawImg, tmpRR2, cv::Scalar(20, 255, 60), 2);
	cv::circle(_drawImg, center2, r1, 30);
	cv::imshow("test", _drawImg);
	cv::waitKey(9999);*/
	std::cout << _contoursAll.size() << "!!!!!!!!!!" << std::endl;
	int count = 0;
	for (int ii = 0; count < _contoursAll.size(); ++ii)
	{
		std::vector<cv::Point> points(PickNumber);
		cv::RotatedRect tmpRR;
		//computeEllipse(ii, tmpRR);
		if (computeEllipse2())
			count++;
		//std::cout << count << std::endl;
		/*cv::ellipse(_drawImg, tmpRR, cv::Scalar(20, 255, 60), 2);
		cv::imshow("ellipse", _drawImg);
		cv::waitKey(99999);
		_rawImg.copyTo(_drawImg);*/
		//std::cout << ii << std::endl;

		//randomPickPoints2(points);
		//fitEllipse2(points, tmpRR);
		
	}
	cv::SparseMatConstIterator_<double>
		itS = paraSpace.begin<double>(),
		itS_end = paraSpace.end<double>();
	double max = -10.0;
	int idx2[5];
	for (; itS != itS_end; ++itS)
	{
		const cv::SparseMat::Node* nS = itS.node();
		double i1 = itS.value<double>();
		int i2 = nS->idx[3];
		double tmp(i1);/// itM.value<double>() );
		
		if (tmp > max)
		{
			max = tmp;
			//itST = itS;
			
			for (int ii = 0; ii < 5; ++ii)
				idx2[ii] = nS->idx[ii];
			//std::cout<<max<< std::endl;
		}
		//++itM;
	}
	std::cout<<max<< std::endl;

	cv::RotatedRect tmpRR(cv::Point2f(idx2[0] * 2, idx2[1] * 2), cv::Size2f(idx2[2] * 2, idx2[3] * 2), idx2[4]*2);
	cv::ellipse(_drawImg, tmpRR, cv::Scalar(20, 255, 60), 2);
	cv::imshow("ellipse", _drawImg);
	cv::waitKey(99999);
	
	return;
}

void HoughEllipse::randomPickPoints2(std::vector<cv::Point> &points_out)
{ 
	points_out.resize(PickNumber);
	srand(time(NULL));
	const int totalCount = 0;
	static int count = 0;
	int sz = _contoursAll.size();
	bool useLarge = false;
	if (sz > RAND_MAX)
	{
		//std::cout << "size > RAND_MAX" << std::endl;
		useLarge = true;
	}
	for (int ii = 0; ii < PickNumber; ++ii)
	{
		int k1(useLarge ? (rand() + rand() + rand()) % sz : rand() % sz);
		//std::cout << kk << std::endl;;
		points_out[ii] = _contoursAll[k1];
		
	}

	return;
}

void HoughEllipse::computeEllipse(const int &ii_in, cv::RotatedRect &rr_out)
{
	randomPickPoints(1);
		_pt1Tmp = &_pt1;
		_pt2Tmp = &_pt2;
		_pt3Tmp = &_pt3;

		computeEllipseCenter();
		
		//_rawImg.copyTo(_drawImg);
		//std::cout << _centerX << "   " << _centerY << std::endl;
		//cv::circle(_drawImg, (*_pt1Tmp), 5, -1);
		//cv::circle(_drawImg, (*_pt2Tmp), 5, -1);
		//cv::circle(_drawImg, (*_pt3Tmp), 5, -1);
		computeEllipseabc(rr_out);
		
		if (std::isnan(rr_out.size.width) || std::isinf(rr_out.size.width))
		{
			//std::cout << "#IND!!!!!!!!!!!!!" << std::endl;
			return;
		}
		/*cv::circle(_drawImg, cv::Point(_centerX, _centerY), 5, -1);
		
		cv::ellipse(_drawImg, rr_out, cv::Scalar(20, 255, 60), 2);
				
		cv::imshow("center", _drawImg);
		cv::waitKey(99999);
		_rawImg.copyTo(_drawImg);*/
		isEllipse2(rr_out);


	return;
}
void HoughEllipse::computeEllipseabc(cv::RotatedRect &rr_out)
{
	Eigen::Matrix3d mtrxA;
	double x1Tmpx = (*_pt1Tmp).x - _centerX;
	double x1Tmpy = (*_pt1Tmp).y - _centerY;
	double x2Tmpx = (*_pt2Tmp).x - _centerX;
	double x2Tmpy = (*_pt2Tmp).y - _centerY; 
	double x3Tmpx = (*_pt3Tmp).x - _centerX;
	double x3Tmpy = (*_pt3Tmp).y - _centerY; 
	mtrxA.setZero();
	mtrxA << x1Tmpx*x1Tmpx, x1Tmpx*x1Tmpy * 2, x1Tmpy*x1Tmpy,
			x2Tmpx*x2Tmpx, x2Tmpx*x2Tmpy * 2, x2Tmpy*x2Tmpy,
			x3Tmpx*x3Tmpx, x3Tmpx*x3Tmpy * 2, x3Tmpy*x3Tmpy;
	/*mtrxA << 1, 0, 0,
			0, 1, 0,
			0, 0, 1;*/
	Eigen::Vector3d b11;
	b11.setOnes();
	//factor a = ((r2*r2*cos(t)*cos(t) + r1*r1*sin(t)*sin(t)) / (r1*r1*r2*r2) )  2 * b = ((sin(t)*cos(t)*(r1*r1 + r2*r2)) / (r1*r1*r2*r2)) c = ((r2*r2*sin(t)*sin(t) + r1*r1*cos(t)*cos(t)) / (r1*r1*r2*r2))
	//b << 1.0, 1.0, 1.0;
	Eigen::Vector3d x (mtrxA.householderQr().solve(b11));
	//double relative_error = (mtrxA*x - b).norm() / b.norm(); // norm() is L2 norm
	double theta(std::asin(4 * x(1) / (x(0) + x(2))) / 2);
	double a = x(0);
	double b = x(1);
	double c = x(2);
	double d = x(2);
	double a1(cos(theta)*cos(theta)), c2(a1);
	double a2(sin(theta)*sin(theta)), c1(a2);
	double r2sq(a / 2 + c / 2 + pow((a*a + 2 * a*c + c*c + 4 * d), 0.5) / 2), r2(std::sqrt(r2sq));
	double r1sq(a+c-r2sq), r1(std::sqrt(r1sq));
	//std::cout << "The relative error is:\n" << x << std::endl << std::endl;
	double simiMajorAxe(1.0 / std::sqrt(a > c ? a : c));
	double simiMinorAxe(1.0 / std::sqrt(a < c ? a : c));
	rr_out.center.x = _centerX;
	rr_out.center.y = _centerY;
	rr_out.size.width = 2 * simiMajorAxe;
	rr_out.size.height = 2 * simiMinorAxe;
	double Q1((2 * (c - a) + std::sqrt(4 * (c - a)*(c - a) + 4 * b*b)) / 2 / b);
	double Q2((2 * (c - a) - std::sqrt(4 * (c - a)*(c - a) + 4 * b*b)) / 2 / b);
	rr_out.angle = std::atan(Q1 > 0 ? Q1 : (Q2 > 0 ? Q2 : 0.0))*180/PI;

	return;
}
void HoughEllipse::computeEllipseCenter()
{
	double b1, m1, b2, m2, b3, m3;
	findK((*_pt1Tmp), b1, m1);
	findK((*_pt2Tmp), b2, m2);
	findK((*_pt3Tmp), b3, m3);
	
	//cv::line(_drawImg, ((*_pt1Tmp)), cv::Point((*_pt1Tmp).x+50, ((*_pt1Tmp).x + 50)*m1 + b1), -1, 2);
	double m12x(double((*_pt1Tmp).x + (*_pt2Tmp).x)/2); 
	double m12y(double((*_pt1Tmp).y + (*_pt2Tmp).y) / 2);
	double m23x(double((*_pt3Tmp).x + (*_pt2Tmp).x) / 2);
	double m23y(double((*_pt3Tmp).y + (*_pt2Tmp).y) / 2);
	double k12(double((*_pt2Tmp).y - (*_pt1Tmp).y) / double((*_pt2Tmp).x - (*_pt1Tmp).x));
	double k23(double((*_pt2Tmp).y - (*_pt3Tmp).y) / double((*_pt2Tmp).x - (*_pt3Tmp).x));
	double k4(-1.0 / k12);
	double k5(-1.0 / k23);


	/*std::cout << "p1 : " << (*_pt1Tmp).x << "  " << (*_pt1Tmp).y << std::endl;
	std::cout << "p2 : " << (*_pt2Tmp).x << "  " << (*_pt2Tmp).y << std::endl;
	std::cout << "p3 : " << (*_pt3Tmp).x << "  " << (*_pt3Tmp).y << std::endl;
	std::cout << "m12 : " << m12x << "  " << m12y << std::endl;
	std::cout << "m12 : " << m23x << "  " << m23y << std::endl;
	std::cout << "k12 k23 : " << k12 << "  " << k23 << std::endl;
	std::cout << "k4 k5 : " << k4 << "  " << k5 << std::endl;*/

	_centerX = (k4*m12x - k5*m23x + m23y - m12y) / (k4 - k5);
	_centerY = k4*(_centerX - m12x) + m12y;



	double p4x((b2 - b1) / (m1 - m2));
	double p4y(m1*p4x + b1);
	double p5x((b3 - b2) / (m2 - m3));
	double p5y(m2*p5x + b2);
	k4 = ((m12y - p4y) / (m12x - p4x));
	k5 = ((m23y - p5y) / (m23x - p5x));
	double _centerX = (k4*m12x - k5*m23x + m23y - m12y) / (k4 - k5);
	double _centerY = k4*(_centerX - m12x) + m12y;
	return;
}
void HoughEllipse::randomPickPoints(const int &ii_in)
{

	std::srand(time(NULL));
	int kk = 0;
	int sz = _contoursAll.size();
	
		_pt1ii = rand() % sz;
		
		while (1)
		{
			_pt2ii = rand() % sz;
			if (_pt1ii != _pt2ii)
				break;
		}
		while (1)
		{
			_pt3ii = rand() % sz;
			if (_pt1ii != _pt3ii && _pt3ii != _pt2ii )
				break;
		}
		_pt1 = _contoursAll[_pt1ii];
		_pt2 = _contoursAll[_pt2ii];
		_pt3 = _contoursAll[_pt3ii];

	return;
}


void HoughEllipse::findK(const cv::Point &pt_in, double &b, double &m)
{
	int x_in(pt_in.x);
	int y_in(pt_in.y);
	int scope = 3;
	if (!(x_in - scope >= 0 && x_in + scope < _cols) || !(y_in - scope >= 0 && y_in + scope < _rows))
		return;
	std::vector<cv::Point> ptList;
	for (int xx = x_in - scope; xx < x_in + scope; ++xx)
	{
		for (int yy = y_in - scope; yy < y_in + scope; ++yy)
		{
			if (((uchar*)_cannyImg.data)[yy*_cols + xx] > 0)
			{
				ptList.push_back(cv::Point(xx, yy));
				
			}
		}
	}
	Eigen::MatrixXd matrA(ptList.size(), 3);
	Eigen::VectorXd vecC(ptList.size());
	Eigen::Matrix2d matrB;
	Eigen::Vector2d vecD;
	matrB.setZero();
	vecD.setZero();
	//std::cout << matrB;
	//std::cout << ptList.size();
	for (int ii = 0; ii < ptList.size(); ++ii)
	{
		matrA(ii, 0) = ptList[ii].x;
		matrA(ii, 1) = ptList[ii].y;
		matrA(ii, 2) = 1;
		vecC(ii) = 1.0;
		matrB(0, 1) += ptList[ii].x;
		matrB(1, 1) += ptList[ii].x*ptList[ii].x;
		vecD(0) += ptList[ii].y;
		vecD(1) += ptList[ii].y*ptList[ii].x;
	}
	matrB(0, 0) = ptList.size();
	matrB(1, 0) = matrB(0, 1);

	Eigen::Vector2d x = matrB.fullPivLu().solve(vecD);
	//std::cout << x;
	b = x(0);
	m = x(1);
	//std::cout << "The least-squares solution is:\n"
		//<< matrA.colPivHouseholderQr().solve(vecC) << std::endl;

	return;
}


int g_fitEllipseMinContourPoint = 30;


bool HoughEllipse::isEllipse(const std::vector<cv::Point> &contours, const cv::RotatedRect &canEll)
{

	double x0 = canEll.center.x;
	double y0 = canEll.center.y;
	double a = canEll.size.height*0.5;
	double a2 = a * 2;
	double b = canEll.size.width*0.5;
	double angle = canEll.angle;
	double c = sqrt(a*a - b*b);
	cv::Vec2f c1(cos(angle)*c + x0, sin(angle)*c + y0);
	cv::Vec2f c2(-cos(angle)*c + x0, -sin(angle)*c + y0);
	///Calculate max and min err
	for (int i = 0; i < contours.size(); i++)
	{
		double len1 = sqrt(pow(abs(c1[0] - contours[i].x), 2) + pow(abs(c1[1] - contours[i].y), 2));
		double len2 = sqrt(pow(abs(c2[0] - contours[i].x), 2) + pow(abs(c2[1] - contours[i].y), 2));
		double errTmp = fabs(len1 + len2 - a2);
		if (errTmp > 0.5 * a2)
		{
			return false;
		}
	}

	return true;
}

bool HoughEllipse::isEllipse2(const cv::RotatedRect &canEll)
{
	///Calculate max and min err
	int idx[] { canEll.center.x / 2, canEll.center.y / 2, canEll.size.width / 2, canEll.size.height / 2, canEll.angle /2 };
	int max = 0, min = _cols;
	for (int tt = 0; tt < 5; ++tt)
	{
		if (max < idx[tt])
			max = idx[tt];
		if (min > idx[tt])
			min = idx[tt];
	}
	float factor = 1.0;
	float minFactor = 20;
	if (canEll.size.width  > float(_cols) * factor || canEll.size.height > float(_cols) * factor || min < 0 || max > _cols*factor || canEll.size.width  <minFactor || canEll.size.height < minFactor)
	{
		//std::cout << "PROblem" << std::endl;
		return false;
	}
	//std::cout << canEll.size.height << " " << canEll.size.width<< std::endl;
	//std::cout << canEll.angle<< std::endl;
	double x0 = canEll.center.x;
	double y0 = canEll.center.y;
	double a = canEll.size.height*0.5;
	double a2 = a * 2;
	double b = canEll.size.width*0.5;
	double angle = (canEll.angle-90)/180*PI;
	double c = sqrt(a*a - b*b);
	cv::Vec2f c1(cos(angle)*c + x0, sin(angle)*c + y0);
	cv::Vec2f c2(-cos(angle)*c + x0, -sin(angle)*c + y0);
	//cv::circle(_drawImg, cv::Point(c1[0], c1[1]), 4, 2,2);
	//cv::circle(_drawImg, cv::Point(c2[0], c2[1]), 4, 2,2);
	int x = _drawImg.channels();
	//cv::ellipse(_drawImg, canEll, cv::Scalar(20, 60, 255), 2);

	for (int ii = 0; ii < _contoursAll.size(); ii++)
	{
			if (std::abs(_contoursAll[ii].x - canEll.center.x) + std::abs(_contoursAll[ii].y - canEll.center.y) > a2+b)
				continue;
			double len1 = sqrt(pow(abs(c1[0] - _contoursAll[ii].x), 2) + pow(abs(c1[1] - _contoursAll[ii].y), 2));
			double len2 = sqrt(pow(abs(c2[0] - _contoursAll[ii].x), 2) + pow(abs(c2[1] - _contoursAll[ii].y), 2));
			double errTmp = fabs(len1 + len2 - a2);
			if (errTmp > 0.005 * a2)
			{
				continue;
			}	
				if (paraSpace.ptr(idx, false) == NULL)
				{
					paraMaximin.ref<double>(idx) = canEll.size.width*std::cos(canEll.angle);
					paraSpace.ref<double>(idx) = 1.0f;
				}
				else
				{
					paraSpace.ref<double>(idx) += 1.0f;
				}		
				
			/*	((uchar*)_drawImg.data)[(_contoursAll[ii].x + _contoursAll[ii].y*_cols) * 3] = 0;
				((uchar*)_drawImg.data)[(_contoursAll[ii].x + _contoursAll[ii].y*_cols) * 3+1] = 200;
				((uchar*)_drawImg.data)[(_contoursAll[ii].x + _contoursAll[ii].y*_cols) * 3+2] = 80;
				((uchar*)_drawImg.data)[(_contoursAll[ii].x+1 + _contoursAll[ii].y*_cols) * 3] = 0;
				((uchar*)_drawImg.data)[(_contoursAll[ii].x+1 + _contoursAll[ii].y*_cols) * 3 + 1] = 200;
				((uchar*)_drawImg.data)[(_contoursAll[ii].x+1 + _contoursAll[ii].y*_cols) * 3 + 2] = 80;
				((uchar*)_drawImg.data)[(_contoursAll[ii].x + (_contoursAll[ii].y+1)*_cols) * 3] = 0;
				((uchar*)_drawImg.data)[(_contoursAll[ii].x + (_contoursAll[ii].y + 1)*_cols) * 3 + 1] = 200;
				((uchar*)_drawImg.data)[(_contoursAll[ii].x + (_contoursAll[ii].y + 1)*_cols) * 3 + 2] = 80;*/
	}
	//std::cout << paraSpace.ref<double>(idx)<<std::endl;
	/*cv::imshow("vote", _drawImg);
	cv::waitKey(99999);
	_rawImg.copyTo(_drawImg);*/
	return true;
}

void HoughEllipse::voteForEllipse(const cv::RotatedRect &rr_in)
{
	return;
}

bool HoughEllipse::computeEllipse2()
{
	std::vector<cv::Point> points;
	randomPickPoints2(points);
	cv::RotatedRect rrTmp = cv::fitEllipse(points);

	if (std::isnan(rrTmp.size.width) || std::isinf(rrTmp.size.width))
		{
			return false;
		}
		/*cv::circle(_drawImg, cv::Point(_centerX, _centerY), 5, -1);

		cv::ellipse(_drawImg, rr_out, cv::Scalar(20, 255, 60), 2);

		cv::imshow("center", _drawImg);
		cv::waitKey(99999);
		_rawImg.copyTo(_drawImg);*/
		return (isEllipse2(rrTmp));

	
}
