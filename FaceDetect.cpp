#include "stdafx.h"
#include "FaceDetect.h"
#include "GrayTrans.h"
#include <math.h>

double median(double **, int, int, int);

FaceDetect::FaceDetect(){
	m_pImgDataOut=NULL;
	m_lpColorTableOut=NULL;	
	m_nColorTableLengthOut=0;
    m_nBitCountOut=0;
	m_imgWidthOut=0;
	m_imgHeightOut=0;
}

FaceDetect::FaceDetect(CSize size, int nBitCount, LPRGBQUAD lpColorTable, 
					unsigned char *pImgData):ImageDib(size, nBitCount, lpColorTable, pImgData){

	m_pImgDataOut=NULL;
	m_lpColorTableOut=NULL;
	m_nColorTableLengthOut=0;
    m_nBitCountOut=0;
	m_imgWidthOut=0;
	m_imgHeightOut=0;
	m_pImgDataIn=pImgData;
	
	// Model parameters
	Cb_Mean=117.4361;
	Cr_Mean=156.5599;
	Cov00=160.1301;		// covariance matrix
	Cov01=12.1430;
	Cov10=12.1430;
	Cov11=299.4574;

}

FaceDetect::~FaceDetect(){

}

void FaceDetect::CalSim(){
	int i,j,k,a1,a2,flag;
	char c_t1[4];

	if(m_nBitCount == 8)
		return;

	// Space clearing
	if(m_pImgDataOut != NULL){
		delete []m_pImgDataOut;
		m_pImgDataOut = NULL;
	}
	if(m_lpColorTableOut != NULL){
		delete []m_lpColorTableOut;
		m_lpColorTableOut = NULL;
	}

	m_nBitCountOut = 8;

	// 
	m_nColorTableLengthOut = ComputeColorTabalLength(m_nBitCountOut);

	//
	if(m_nColorTableLengthOut!=0){
		m_lpColorTableOut = new RGBQUAD[m_nColorTableLengthOut];
		for(i=0; i<m_nColorTableLengthOut; i++){
			m_lpColorTableOut[i].rgbBlue = i;
			m_lpColorTableOut[i].rgbGreen = i;			
			m_lpColorTableOut[i].rgbRed = i;
			m_lpColorTableOut[i].rgbReserved = 0;
		}
	}

	// 3-channel 
	int pixelByteIn = 3;

	//
	int lineByteIn = (m_imgWidth * pixelByteIn + 3) / 4 * 4;
	m_imgHeightOut = m_imgHeight;
	m_imgWidthOut = m_imgWidth;

	//
	int lineByteOut = (m_imgWidth + 3) / 4 * 4;
	m_pImgDataOut = new unsigned char[lineByteOut * m_imgHeight];
	m_pImgDataOut = new unsigned char[lineByteIn*m_imgHeight];

	for(i=0; i<m_imgHeight; i++)
		for(j=0; j<m_imgWidth; j++){
			*(m_pImgDataOut + i * lineByteIn + j) = 255;
		}

		m_pSimArray = new double*[m_imgHeight];

		for(i=0; i<m_imgHeight; i++){
			m_pSimArray[i] = new double[m_imgWidth];
		}

		// YCbCr
		for(i=0; i<m_imgHeight; i++)
			for(j=0; j<m_imgWidth; j++){
				for(k=0; k<3; k++)
					c_t1[k] = *(m_pImgDataIn + i * lineByteIn + j * pixelByteIn + k);
				int C_b = (int)c_t1[0]&255;
				int C_g = (int)c_t1[1]&255;
				int C_r = (int)c_t1[2]&255;

				double Cb=(128-37.797*C_r/255-74.203*C_g/255+112*C_b/255);
				double Cr=(128+112*C_r/255-93.786*C_g/255-18.214*C_b/255);
				double P_CbCr =(Cb-Cb_Mean)*((Cb-Cb_Mean)*Cov11-(Cr-Cr_Mean)*Cov10)+(Cr-Cr_Mean)*(-(Cb-Cb_Mean)*Cov01+(Cr-Cr_Mean)*Cov00);
				P_CbCr = -0.5 * P_CbCr / (Cov00*Cov11-Cov10*Cov01);
				m_pSimArray[i][j] = exp(P_CbCr);
			}
		
		// Filtering
		//CalAveFl(m_pSimArray,m_imgWidth,m_imgHeight,9);
		CalMedFl(m_pSimArray,m_imgWidth,m_imgHeight,9);

		// Statistics for similarity values
		double max = 0.0;

		for(i=0; i<m_imgHeight; i++)
			for(j=0; j<m_imgWidth; j++){
				if(m_pSimArray[i][j] > max)
					max = m_pSimArray[i][j];
			}

		// Normalization and Output
		for(i=0; i<m_imgHeight; i++)
			for(j=0; j<m_imgWidth; j++){
				m_pSimArray[i][j] = m_pSimArray[i][j]/ max;
				*(m_pImgDataOut + i*lineByteOut + j) = (int)(m_pSimArray[i][j]*255);
			}

}

// Median Filter -> Slower, but robust to noises.
void FaceDetect::CalMedFl(double **s, int width, int height, int radius){
	if(radius%2==0)
		return;

	int i,j;
	double **temp;
	temp = new double *[height];
	for(i=0; i<height; i++)
		temp[i] = new double[width];
	for(i=0; i<height; i++)
		for(j=0; j<width; j++)
			temp[i][j] = s[i][j];

	for(i=radius/2+1; i<height-radius/2; i++)
		for(j=radius/2+1; j<width-radius/2; j++){
			temp[i][j] = median(s,i,j,radius);
		}

	for(i=radius/2+1; i<height-radius/2; i++)
		for(j=radius/2+1; j<width-radius/2; j++){
			s[i][j] = temp[i][j];
		}

	for(i=0; i<height; i++)
		delete []temp[i];
	delete []temp;

}

// Average Filter -> Faster, but less precise
void FaceDetect::CalAveFl(double **s, int width, int height, int radius){
	int i,j;
	double **temp;
	temp = new double*[height+2*(int)(radius/2)];
	for(i=0; i<height+2*(int)(radius/2); i++)
		temp[i] = new double[width+2*(int)(radius/2)];
	for(i=0; i<height+2*(int)(radius/2);i++)
		for(j=0; j<width+2*(int)(radius/2); j++)
			temp[i][j] = 0.0;
	for(i=0; i<height; i++)
		for(j=0; j<width; j++)
			temp[i+(int)(radius/2)][j+(int)(radius/2)] = s[i][j];
	for(i=0; i<height; i++)
		for(j=0; j<width; j++){
			s[i][j] = 0;
			for(int r=0; r<radius; r++)
				for(int t=0; t<radius; t++)
					s[i][j] += temp[i+r][j+t];
			s[i][j] /= radius*radius;
		}
	for(i=0; i<height+2*(int)(radius/2); i++)
		delete []temp[i];
	delete []temp;
}

inline double median(double **ss,int index_1, int index_2, int r){
	int k1,k2;
	double res,t;
	double *s_patch = new double[r*r];
	for(k1=0; k1<r; k1++)
		for(k2=0; k2<r; k2++)
			s_patch[k1*r+k2] = ss[index_1-r/2+k1][index_2-r/2+k2];
	
	//Sort
	for(k1=0; k1<r*r; k1++)
		for(k2=1; k2<r*r-k1; k2++){
			if(s_patch[k2] < s_patch[k2-1]){
				t = s_patch[k2];
				s_patch[k2] = s_patch[k2-1];
				s_patch[k2-1] = t;
			}
		}
	res = s_patch[r*r/2+1];	
	delete []s_patch;
	return(res);
}