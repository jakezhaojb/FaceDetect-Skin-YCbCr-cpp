#ifndef _GRAYTRANS_
#define _GRAYTRANS_

#include "ImageDib.h"

class GrayTrans : public ImageDib  
{
public:
	//
	int m_nBitCountOut;
	
	//
	unsigned char * m_pImgDataOut;
	
	//
	LPRGBQUAD m_lpColorTableOut;

	//
	int m_imgWidthOut;
	
	//
	int m_imgHeightOut;
	
	//
	int m_nColorTableLengthOut;
	
public:
	//
	GrayTrans();
	
	//
	GrayTrans(CSize size, int nBitCount, LPRGBQUAD lpColorTable, 
		unsigned char *pImgData);
	
	//
	~GrayTrans();
	
	//
	CSize GetDimensions();
	
	//
	void ColorToGray();

	//
	void BinaryImage(int threshold=128);

	//
	void RevImage();

    //
	void ThresholdWindow(int bTop, int bBottom);
	
	//
	void LinearStrech(CPoint point1,CPoint point2);
	
private:
	void LinearStrechForSnglChannel(unsigned char *pImgDataIn,
		unsigned char *pImgDataOut,int imgWidth,int imgHeight,
		CPoint point1,CPoint point2);

};

#endif
