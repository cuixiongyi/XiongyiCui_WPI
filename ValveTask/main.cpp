#include <opencv/cv.h>
#include <opencv/cxcore.h>
#include <opencv/highgui.h>
#include <math.h>
#include <stdlib.h>
#include "Ellipse.h"

#define ROBERT	    0
#define SOBEL	    1
#define HORIZONTAL 0
#define VERTICAL   1
#define SOBEL_MAGNITUDE 2

#define ORIGINNODE	-1
#define EAST		0
#define NEAST		1
#define NORTH		2
#define NWEST		3
#define WEST		4
#define SWEST		5
#define SOUTH		6
#define SEAST		7
//
//void InitThinTemplate(thinTem *TT, IplImage *EImage);
//void ThinEdge(thinTem *TT);
int Removalbe(thinTem *TT, int x, int y);
//void DisplayThinImage(thinTem *TT);
//void SobelEdge(const IplImage *Img, IplImage *destImg, int direction, int threshold);


void SobelEdge(const IplImage *Img, IplImage *destImg, int direction, int threshold)
{
	int SImgValues[6];
	CvScalar color, setColor;
	CvSize sizeImg = cvGetSize(Img);
	int width = sizeImg.width;
	int height = sizeImg.height;
	int i, j;
	int delta_H, delta_V, max = 0;
	if (direction == HORIZONTAL)
	{
		for (i = 1; i<height - 1; i++)
		{
			for (j = 1; j<width - 1; j++)
			{
				color = cvGet2D(Img, i - 1, j - 1);		//Left
				SImgValues[0] = color.val[0];
				color = cvGet2D(Img, i, j - 1);
				SImgValues[1] = color.val[0];
				color = cvGet2D(Img, i + 1, j - 1);
				SImgValues[2] = color.val[0];

				color = cvGet2D(Img, i - 1, j + 1);		//Right
				SImgValues[3] = color.val[0];
				color = cvGet2D(Img, i, j + 1);
				SImgValues[4] = color.val[0];
				color = cvGet2D(Img, i + 1, j + 1);
				SImgValues[5] = color.val[0];

				if (abs(SImgValues[3] + 2 * SImgValues[4] + SImgValues[5] - SImgValues[0] - 2 * SImgValues[1] - SImgValues[2])>threshold)
				{
					setColor.val[0] = 0;
				}
				else
				{
					setColor.val[0] = 255;
				}
				cvSet2D(destImg, i, j, setColor);
			}
		}
	}
	else if (direction == VERTICAL)
	{
		for (i = 1; i<height - 1; i++)
		{
			for (j = 1; j<width - 1; j++)
			{
				color = cvGet2D(Img, i + 1, j - 1);		//Bottom
				SImgValues[0] = color.val[0];
				color = cvGet2D(Img, i + 1, j);
				SImgValues[1] = color.val[0];
				color = cvGet2D(Img, i + 1, j + 1);
				SImgValues[2] = color.val[0];

				color = cvGet2D(Img, i - 1, j - 1);		//Top
				SImgValues[3] = color.val[0];
				color = cvGet2D(Img, i - 1, j);
				SImgValues[4] = color.val[0];
				color = cvGet2D(Img, i - 1, j + 1);
				SImgValues[5] = color.val[0];

				int s_mag = abs(SImgValues[3] + 2 * SImgValues[4] + SImgValues[5] - SImgValues[0] - 2 * SImgValues[1] - SImgValues[2]);
				if (s_mag>threshold)
				{
					setColor.val[0] = 0;
				}
				else
				{
					setColor.val[0] = 255;
				}
				cvSet2D(destImg, i, j, setColor);
			}
		}
	}
	else
	{
		int tempMatrix[850][850];
		for (i = 1; i<height - 1; i++)
		{
			for (j = 1; j<width - 1; j++)
			{
				color = cvGet2D(Img, i - 1, j - 1);		//Left
				SImgValues[0] = color.val[0];
				color = cvGet2D(Img, i, j - 1);
				SImgValues[1] = color.val[0];
				color = cvGet2D(Img, i + 1, j - 1);
				SImgValues[2] = color.val[0];

				color = cvGet2D(Img, i - 1, j + 1);		//Right
				SImgValues[3] = color.val[0];
				color = cvGet2D(Img, i, j + 1);
				SImgValues[4] = color.val[0];
				color = cvGet2D(Img, i + 1, j + 1);
				SImgValues[5] = color.val[0];

				delta_H = pow((float)(SImgValues[3] + 2 * SImgValues[4] + SImgValues[5] - SImgValues[0] - 2 * SImgValues[1] - SImgValues[2]), 2);

				color = cvGet2D(Img, i + 1, j - 1);		//Bottom
				SImgValues[0] = color.val[0];
				color = cvGet2D(Img, i + 1, j);
				SImgValues[1] = color.val[0];
				color = cvGet2D(Img, i + 1, j + 1);
				SImgValues[2] = color.val[0];

				color = cvGet2D(Img, i - 1, j - 1);		//Top
				SImgValues[3] = color.val[0];
				color = cvGet2D(Img, i - 1, j);
				SImgValues[4] = color.val[0];
				color = cvGet2D(Img, i - 1, j + 1);
				SImgValues[5] = color.val[0];

				delta_V = pow((float)(SImgValues[3] + 2 * SImgValues[4] + SImgValues[5] - SImgValues[0] - 2 * SImgValues[1] - SImgValues[2]), 2);

				tempMatrix[i][j] = delta_H + delta_V;

				if (tempMatrix[i][j]>threshold)
				{
					setColor.val[0] = 0;
				}
				else
				{
					setColor.val[0] = 255;
				}

				cvSet2D(destImg, i, j, setColor);

				if (tempMatrix[i][j] > max)
				{
					max = tempMatrix[i][j];
				}
			}
		}

		for (i = 1; i<height - 1; i++)
		{
			for (j = 1; j<width - 1; j++)
			{
				tempMatrix[i][j] = (int)(255 * tempMatrix[i][j] / max);
				if (tempMatrix[i][j]>threshold)
				{
					setColor.val[0] = 0;
				}
				else
				{
					setColor.val[0] = 255;
				}

				cvSet2D(destImg, i, j, setColor);
			}
		}
	}
	return;
}

void InitThinTemplate(thinTem *TT, IplImage *EImage)
{
	TT->numPoints = 0;
	CvScalar color;
	CvSize sizeImg = cvGetSize(EImage);
	TT->IWidth = sizeImg.width;
	TT->IHeight = sizeImg.height;

	int i, j;

	for (i = 1; i<TT->IHeight - 1; i++)
	{
		for (j = 1; j<TT->IWidth - 1; j++)
		{
			color = cvGet2D(EImage, i, j);
			if (color.val[0] == 255)
			{
				TT->numPoints++;
				TT->EdgeImage[i][j] = 1;
				TT->EdgeImageBackup[i][j] = 1;
			}
			else
			{
				TT->EdgeImage[i][j] = 0;
				TT->EdgeImageBackup[i][j] = 0;
			}
		}
	}
	printf("Points: %d\n", TT->numPoints);
}

void ThinEdge(thinTem *TT)
{
	int nodes = TT->numPoints;
	int i, j;
	for (i = 1; i<TT->IHeight - 1; i++)
	{
		for (j = 1; j<TT->IWidth - 1; j++)
		{
			if (Removalbe(TT, i, j) == 1)
			{
				TT->EdgeImage[i][j] = 0;
				TT->numPoints--;
			}
		}
	}
}

int Removalbe(thinTem *TT, int x, int y)
{
	static int numnot = 0;
	if (TT->EdgeImage[x][y] == 0)
	{
		return -1;
	}

	int no = TT->EdgeImage[x - 1][y];
	int nw = TT->EdgeImage[x - 1][y - 1];
	int we = TT->EdgeImage[x][y - 1];
	int sw = TT->EdgeImage[x + 1][y - 1];
	int so = TT->EdgeImage[x + 1][y];
	int se = TT->EdgeImage[x + 1][y + 1];
	int ea = TT->EdgeImage[x][y + 1];
	int ne = TT->EdgeImage[x - 1][y + 1];

	if (we == 0 && (((no | so)&(1 - ea)) | ((1 - so)&sw) | (nw&(1 - no)) | ((1 - ea)&ne&(1 - no)) | ((1 - so)&se&(1 - ea))) == 0)
	{
		return 1;
	}
	numnot++;
	return 0;			//Not removable
}

void DisplayThinImage(thinTem *TT)
{
	CvSize sizeImg;
	sizeImg.width = TT->IWidth;
	sizeImg.height = TT->IHeight;
	CvScalar setColor;
	IplImage *thinImage = cvCreateImage(sizeImg, IPL_DEPTH_8U, 1);
	int i, j;
	for (i = 1; i<TT->IHeight; i++)
	{
		for (j = 1; j<TT->IWidth; j++)
		{
			if (TT->EdgeImage[i][j] == 1)
			{
				setColor.val[0] = 255;
			}
			else
			{
				setColor.val[0] = 0;
			}
			cvSet2D(thinImage, i, j, setColor);
		}
	}
	printf("Point left %d\n", TT->numPoints);
	cvShowImage("ThinEdge", thinImage);
	return;
}



void main()
{
	char waitKey;

	IplImage *pImg, *pImgNoFilter, *pGray, *pSobel, *pEdgeSobel, *pEdge, *pTemp;
	IplImage *pEllipse;

	pImgNoFilter = cvLoadImage("ellipse1.jpg");
	pImg = cvLoadImage("ellipse1.jpg");
	//sleep(1);
	cvSmooth(pImgNoFilter, pImg, CV_GAUSSIAN, 5, 5);
	cvShowImage("Image", pImg);

	thinTem TT;				//Thin Edge Image Structure

	waitKey = cvWaitKey(0);
	if (waitKey == 'e')
	{
		pGray = cvCreateImage(cvGetSize(pImg), IPL_DEPTH_8U, 1);
		cvSplit(pImg, NULL, pGray, NULL, NULL);
		pEdge = cvCreateImage(cvGetSize(pGray), IPL_DEPTH_8U, 1);
		EdgeDetect(pGray, pEdge);
		cvShowImage("Edge", pEdge);

		/*Thinning*/
		InitThinTemplate(&TT, pEdge);
		ThinEdge(&TT);
		DisplayThinImage(&TT);

		/*Create enhanced edge picture, store in structure IL*/
		IplImage *pEllipse = cvCreateImage(cvGetSize(pImg), IPL_DEPTH_8U, 3);
		iLines *IL = new iLines;
		InitImageLines(IL);
		FindLines(&TT, IL);
		DiscardSLines(IL);
		DiscardNCLines(IL);

		ifStack IFS;
		InitInflectionStack(&IFS);

		/*Find Ellipses*/
		FindEllipses(IL, pEllipse);
		cvShowImage("FindEllipses", pEllipse);
		cvWaitKey(0);
	}

	return;
}
