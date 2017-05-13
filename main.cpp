#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <algorithm>  
#include "bmpFile.h"
using namespace std;

int findNemo(int * pointlist,int n){
	sort(pointlist, pointlist + 240);
    int i = 0;  
    int MaxCount = 1;  
    int index = 0;  
  
    while (i < 239) 
    {   int count = 1;  
        int j ;  
        for (j = i; j < 240 - 1; j++)  
        {  
            if (pointlist[j] == pointlist[j + 1]) count++;  
            else break;
        }  
        if (MaxCount < count)  
        {  MaxCount = count;
           index = j;
        }  
        ++j;  
        i = j;
    }  
	return pointlist[index];
}

int getgd(BYTE* p,int x,int y){
	return *(p+y*240+x);
}

void wagepos(){
	int width,height;
	BYTE *pGryImg=RmwRead8BitBmpFile2Img("D:\\XBDCurrentFrame-2017-04-11-11-08-58.bmp",&width,&height);
	int imgSize=width*height;
	int* line = new int[480];
	int* row = new int[736];

//============================//
//===========粗定位===========//
//============================//
	BYTE* pCur = pGryImg;
	BYTE* pEnd = pGryImg+imgSize;
	int linecounter=0;
	int lineIndexer=0;
	int lineSum=0;
	for(;pCur<pEnd;pCur++){
		lineSum+=*pCur;
		row[linecounter]+=*pCur;
		linecounter++;
		if(linecounter==width){
			linecounter = 0;
			line[lineIndexer]=lineSum;
			lineSum=0;
			lineIndexer++;
		}
	}

/*	
	memset(pPrj2X,0,sizeof(int)*width);
	memset(pPrj2Y,0,sizeof(int)*height);
	for(int y=0,pCur=pGryImg;y<height;y++)
	{
		for(int x=0;x<width;x++,pCur++)
		{
			pPrj2X[x] += *pCur;
			pPrj2Y[y] += *pCur;
		}
	}
*/
	int templinesum = 0;
	int positionline;
	for(int i=0;i<240;i++){
		templinesum+=line[i];
	}
	int maxlinesum = templinesum;
	for(int i=240;i<480;i++){
		templinesum=templinesum-line[i-240]+line[i];
		if(templinesum>maxlinesum){
			maxlinesum=templinesum;
			positionline=i;
		}
	}

	int temprowsum = 0;
	int positionrow;
	for(int i=0;i<240;i++){
		temprowsum+=line[i];
	}
	int maxrowsum = temprowsum;
	for(int i=240;i<736;i++){
		temprowsum=temprowsum-row[i-240]+row[i];
		if(temprowsum>maxrowsum){
			maxrowsum=temprowsum;
			positionrow=i;
		}
	}

	BYTE *pBluImg=new BYTE [240*240];
	
	BYTE* pBCur = pBluImg;
	pCur = pGryImg;
	for(int x=positionrow-239;x<positionrow+1;x++)
		for(int y=positionline-239;y<positionline+1;y++){
			*pBCur = *(pCur+y*width+x-1);
			pBCur++;
		}
	RmwWriteByteImg2BmpFile(pBluImg,240,240,"D:\\blur.bmp");
//============================//
//===========精定位===========//
//============================//
	
	BYTE *pExatImg=new BYTE [240*240];
	BYTE* pECur = pBluImg;
	BYTE* pEStart = pBluImg;
	BYTE* pEEnd = pECur;
//==========圆心横坐标=======//
	int prel2r[240]={0};
	int taill2r[240]={0};
	for(int y=0;y<240;y++){
		pECur = pEStart+y*240;
		//前扫描点
		int temppre=0,temptail=0;
		for(;pECur<pEStart+(y+1)*240;){
			temppre++;
			if((*(pECur+1)-*(pECur))>13){
				prel2r[y]=temppre;
				temppre=0;
				break;
			}
			pECur++;

		}
		//后扫描点
		pECur = pEStart+(y+1)*240-1;
		for(;pECur>pEStart+y*240;){
			temptail++;
			if((*(pECur-1)-*(pECur))>13){
				taill2r[y]=240-temptail;
				temptail=0;
				break;
			}
			pECur--;
		}
	}
	int xpointlist[240]={0};
	for(int i=0;i<240;i++){
//		cout<<prel2r[i]<<" "<<taill2r[i]<<endl;
		xpointlist[i]=(taill2r[i]+prel2r[i])/2;
	}
	
	int xpoint = findNemo(xpointlist,240);
	


//============圆心纵坐标===========//
	int preu2b[240]={0};
	int tailu2b[240]={0};
	for(int x=0;x<240;x++){
		pECur = pEStart+x;
		//前扫描点
		int temppre=0,temptail=0;
		for(;pECur<pEStart+239*240+x;){
			temppre++;
			if((*(pECur+240)-*(pECur))>13){
				preu2b[x]=temppre;
				temppre=0;
				break;
			}
			pECur+=240;

		}
		//后扫描点
		pECur = pEStart+239*240+x;
		for(;pECur>pEStart+x;){
			temptail++;
			if((*(pECur-240)-*(pECur))>13){
				tailu2b[x]=240-temptail;
				temptail=0;
				break;
			}
			pECur-=240;
		}
	}
	int ypointlist[240]={0};
	for(int i=0;i<240;i++){
//		cout<<prel2r[i]<<" "<<taill2r[i]<<endl;
		ypointlist[i]=(tailu2b[i]+preu2b[i])/2;
	}
	
	int ypoint = findNemo(ypointlist,240);

	cout<<xpoint<<""<<ypoint<<endl;
//===============================//
//=============求半径============//
//===============================//
	int radium[960]={0};
	for(int i=0,j=0;i<240;i++){
		double temp = (xpoint-prel2r[i])*(xpoint-prel2r[i])+(ypoint-i)*(ypoint-i);
		radium[j]=sqrt(temp);
		j++;
		temp = (xpoint-taill2r[i])*(xpoint-taill2r[i])+(ypoint-i)*(ypoint-i);
		radium[j]=sqrt(temp);
		j++;
		temp = (ypoint-preu2b[i])*(ypoint-preu2b[i])+(xpoint-i)*(xpoint-i);
		radium[j]=sqrt(temp);
		j++;
		temp = (ypoint-tailu2b[i])*(ypoint-tailu2b[i])+(xpoint-i)*(xpoint-i);
		radium[j]=sqrt(temp);
		j++;
	}
	
	
	int realradium= findNemo(radium,960);
	cout<<realradium;
	
//===============================//
//=======读取测试圆直方图========//
//===============================//
	int hist[255]={0};
	for(int i=0;i<240;i++){
		if(prel2r[i]!=0){
			int tempx=(xpoint+prel2r[i])/2;
			int tempy=(ypoint+i)/2;
			int tempgd=getgd(pBluImg,tempx,tempy);
			hist[tempgd]++;
		}
		if(taill2r[i]!=0){
			int tempx=(xpoint+prel2r[i])/2;
			int tempy=(ypoint+i)/2;
			int tempgd=getgd(pBluImg,tempx,tempy);
			hist[tempgd]++;
		}
		if(preu2b[i]!=0){
			int tempx=(xpoint+prel2r[i])/2;
			int tempy=(ypoint+i)/2;
			int tempgd=getgd(pBluImg,tempx,tempy);
			hist[tempgd]++;
		}
		if(tailu2b[i]!=0){
			int tempx=(xpoint+prel2r[i])/2;
			int tempy=(ypoint+i)/2;
			int tempgd=getgd(pBluImg,tempx,tempy);
			hist[tempgd]++;
		}
	}

	FILE *p;
    if((p=fopen("D:\\一毛花面.txt","wt"))!=NULL)
		for(int i=0;i<255;i++){
		   fprintf(p,"%u",hist[i]);
		   fputs(",", p);
		}
	fclose(p);

//===============================//
//=========标准元直方图==========//
//===============================//
	int hist1fan[255]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,6,0,0,24,8,16,8,23,4,4,20,44,24,80,50,76,60,52,16,68,28,20,16,24,20,16,0,40,20,8,3,0,16,4,16,15,8,4,0,0,0,0,0,0,0,0,0,4,8,0,0,0,5,0,0,0,0,4,0,0,0,0,4,0,4,0,0,0,0,0,0,0,0,0,0,4,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	int hist1zheng[255]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,16,8,0,8,8,8,0,0,0,4,16,0,20,0,16,8,4,8,20,21,22,16,6,24,4,22,6,28,12,22,16,22,0,24,80,44,46,28,48,36,20,38,8,52,4,12,16,16,4,8,16,0,4,8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,0,0,0,8,0,0,0,8,0,0,0,0,0,0,0,0,0,0,4,0,0,4,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	int hist5zheng[255]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,28,8,8,12,24,16,44,52,13,12,18,10,24,6,12,5,2,1,8,24,8,18,4,4,8,16,20,0,16,28,0,8,40,8,8,20,8,20,8,24,12,0,8,8,0,4,0,0,0,0,0,4,0,0,8,0,16,0,0,0,0,8,0,0,0,0,4,4,0,8,0,0,0,8,0,0,0,8,0,0,0,0,0,4,16,0,8,0,0,0,8,0,0,0,4,0,0,0,0,0,8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	int hist5fan[255]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,4,0,0,16,9,18,12,2,4,12,8,32,24,12,17,16,20,12,8,12,36,24,28,16,40,48,44,16,8,14,16,12,4,20,14,18,7,4,0,12,10,8,10,0,0,0,0,0,8,4,0,12,0,4,0,0,8,8,0,0,0,0,0,0,4,0,2,2,0,0,0,8,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,4,0,0,0,8,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	int hist01zheng[255]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,14,13,23,12,18,16,34,28,24,26,32,2,32,20,8,8,4,4,4,0,0,0,8,16,0,4,8,8,10,8,0,32,8,8,16,8,0,0,16,8,0,0,2,0,0,8,0,0,12,8,8,8,16,0,12,8,0,0,0,0,0,8,0,0,0,4,0,2,4,8,10,0,0,0,8,8,0,0,0,0,4,0,0,0,8,0,0,0,4,0,0,0,0,0,16,0,0,0,12,0,0,8,0,8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	int hist01fan[255]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,12,16,16,68,48,28,16,75,20,44,26,26,0,8,0,28,4,2,12,2,8,8,4,0,10,12,2,4,0,4,4,18,16,8,14,2,4,0,4,4,8,0,8,0,0,0,8,0,2,4,0,0,0,4,0,0,0,0,0,0,8,0,0,0,0,0,0,0,0,0,0,8,8,0,0,8,8,0,0,0,0,0,0,0,8,0,4,0,0,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,8,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

	int comparesum1=0;
	int comparesum2=0;
	for(int i=0;i<255;i++){
		comparesum1+=(hist[i]-hist1fan[i])*(hist[i]-hist1fan[i]);
		comparesum2+=(hist[i]-hist1zheng[i])*(hist[i]-hist1zheng[i]);
	}

	if(comparesum1<comparesum2) cout<<"反";
	else cout<<"正";

	delete pGryImg;
	delete pBluImg;
//	delete pRoundImg;
	return;
}

int main(){
	wagepos();
}