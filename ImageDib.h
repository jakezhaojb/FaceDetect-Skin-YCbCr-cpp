#ifndef _IMAGEDIB_
#define _IMAGEDIB_

class ImageDib  
{
	public:	
		unsigned char * m_pImgData; 	
		LPRGBQUAD m_lpColorTable; 		
		int m_nBitCount;			
	private:
		LPBYTE m_lpDib;	
		HPALETTE m_hPalette;	
		int m_nColorTableLength; 		
	public:
		int m_imgWidth;							
		int m_imgHeight; 						
		LPBITMAPINFOHEADER m_lpBmpInfoHead; 	
	
	public:
		ImageDib();
		ImageDib(CSize size, int nBitCount, LPRGBQUAD lpColorTable, 
		unsigned char *pImgData);
		~ImageDib();
		BOOL Read(LPCTSTR lpszPathName);
		BOOL Write(LPCTSTR lpszPathName);
		int ComputeColorTabalLength(int nBitCount);
		BOOL Draw(CDC* pDC, CPoint origin, CSize size);
		CSize GetDimensions();
		void ReplaceDib(CSize size, int nBitCount, LPRGBQUAD lpColorTable,
		unsigned char *pImgData);
	private:
		void MakePalette();
		void Empty();

};

#endif
