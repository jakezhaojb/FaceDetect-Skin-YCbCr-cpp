#include "stdafx.h"
#include "ImageDib.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


ImageDib::ImageDib()
{
	m_lpDib=NULL;
	m_lpColorTable=NULL;
	m_pImgData=NULL;
	m_lpBmpInfoHead=NULL;
	m_hPalette = NULL;
}

ImageDib::~ImageDib()
{
	//
	if(m_lpDib != NULL) 
		delete [] m_lpDib; 
	
	//
	if(m_hPalette != NULL)
		::DeleteObject(m_hPalette);
}

ImageDib::ImageDib(CSize size, int nBitCount, LPRGBQUAD lpColorTable,
			   unsigned char *pImgData)
{
	if(pImgData==NULL){
		m_lpDib=NULL;
		m_lpColorTable=NULL;
		m_pImgData=NULL;  
		m_lpBmpInfoHead=NULL;
		m_hPalette = NULL;
	}
	else{

		m_imgWidth=size.cx;
		m_imgHeight=size.cy;
		m_nBitCount=nBitCount;
		
		m_nColorTableLength=ComputeColorTabalLength(nBitCount);
		
		int lineByte=(m_imgWidth*nBitCount/8+3)/4*4;
		
		int imgBufSize=m_imgHeight*lineByte;
		
		m_lpDib=new BYTE [sizeof(BITMAPINFOHEADER) + 
			sizeof(RGBQUAD) * m_nColorTableLength+imgBufSize];
		
		m_lpBmpInfoHead = (LPBITMAPINFOHEADER) m_lpDib;
		m_lpBmpInfoHead->biSize = sizeof(BITMAPINFOHEADER);
		m_lpBmpInfoHead->biWidth = m_imgWidth;
		m_lpBmpInfoHead->biHeight = m_imgHeight;
		m_lpBmpInfoHead->biPlanes = 1;
		m_lpBmpInfoHead->biBitCount = m_nBitCount;
		m_lpBmpInfoHead->biCompression = BI_RGB;
		m_lpBmpInfoHead->biSizeImage = 0;
		m_lpBmpInfoHead->biXPelsPerMeter = 0;
		m_lpBmpInfoHead->biYPelsPerMeter = 0;
		m_lpBmpInfoHead->biClrUsed = m_nColorTableLength;
		m_lpBmpInfoHead->biClrImportant = m_nColorTableLength;
		
		m_hPalette = NULL;

		if(m_nColorTableLength!=0){
			
			m_lpColorTable=(LPRGBQUAD)(m_lpDib+sizeof(BITMAPINFOHEADER));
			
			memcpy(m_lpColorTable,lpColorTable,sizeof(RGBQUAD) * m_nColorTableLength);
			
			MakePalette();
		}
		
		m_pImgData = (LPBYTE)m_lpDib+sizeof(BITMAPINFOHEADER)+
			sizeof(RGBQUAD) * m_nColorTableLength;
		
		memcpy(m_pImgData,pImgData,imgBufSize);
	}
	
}


BOOL ImageDib::Read(LPCTSTR lpszPathName)
{
	CFile file;
	if (!file.Open(lpszPathName, CFile::modeRead | CFile::shareDenyWrite))
		return FALSE;
	BITMAPFILEHEADER bmfh;

	int nCount=file.Read((LPVOID) &bmfh, sizeof(BITMAPFILEHEADER));

	if(m_lpDib!=NULL)	delete []m_lpDib;
	m_lpDib=new BYTE[file.GetLength() -sizeof(BITMAPFILEHEADER)];
	file.Read(m_lpDib, file.GetLength() -sizeof(BITMAPFILEHEADER));

	m_lpBmpInfoHead = (LPBITMAPINFOHEADER)m_lpDib;

	m_imgWidth=m_lpBmpInfoHead->biWidth;
	m_imgHeight=m_lpBmpInfoHead->biHeight;
	m_nBitCount=m_lpBmpInfoHead->biBitCount; 

	m_nColorTableLength=	ComputeColorTabalLength(m_lpBmpInfoHead->biBitCount);

	m_hPalette = NULL;
	if(m_nColorTableLength!=0){m_lpColorTable=
	(LPRGBQUAD)(m_lpDib+sizeof(BITMAPINFOHEADER));
	    MakePalette();
	}

	m_pImgData = (LPBYTE)m_lpDib+sizeof(BITMAPINFOHEADER) +
		sizeof(RGBQUAD) * m_nColorTableLength;
	return TRUE;
}

BOOL ImageDib::Write(LPCTSTR lpszPathName)
{
	CFile file;
	if (!file.Open(lpszPathName, CFile::modeCreate | CFile::modeReadWrite 
		| CFile::shareExclusive))
		return FALSE;
	
	BITMAPFILEHEADER bmfh;
	bmfh.bfType = 0x4d42;  // 'BM'
	bmfh.bfSize = 0;
	bmfh.bfReserved1 = bmfh.bfReserved2 = 0;
	bmfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) +
		sizeof(RGBQUAD) * m_nColorTableLength;	
	try {

		file.Write((LPVOID) &bmfh, sizeof(BITMAPFILEHEADER));
		
		file.Write(m_lpBmpInfoHead,  sizeof(BITMAPINFOHEADER));
		
		if(m_nColorTableLength!=0)
			file.Write(m_lpColorTable, sizeof(RGBQUAD) * m_nColorTableLength);
		
		int imgBufSize=(m_imgWidth*m_nBitCount/8+3)/4*4*m_imgHeight;
		file.Write(m_pImgData, imgBufSize);
	}
	catch(CException* pe) {
		pe->Delete();
		//AfxMessageBox("write error");
		return FALSE;
	}
	
	return TRUE;
}


void ImageDib::MakePalette()
{
	if(m_nColorTableLength == 0) 
		return;

	if(m_hPalette != NULL) ::DeleteObject(m_hPalette);

	LPLOGPALETTE pLogPal = (LPLOGPALETTE) new char[2 * sizeof(WORD) +
		m_nColorTableLength * sizeof(PALETTEENTRY)];
	pLogPal->palVersion = 0x300;
	pLogPal->palNumEntries = m_nColorTableLength;
	LPRGBQUAD m_lpDibQuad = (LPRGBQUAD) m_lpColorTable;
	for(int i = 0; i < m_nColorTableLength; i++) {
		pLogPal->palPalEntry[i].peRed = m_lpDibQuad->rgbRed;
		pLogPal->palPalEntry[i].peGreen = m_lpDibQuad->rgbGreen;
		pLogPal->palPalEntry[i].peBlue = m_lpDibQuad->rgbBlue;
		pLogPal->palPalEntry[i].peFlags = 0;
		m_lpDibQuad++;
	}	

	m_hPalette = ::CreatePalette(pLogPal);

	delete pLogPal;
}

int ImageDib::ComputeColorTabalLength(int nBitCount)
{
	int colorTableLength;
	switch(nBitCount) {
	case 1:
		colorTableLength = 2;
		break;
	case 4:
		colorTableLength = 16;
		break;
	case 8:
		colorTableLength = 256;
		break;
	case 16:
	case 24:
	case 32:
		colorTableLength = 0;
		break;
	default:
		ASSERT(FALSE);
	}
	ASSERT((colorTableLength >= 0) && (colorTableLength <= 256)); 
	return colorTableLength;
}

BOOL ImageDib::Draw(CDC* pDC, CPoint origin, CSize size)
{
	HPALETTE hOldPal=NULL;
	if(m_lpDib == NULL) return FALSE;
	if(m_hPalette != NULL) {
		hOldPal=::SelectPalette(pDC->GetSafeHdc(), m_hPalette, TRUE);
		pDC->RealizePalette();			
	}
	pDC->SetStretchBltMode(COLORONCOLOR);
	::StretchDIBits(pDC->GetSafeHdc(), origin.x, origin.y, size.cx, size.cy,
		0, 0, m_lpBmpInfoHead->biWidth, m_lpBmpInfoHead->biHeight,m_pImgData,
		 (LPBITMAPINFO) m_lpBmpInfoHead, DIB_RGB_COLORS, SRCCOPY);
	if(hOldPal!=NULL)
		::SelectPalette(pDC->GetSafeHdc(), hOldPal, TRUE);
	return TRUE;
}

CSize ImageDib::GetDimensions()
{	
	if(m_lpDib == NULL) return CSize(0, 0);
	return CSize(m_imgWidth, m_imgHeight);
}

void ImageDib::Empty()
{
	if(m_lpDib != NULL) {
		delete [] m_lpDib;
		m_lpDib=NULL;
		m_lpColorTable=NULL;
		m_pImgData=NULL;  
		m_lpBmpInfoHead=NULL;
	}
	if(m_hPalette != NULL){
		::DeleteObject(m_hPalette);
		m_hPalette = NULL;
	}
}


void ImageDib::ReplaceDib(CSize size, int nBitCount,  
						LPRGBQUAD lpColorTable,unsigned char *pImgData)
{ 
	Empty();
	
	m_imgWidth=size.cx;
	m_imgHeight=size.cy;
	m_nBitCount=nBitCount;
	
	m_nColorTableLength=ComputeColorTabalLength(nBitCount);
	
	int lineByte=(m_imgWidth*nBitCount/8+3)/4*4;
	
	int imgBufSize=m_imgHeight*lineByte;
	
	m_lpDib=new BYTE [sizeof(BITMAPINFOHEADER) + 
		sizeof(RGBQUAD) * m_nColorTableLength+imgBufSize];
	
	m_lpBmpInfoHead = (LPBITMAPINFOHEADER) m_lpDib;
	m_lpBmpInfoHead->biSize = sizeof(BITMAPINFOHEADER);
	m_lpBmpInfoHead->biWidth = m_imgWidth;
	m_lpBmpInfoHead->biHeight = m_imgHeight;
	m_lpBmpInfoHead->biPlanes = 1;
	m_lpBmpInfoHead->biBitCount = m_nBitCount;
	m_lpBmpInfoHead->biCompression = BI_RGB;
	m_lpBmpInfoHead->biSizeImage = 0;
	m_lpBmpInfoHead->biXPelsPerMeter = 0;
	m_lpBmpInfoHead->biYPelsPerMeter = 0;
	m_lpBmpInfoHead->biClrUsed = m_nColorTableLength;
	m_lpBmpInfoHead->biClrImportant = m_nColorTableLength;
	
	m_hPalette = NULL;
	
	if(m_nColorTableLength!=0){
		m_lpColorTable=(LPRGBQUAD)(m_lpDib+sizeof(BITMAPINFOHEADER));
		memcpy(m_lpColorTable,lpColorTable,sizeof(RGBQUAD) * m_nColorTableLength);
		MakePalette();
	}
	
	m_pImgData = (LPBYTE)m_lpDib+sizeof(BITMAPINFOHEADER)+
		sizeof(RGBQUAD) * m_nColorTableLength;
	
	memcpy(m_pImgData,pImgData,imgBufSize);
}
