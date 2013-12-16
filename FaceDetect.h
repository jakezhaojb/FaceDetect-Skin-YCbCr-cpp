#ifndef _FACEDETECT_
#define _FACEDETECT_

#include "ImageDib.h"

class FaceDetect : public ImageDib{
public:
	int m_nBitCountOut;
	unsigned char *m_pImgDataOut;
	LPRGBQUAD m_lpColorTableOut;
	unsigned char *m_pImgDataIn;
	double **m_pSimArray;
	double Cb_Mean;
	double Cr_Mean;
	double Cov00;
	double Cov01;
	double Cov10;
	double Cov11;

private:
	int m_imgWidthOut;
	int m_imgHeightOut;
	int m_nColorTableLengthOut;

public:
	void CalSBount(int top, int bottom, int left, int right);
	FaceDetect();
	FaceDetect(CSize size, int nBitCount, LPRGBQUAD lpColorTable, unsigned char *pImgData);
	~FaceDetect();
	void CalMedFl(double **s, int width, int height, int radius);
	void CalAveFl(double **s, int width, int height, int radius);
	void CalSim();
};


#endif
