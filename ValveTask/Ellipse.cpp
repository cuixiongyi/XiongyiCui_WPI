#include "Ellipse.h"

void InitImageLines(iLines* IL)
{
	IL->NodeHead=new lNode;
	IL->NodeHead->headFlag=0;
	IL->NodeHead->inflectNode=0;
	IL->NodeHead->next=NULL;
	IL->NodeHead->prev=NULL;
	IL->NodeHead->next_head=NULL;
}

void FindLines(thinTem *TT,iLines *IL)
{
	int width=TT->IWidth;
	int height=TT->IHeight;
	int numPoints=TT->numPoints;
	int i,j;
	lNode *p=IL->NodeHead;		//p: track heads,pp: track nodes in one line
	int line_flag=0;
	int m=0,n=0;
	for(i=1;i<height-1;i++)
	{
		for(j=1;j<width-1;j++)
		{
			if(TT->EdgeImage[i][j]==1)
			{
				lNode *lineHead=new lNode;				//Add a new head to the list
				p->next_head=lineHead;					//Move p to new head
				p=p->next_head;
				lNode *pp=lineHead;
				lineHead->headFlag=1;
				lineHead->nodeNum=1;
				lineHead->posi=i;
				lineHead->posj=j;
				lineHead->inflectNode=0;
				lineHead->next=NULL;
				lineHead->prev=NULL;
				lineHead->next_head=NULL;
				/*Set itself to 0*/
				TT->EdgeImage[i][j]=0;
				line_flag=1;
				/*Find new adjacent points and add them to the list*/
				int k=i,l=j;
				while(line_flag==1)
				{
					lNode *line_Node;
					int no=TT->EdgeImage[k-1][l];
					int nw=TT->EdgeImage[k-1][l-1];
					int we=TT->EdgeImage[k][l-1];
					int sw=TT->EdgeImage[k+1][l-1];
					int so=TT->EdgeImage[k+1][l];
					int se=TT->EdgeImage[k+1][l+1];
					int ea=TT->EdgeImage[k][l+1];
					int ne=TT->EdgeImage[k-1][l+1];
					if((no|nw|we|sw|so|se|ea|ne)==1)		//Exists an adjacent node
					{
						p->nodeNum++;					//Number of Nodes
						line_Node=new lNode;
						line_Node->next=NULL;
						line_Node->posi=k;
						line_Node->posj=l;
						line_Node->headFlag=2;
						line_Node->inflectNode=0;
						line_Node->prev=pp;
						pp->next=line_Node;
						pp=pp->next;
					}
					else
					{
						line_flag=0;			//No adjacent points
						k=-1;
						l=-1;
						continue;
					}
					if(se==1)					//Find that point
					{
						TT->EdgeImage[k+1][l+1]=0;
						k=k+1;
						l=l+1;				//Good for watch
						continue;
					}
					else if(ea==1)
					{
						TT->EdgeImage[k][l+1]=0;
						k=k;
						l=l+1;
						continue;
					}
					else if(ne==1)
					{
						TT->EdgeImage[k-1][l+1]=0;
						k=k-1;
						l=l+1;
						continue;
					}
					else if(no==1)
					{
						TT->EdgeImage[k-1][l]=0;
						k=k-1;
						l=l;
						continue;
					}
					else if(nw==1)
					{
						TT->EdgeImage[k-1][l-1]=0;
						k=k-1;
						l=l-1;
						continue;
					}
					else if(we==1)
					{
						TT->EdgeImage[k][l-1]=0;
						k=k;
						l=l-1;
						continue;
					}
					else if(sw==1)
					{
						TT->EdgeImage[k+1][l-1]=0;
						k=k+1;
						l=l-1;
						continue;
					}
					else						//South
					{
						TT->EdgeImage[k+1][l]=0;
						k=k+1;
						l=l;
						continue;
					}
				}
			}
		}
	}
}

void DisplayLines(IplImage *Img,iLines *IL)
{
	CvSize sizeImg=cvGetSize(Img);
	int width=sizeImg.width;
	int height=sizeImg.height;
	CvScalar setColor;

	int i=0,j=0;
	lNode *lineHeads=IL->NodeHead;
	while(lineHeads->next_head!=NULL)
	{
		cvCircle(Img,cvPoint(lineHeads->next_head->posj,lineHeads->next_head->posi),5,CV_RGB(255,0,0),2);
		printf("%dth head",i++);
		setColor.val[0]=((int)rand())%256;
		setColor.val[1]=((int)rand())%256;
		setColor.val[2]=((int)rand())%256;
		printf(" Colors--%f %f %f-- ",setColor.val[0],setColor.val[1],setColor.val[2]);
		lNode *oneLine=lineHeads->next_head;
		
		while(oneLine->next!=NULL)
		{
			j++;
			cvSet2D(Img,oneLine->next->posi,oneLine->next->posj,setColor);
			oneLine=oneLine->next;
		}
		printf(" %d\n",j);
/*		if(j==598 || j==514)			//TODO:This is a test, delete it
		{
			CvPoint pt=cvPoint(oneLine->posj,oneLine->posi);
			cvCircle(Img,pt,10,CV_RGB(255,0,0));
		}*/
		j=0;
		lineHeads=lineHeads->next_head;
	}
}

void EdgeDetect(IplImage *pGray,IplImage *pEdge)
{
	if(pGray!=NULL && pEdge!=NULL)
	{
		cvCanny(pGray,pEdge,100,50);
	}
}

void DiscardSLines(iLines *IL)
{
	lNode *p=IL->NodeHead;
	while(p->next_head!=NULL)
	{
		if(p->next_head->nodeNum<50)
		{
			lNode *temp=p->next_head;
			p->next_head=temp->next_head;
			//TODO: Free everything of temp;
		//	printf("Discard Short: %d,%d\n",temp->posi,temp->posj);
		}
		else
		{
			p=p->next_head;
		}
	}
}

void DiscardNCLines(iLines *IL)
{
	lNode *p=IL->NodeHead;
	while(p->next_head!=NULL)
	{
		lNode *pp=p->next_head;
		int i;
		for(i=0;i<p->next_head->nodeNum-2;i++)		//Find last point
		{
			pp=pp->next;
		}
		if((abs(pp->posi-p->next_head->posi)+abs(pp->posj-p->next_head->posj))>5)
		{
			lNode *temp=p->next_head;
			p->next_head=temp->next_head;
			//TODO: Free temp
		//	printf("Discard Not Close: %d,%d\n",temp->posi,temp->posj);
		}
		if(p->next_head!=NULL)
		{
			p=p->next_head;
		}
	}
}

void InitInflectionStack(ifStack *IS)
{
	IS->index=0;
}

void FindInflections(lNode *head, ifStack *IS)		//Call this function after discard short lines
{
	int nodeCount=0;
	lNode *p=head->next->next->next;
	nodeCount=3;
	while(p!=NULL && nodeCount<head->nodeNum-3)
	{
		int pi1,pi2,pi3,ni1,ni2,ni3;
		int pj1,pj2,pj3,nj1,nj2,nj3;

		pi1=p->prev->posi;
		pi2=p->prev->prev->posi;
		pi3=p->prev->prev->prev->posi;
		ni1=p->next->posi;
		ni2=p->next->next->posi;
		ni3=p->next->next->next->posi;

		pj1=p->prev->posj;
		pj2=p->prev->prev->posj;
		pj3=p->prev->prev->prev->posj;
		nj1=p->next->posj;
		nj2=p->next->next->posj;
		nj3=p->next->next->next->posj;

	/*	if( (pi1-p->posi+pi2-p->posi+pi3-p->posi>1 && ni1-p->posi+ni2-p->posi+ni3-p->posi>1) || (pi1-p->posi+pi2-p->posi+pi3-p->posi<-1 && ni1-p->posi+ni2-p->posi+ni3-p->posi<-1))
		{
			IS->IFPoints[IS->index].x=p->posj;
			IS->IFPoints[IS->index].y=p->posi;
			p->inflectNode=1;
			IS->index++;
			printf("Add one\n");
		}*/
	//	else if((pj1-p->posj+pj2-p->posj+pj3-p->posj<-2 && nj1-p->posj+nj2-p->posj+nj3-p->posj>2) || (pj1-p->posj+pj2-p->posj+pj3-p->posj>2 && nj1-p->posj+nj2-p->posj+nj3-p->posj<-2))
		if(abs(pj2-nj2)+abs(pj3-nj3)<2 && ((pj2-p->posj)+(pj3-p->posj))*((nj2-p->posj)+(nj3-p->posj))>0 && abs(pj2-pj3)+abs(pj1-pj2)>1 && abs(nj2-nj3)+abs(nj1-nj2)>1)
		{
			IS->IFPoints[IS->index].x=p->posj;
			IS->IFPoints[IS->index].y=p->posi;
		//	printf("Inflection Point: %d %d \n",IS->IFPoints[IS->index].x,IS->IFPoints[IS->index].y);
			p->inflectNode=1;
			IS->index++;
		//	printf("Added one\n");
			printf("%d %d\n",p->posj,p->posi);
			printf("%d %d\n",pj1,pi1);
			printf("%d %d\n",nj1,ni1);
			printf("%d %d\n",pj2,pi2);
			printf("%d %d\n",nj2,ni2);
		}
		p=p->next;
		nodeCount++;
	}
}

long Accumulator[900][900][3];
ofstream ofs;
static int ellynum=0;

void FindEllipses(iLines *IL,IplImage *destImg)
{
	int width=destImg->width;
	int height=destImg->height;

	ofs.open("Ellipse.log");
	ofs<<"Four arguments of all ellipses:"<<endl;
	ofs<<"Ellipse#  CenterX     CenterY    Major Axis   Minor Axis"<<endl;

	lNode *p=IL->NodeHead->next_head;
	ifStack IFS;
	InitInflectionStack(&IFS);
	printf("Start looking for ellipses, wait for some time...\n");
	while(p!=NULL)
	{
		ClearAccumulator();
		FindInflections(p,&IFS);
		if(IFS.index==0)
		{
			
			CvPoint center=accumulateEllyCenter(p,width,height,destImg);
			cvCircle(destImg,center,5,CV_RGB(0,255,255),2);
		}
		else
		{
			CvPoint *centers=accumulateEllyCenter(p,&IFS,width,height,destImg);
			cvCircle(destImg,centers[0],5,CV_RGB(0,0,255),2);
			cvCircle(destImg,centers[1],5,CV_RGB(0,0,255),2);
		}
		ClearInflectionStack(&IFS);
		p=p->next_head;
	}
	printf("Find all ellipses.\n");
}

CvPoint accumulateEllyCenter(lNode *head,int width,int height,IplImage *destImage)
{
//	printf("Single Ellipse\n");
	int i,j;
	int pmax=-1;
	for(i=0;i<height;i++)
	{
		for(j=0;j<width;j++)
		{
			pmax=-1;
			lNode *p=head;
			while(p!=NULL)
			{
				long dis=pow((float)p->posi-i,2)+pow((float)p->posj-j,2);
				if(dis>pmax)
				{
					pmax=dis;
				}
				p=p->next;
			}
			Accumulator[i][j][0]=pmax;
		}
	}

	int pmin=9999999,pmini,pminj;
	for(i=0;i<height;i++)
	{
		for(j=0;j<width;j++)
		{
			if(Accumulator[i][j][0]<pmin)
			{
				pmin=Accumulator[i][j][0];
				pmini=i;
				pminj=j;
			}
		}
	}
	int maxis=(int)sqrt((float)pmin);
	int minaxis=99999999;

	/*Find Minor Axis*/
	lNode *p=head;
	while(p!=NULL)
	{
		int temp=pow(p->posi-pmini,2)+pow(p->posj-pminj,2);
		if(temp<minaxis)
		{
			minaxis=temp;
		}
		p=p->next;
	}
	minaxis=(int)sqrt((float)minaxis);

	//printf("Major Axis: %d Minor Axis: %d \n",maxis,minaxis);
	//printf("Center: %d %d \n",pminj,pmini);
	cvEllipse(destImage,cvPoint(pminj,pmini),cvSize(maxis,minaxis),0,0,360,CV_RGB(0,255,255));
	ofs<<ellynum++<<"          "<<pminj<<"          "<<pmini<<"           "<<maxis<<"           "<<minaxis<<endl;
	return cvPoint(pminj,pmini);
}

CvPoint* accumulateEllyCenter(lNode *head, ifStack *IS,int width,int height,IplImage *destImage)
{
	int numEllipse=IS->index+1/2;
//	printf("Num Elly: %d\n",numEllipse);
	int ellipse_counter=0;
	CvPoint *centers=new CvPoint[numEllipse];
	int pmax=-1;
	static int counter=0;
	int i,j,k;
	for(k=0;k<numEllipse;k++)			//For only two ellipses
	{
	//	printf("Enter Calculation: %d\n",k);
		for(i=0;i<height;i++)
		{
			for(j=0;j<width;j++)
			{
				ellipse_counter=0;
				pmax=-1;
				lNode *p=head;
				while(p!=NULL)
				{
					if(p->inflectNode==1)
					{
						ellipse_counter=1-ellipse_counter;
					}
					if(ellipse_counter==k)		//Same ellipse as k indicates
					{
						long dis=pow((float)p->posi-i,2)+pow((float)p->posj-j,2);
						if(dis>pmax)
						{
							pmax=dis;
						}
					}
					Accumulator[i][j][k]=pmax;
					p=p->next;
				}
			}
		}
		
		int pmin=9999999,pmini,pminj;
		for(i=0;i<height;i++)
		{
			for(j=0;j<width;j++)
			{
				if(Accumulator[i][j][k]<pmin)
				{
					pmin=Accumulator[i][j][k];
					pmini=i;
					pminj=j;
				}
			}
		}

		int maxis=(int)sqrt((float)pmin);
		int minaxis=99999999;

		/*Find Minor Axis*/
		lNode *p=head;
		while(p!=NULL)
		{
			int temp=pow(p->posi-pmini,2)+pow(p->posj-pminj,2);
			if(temp<minaxis)
			{
				minaxis=temp;
			}
			p=p->next;
		}

		minaxis=(int)sqrt((float)minaxis);
		//printf("Major Axis: %d Minor Axis: %d \n",maxis,minaxis);
		//printf("Center: %d %d \n",pminj,pmini);
		cvEllipse(destImage,cvPoint(pminj,pmini),cvSize(maxis,minaxis),0,0,360,CV_RGB(0,255,255));
		ofs<<ellynum++<<"          "<<pminj<<"          "<<pmini<<"           "<<maxis<<"           "<<minaxis<<endl;
		centers[k].x=pminj;
		centers[k].y=pmini;
	}

	return centers;
}

void ClearAccumulator()
{
	int i,j,k;
	for(i=0;i<900;i++)
	{
		for(j=0;j<900;j++)
		{
			for(k=0;k<3;k++)
			{
				Accumulator[i][j][k]=0;
			}
		}
	}
}

void ClearInflectionStack(ifStack *IS)
{
	IS->index=0;
}
