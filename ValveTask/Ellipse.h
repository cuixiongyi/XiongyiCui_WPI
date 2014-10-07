#include <opencv/cv.h>
#include <opencv/cxcore.h>
#include <opencv/highgui.h>
#include <math.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>
using namespace std;

#define PI 3.14159265

typedef struct ThinTemplate
{
	int IWidth;
	int IHeight;
	int numPoints;
	int EdgeImage[900][900];
	int EdgeImageBackup[900][900];		//Need to compare to fill disconnection
}thinTem;

typedef struct LineNode
{
	int posi;			//Positions in the original image,Y
	int posj;			//X
	float direction;	//tangent direction
	int headFlag;		//0 IL head 1 Line head 2 Normal not head
	int nodeNum;
	int inflectNode;
	LineNode *next;
	LineNode *prev;
	LineNode *next_head;
}lNode;

typedef struct ImageLines
{
	lNode *NodeHead;
}iLines;

typedef struct InflectionStack
{
	CvPoint IFPoints[1000];
	int index;
}ifStack;

void EdgeDetect(IplImage *pGray,IplImage *pEdge);
void InitImageLines(iLines* IL);
void FindLines(thinTem *TT,iLines *IL);
void DisplayLines(IplImage *Img,iLines *IL);

void DiscardSLines(iLines *IL);			//Discard short lines
void DiscardNCLines(iLines *IL);			//Discard not-closed lines

void InitInflectionStack(ifStack *IS);
void ClearInflectionStack(ifStack *IS);
void FindInflections(lNode *head, ifStack *IS);

void FindEllipses(iLines *IL,IplImage *destImg);
void ClearAccumulator();
CvPoint accumulateEllyCenter(lNode *head,int width,int height,IplImage *destImage);
CvPoint* accumulateEllyCenter(lNode *head, ifStack *IS,int width,int height,IplImage *destImage);
