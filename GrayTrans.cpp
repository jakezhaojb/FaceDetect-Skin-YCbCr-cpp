#include "stdio.h"
#include "stdafx.h"
#include "GrayTrans.h"
#include "math.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

GrayTrans::GrayTrans()
{
	m_pImgDataOut=NULL;
	m_lpColorTableOut=NULL;
	m_nColorTableLengthOut=0;
    m_nBitCountOut=0;
	m_imgWidthOut=0;
	m_imgHeightOut=0;
}

GrayTrans::GrayTrans(CSize size, int nBitCount, LPRGBQUAD lpColorTable, 
					 unsigned char *pImgData):
ImageDib(size, nBitCount, lpColorTable, pImgData)
{

    m_pImgDataOut=NULL;
	m_lpColorTableOut=NULL;
	m_nColorTableLengthOut=0;
    m_nBitCountOut=0;
	m_imgWidthOut=0;
	m_imgHeightOut=0;
}

GrayTrans::~GrayTrans()
{
	if(m_pImgDataOut!=NULL){
		delete []m_pImgDataOut;
    	m_pImgDataOut=NULL;
	}

	if(m_lpColorTableOut!=NULL){
		delete []m_lpColorTableOut;
		m_lpColorTableOut=NULL;
	}
}


CSize GrayTrans::GetDimensions()
{	
	if(m_pImgDataOut == NULL) return CSize(0, 0);
	return CSize(m_imgWidthOut, m_imgHeightOut);
}


void GrayTrans::LinearStrech(CPoint point1,CPoint point2)
{
	if(m_pImgDataOut!=NULL){
		delete []m_pImgDataOut;
		m_pImgDataOut=NULL;
	}
	if(m_lpColorTableOut!=NULL){
		delete []m_lpColorTableOut;
		m_lpColorTableOut=NULL;
	}

	m_nBitCountOut=m_nBitCount;

	m_nColorTableLengthOut=ComputeColorTabalLength(m_nBitCountOut);

	if(m_nColorTableLengthOut!=0){
    	m_lpColorTableOut=new RGBQUAD[m_nColorTableLengthOut];
		memcpy(m_lpColorTableOut,m_lpColorTable,
			sizeof(RGBQUAD)*m_nColorTableLengthOut);
	}

	m_imgWidthOut=m_imgWidth;
	m_imgHeightOut=m_imgHeight;

	int lineByteOut=(m_imgWidth*m_nBitCountOut/8+3)/4*4;
	m_pImgDataOut=new unsigned char[lineByteOut*m_imgHeight];


	if(m_nBitCount==8)
		LinearStrechForSnglChannel(m_pImgData, m_pImgDataOut,
		                             m_imgWidth, m_imgHeight,point1,point2);
	else{

		int lineByteGray=(m_imgWidth+3)/4*4;

		unsigned char *bufIn=new unsigned char[m_imgHeight*lineByteGray];

		unsigned char *bufOut=new unsigned char[m_imgHeight*lineByteGray];

		int i, j;
		int k;

		for(k=0;k<3;k++){
			for(i=0;i<m_imgHeight;i++){
				for(j=0;j<m_imgWidth;j++)
					bufIn[i*lineByteGray+j]=*(m_pImgData+i*lineByteOut+j*3+k);
			}

			LinearStrechForSnglChannel(bufIn,bufOut,m_imgWidth,m_imgHeight,point1,point2);

			for(i=0;i<m_imgHeight;i++){
				for(j=0;j<m_imgWidth;j++)
					*(m_pImgDataOut+i*lineByteOut+j*3+k)=bufOut[i*lineByteGray+j];
			}
		}

		delete []bufIn;
		delete []bufOut;
	}

}


void GrayTrans::LinearStrechForSnglChannel(unsigned char *pImgDataIn, 
		unsigned char *pImgDataOut, int imgWidth,int imgHeight,CPoint point1,CPoint point2)
{
	int i,j;

	int lineByte=(imgWidth+3)/4*4;

	double x1=point1.x,y1=point1.y,x2=point2.x, y2=point2.y;

	double slop1=y1/x1,slop2=(y2-y1)/(x2-x1),slop3=(255-y2)/(255-x2);

	double dis1=0,dis2=y1-slop2*x1,dis3=y2-slop3*x2;

	int Map[256];

	for(i=0;i<256;i++)	
	{
		if(i<x1)
			Map[i]=(int)(slop1*i+dis1+0.5);
		else if(i<x2)
			Map[i]=(int)(slop2*i+dis2+0.5);
		else
			Map[i]=(int)(slop3*i+dis3+0.5);
	}

	int temp;

	for(i=0;i<imgHeight;i++)
	{
		for(j=0;j<lineByte;j++)
		{
			temp=*(pImgDataIn+i*lineByte+j);

			*(pImgDataOut+i*lineByte+j)=Map[temp];
		}
	}
}




void GrayTrans::RevImage()
{
	if(m_pImgDataOut!=NULL){
		delete []m_pImgDataOut;
		m_pImgDataOut=NULL;
	}
	if(m_lpColorTableOut!=NULL){
		delete []m_lpColorTableOut;
		m_lpColorTableOut=NULL;
	}
	
	m_nBitCountOut=m_nBitCount;
	
	m_nColorTableLengthOut=ComputeColorTabalLength(m_nBitCountOut);
	
	if(m_nColorTableLengthOut!=0){
		m_lpColorTableOut=new RGBQUAD[m_nColorTableLengthOut];
		memcpy(m_lpColorTableOut,m_lpColorTable,sizeof(RGBQUAD)*m_nColorTableLengthOut);
	}
	
	m_imgWidthOut=m_imgWidth;
	m_imgHeightOut=m_imgHeight;
	
	int lineByteOut=(m_imgWidth*m_nBitCountOut/8+3)/4*4;
	
	m_pImgDataOut=new unsigned char[lineByteOut*m_imgHeight];
	
	int i,j;
	
	int pixelByte=m_nBitCountOut/8;

	int k;
	
	for(i=0;i<m_imgHeight;i++){
		for(j=0;j<m_imgWidth;j++){
			for(k=0;k<pixelByte;k++)
				*(m_pImgDataOut+i*lineByteOut+j*pixelByte+k)
				=255-*(m_pImgData+i*lineByteOut+j*pixelByte+k);
		}
	}

}


void GrayTrans::ColorToGray()
{

	if(m_nBitCount==8) return;

	if(m_pImgDataOut!=NULL){
		delete []m_pImgDataOut;
		m_pImgDataOut=NULL;
	}
	if(m_lpColorTableOut!=NULL){
		delete []m_lpColorTableOut;
		m_lpColorTableOut=NULL;
	}

	m_nBitCountOut=8;

	m_nColorTableLengthOut=ComputeColorTabalLength(m_nBitCountOut);

	if(m_nColorTableLengthOut!=0){
    	m_lpColorTableOut=new RGBQUAD[m_nColorTableLengthOut];
		for(int i=0; i<m_nColorTableLengthOut;i++){
			m_lpColorTableOut[i].rgbBlue=i;
			m_lpColorTableOut[i].rgbGreen=i;
			m_lpColorTableOut[i].rgbRed=i;
			m_lpColorTableOut[i].rgbReserved=0;
		}
	}

	int pixelByteIn=3;
	
	int lineByteIn=(m_imgWidth*pixelByteIn+3)/4*4;

	m_imgWidthOut=m_imgWidth;
	m_imgHeightOut=m_imgHeight;

	int lineByteOut=(m_imgWidth*m_nBitCountOut/8+3)/4*4;

	m_pImgDataOut=new unsigned char[lineByteOut*m_imgHeight];

	int i,j;

	for(i=0;i<m_imgHeight;i++){
		for(j=0;j<m_imgWidth;j++)
			*(m_pImgDataOut+i*lineByteOut+j)=0.11**(m_pImgData+i*lineByteIn+j*pixelByteIn+0)
		        	+0.59**(m_pImgData+i*lineByteIn+j*pixelByteIn+1)
		        	+0.30**(m_pImgData+i*lineByteIn+j*pixelByteIn+2)+0.5;
	}

}


void GrayTrans::BinaryImage(int threshold)
{
	if(m_nBitCount==8){
		if(m_pImgDataOut!=NULL){
			delete []m_pImgDataOut;
			m_pImgDataOut=NULL;
		}
		if(m_lpColorTableOut!=NULL){
			delete []m_lpColorTableOut;
			m_lpColorTableOut=NULL;
		}

		m_nBitCountOut=m_nBitCount;
		m_nColorTableLengthOut=ComputeColorTabalLength(m_nBitCountOut);
		
		m_lpColorTableOut=new RGBQUAD[m_nColorTableLengthOut];
		memcpy(m_lpColorTableOut,m_lpColorTable,sizeof(RGBQUAD)*m_nColorTableLengthOut);
		
		m_imgWidthOut=m_imgWidth;
		m_imgHeightOut=m_imgHeight;
		
		
		int lineByte=(m_imgWidth*m_nBitCount/8+3)/4*4;
		
		m_pImgDataOut=new unsigned char[lineByte*m_imgHeight];
		
		int i,j;
		
		for(i=0;i<m_imgHeight;i++){
			for(j=0;j<m_imgWidth;j++){
				if(*(m_pImgData+i*lineByte+j)<threshold)
					*(m_pImgDataOut+i*lineByte+j)=0;
				else
					*(m_pImgDataOut+i*lineByte+j)=255;
			}
		}
	}
	else
	{
		if(m_pImgDataOut!=NULL){
			delete []m_pImgDataOut;
			m_pImgDataOut=NULL;
		}
		if(m_lpColorTableOut!=NULL){
			delete []m_lpColorTableOut;
			m_lpColorTableOut=NULL;
		}

		m_nBitCountOut=8;

		m_nColorTableLengthOut=ComputeColorTabalLength(m_nBitCountOut);

		if(m_nColorTableLengthOut!=0){
			m_lpColorTableOut=new RGBQUAD[m_nColorTableLengthOut];
			for(int i=0; i<m_nColorTableLengthOut;i++){
				m_lpColorTableOut[i].rgbBlue=i;
				m_lpColorTableOut[i].rgbGreen=i;
				m_lpColorTableOut[i].rgbRed=i;
				m_lpColorTableOut[i].rgbReserved=0;
			}
		}
		
		int pixelByteIn=3;

		int lineByteIn=(m_imgWidth*pixelByteIn+3)/4*4;

		m_imgWidthOut=m_imgWidth;
		m_imgHeightOut=m_imgHeight;
		
		int lineByteOut=(m_imgWidth*m_nBitCountOut/8+3)/4*4;
		
		m_pImgDataOut=new unsigned char[lineByteOut*m_imgHeight];
		
		int m,n;
		
		for(m=0;m<m_imgHeight;m++){
			for(n=0;n<m_imgWidth;n++)
				*(m_pImgDataOut+m*lineByteOut+n)=0.11**(m_pImgData+m*lineByteIn+n*pixelByteIn+0)
				+0.59**(m_pImgData+m*lineByteIn+n*pixelByteIn+1)
				+0.30**(m_pImgData+m*lineByteIn+n*pixelByteIn+2)+0.5;
	}
		int i,j;

		for(i=0;i<m_imgHeight;i++){
			for(j=0;j<m_imgWidth;j++)
				if(*(m_pImgDataOut+i*lineByteOut+j)<threshold)
					*(m_pImgDataOut+i*lineByteOut+j)=0;
				else
					*(m_pImgDataOut+i*lineByteOut+j)=255;
		}
	}
	
}



void GrayTrans::ThresholdWindow(int bBottom, int bTop )
{
	if(m_pImgDataOut!=NULL){
		delete []m_pImgDataOut;
		m_pImgDataOut=NULL;
	}
	if(m_lpColorTableOut!=NULL){
		delete []m_lpColorTableOut;
		m_lpColorTableOut=NULL;
	}

	m_nBitCountOut=m_nBitCount;
	m_nColorTableLengthOut=ComputeColorTabalLength(m_nBitCountOut);
	
	m_lpColorTableOut=new RGBQUAD[m_nColorTableLengthOut];
	memcpy(m_lpColorTableOut,m_lpColorTable,sizeof(RGBQUAD)*m_nColorTableLengthOut);
	
	m_imgWidthOut=m_imgWidth;
	m_imgHeightOut=m_imgHeight;

	int lineByte=(m_imgWidthOut*m_nBitCountOut/8+3)/4*4;
	
	m_pImgDataOut=new unsigned char[lineByte*m_imgHeightOut];
	
	int pixelByte=m_nBitCountOut/8;

	int i,j,k;

	for(i=0;i<m_imgHeightOut ;i++){
		
		for(j=0;j<m_imgWidthOut;j++){
			for(k=0;k<pixelByte;k++){
				*(m_pImgDataOut+i*lineByte+j*pixelByte+k)=*(m_pImgData+i*lineByte+j*pixelByte+k);
		
			if(*(m_pImgData+i*lineByte+j*pixelByte+k)<bBottom)
				*(m_pImgDataOut+i*lineByte+j*pixelByte+k)=0;
			else if (*(m_pImgData+i*lineByte+j*pixelByte+k)>bTop) 
				*(m_pImgDataOut+i*lineByte+j*pixelByte+k)=255;
			}
		
		}
	}

}
