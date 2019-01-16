// cdib.cpp
// new version for WIN32

// Notes
///////////////////////////////////////////////////////////////////////////////////////////
// FillIn() changed
// Commented out changes made 9/2/99 so function can be recalled
// Added GetColorTable() 9/20/99 for Fractals not in lab version yet
// changed 9/12/04 for param movie - added SetBytes()

#include "stdafx.h"

#define NOMINMAX

#include <stdlib.h>
#include <direct.h>
#include <stdexcept>
#include "cdib.h"
#include <algorithm>
//#include "SerializationException.h"

using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_SERIAL(CDib, CObject, 0);

CDib::CDib()
{
	m_hFile = NULL;
	m_hBitmap = NULL;
	m_hPalette = NULL;
	m_BitCount = 0;
	m_RealWidth = 0;
	m_nBmihAlloc = m_nImageAlloc = noAlloc;
	Empty();
}

// user must delete the copy
// this was implemented for backing up the CFractalsDoc Dib when 
// the user changed the colors
shared_ptr<CDib> CDib::Copy()
{
	shared_ptr<CDib> pdib(new CDib());

	if(IsWidth())
		pdib->Copy(GetDimensions(),GetBitCount(),(RGBQUAD*)GetColorTable(GetBitCount()),GetBytes());

	return pdib;
}

shared_ptr<CDib> CDib::FlipHorizontal()
{
	shared_ptr<CDib> pdib(new CDib());

	if (IsWidth())
	{
		pdib->Copy(GetDimensions(),GetBitCount(),(RGBQUAD*)GetColorTable(GetBitCount()),GetBytes());

		// Calculate min and max values
		CSize size = GetDimensions();
		int bitwidth = static_cast<int>(GetBitWidth());
		LPBYTE pOrigBytes = GetBytes();
		LPBYTE pNewBytes = pdib->GetBytes();

		if (GetBitCount() == 8)
		{
			for ( int H = 0; H < size.cy; H++)
			{
				for (int W = 0; W < size.cx; W++)
				{
					*(pNewBytes + (size.cy-(H+1))*bitwidth + W) = *(pOrigBytes + H*bitwidth + W);
				}
			}
		}
		else if (GetBitCount() == 24)
		{
			for ( int H = 0; H < size.cy; H++)
			{
				for (int W = 0; W < size.cx; W++)
				{
					*(pNewBytes + (size.cy-(H+1))*bitwidth + 3*W) = *(pOrigBytes + H*bitwidth + 3*W);
					*(pNewBytes + (size.cy-(H+1))*bitwidth + 3*W + 1) = *(pOrigBytes + H*bitwidth + 3*W + 1);
					*(pNewBytes + (size.cy-(H+1))*bitwidth + 3*W + 2) = *(pOrigBytes + H*bitwidth + 3*W + 2);
				}
			}
		}
		else
			throw std::exception("Transformation only supported for bitmaps with bit counts of 8 and 24 are supported!");
	}

	return pdib;
}

shared_ptr<CDib> CDib::FlipVertical()
{
	shared_ptr<CDib> pdib(new CDib());

	if (IsWidth())
	{
		pdib->Copy(GetDimensions(),GetBitCount(),(RGBQUAD*)GetColorTable(GetBitCount()),GetBytes());

		// Calculate min and max values
		CSize size = GetDimensions();
		int bitwidth = static_cast<int>(GetBitWidth());
		LPBYTE pOrigBytes = GetBytes();
		LPBYTE pNewBytes = pdib->GetBytes();

		if (GetBitCount() == 8)
		{
			for ( int H = 0; H < size.cy; H++)
			{
				for (int W = 0; W < size.cx; W++)
				{
					*(pNewBytes + H*bitwidth + size.cx - (W+1)) = *(pOrigBytes + H*bitwidth + W);
				}
			}
		}
		else if (GetBitCount() == 24)
		{
			for ( int H = 0; H < size.cy; H++)
			{
				for (int W = 0; W < size.cx; W++)
				{
					*(pNewBytes + H*bitwidth + 3*(size.cx - W - 1)) = *(pOrigBytes + H*bitwidth + 3*W);
					*(pNewBytes + H*bitwidth + 3*(size.cx - W - 1) + 1) = *(pOrigBytes + H*bitwidth + 3*W + 1);
					*(pNewBytes + H*bitwidth + 3*(size.cx - W - 1) + 2) = *(pOrigBytes + H*bitwidth + 3*W + 2);
				}
			}
		}
		else
			throw std::exception("Transformation only supported for bitmaps with bit counts of 8 and 24 are supported!");
	}

	return pdib;
}

shared_ptr<CDib> CDib::Rotate90Degrees()
{
// use fill in and the check bits to determine how to copy
	shared_ptr<CDib> pdib(new CDib());

	if (IsWidth())
	{
		CSize size = GetDimensions();
		CSize newSize(size.cy, size.cx);
		int bitwidth = static_cast<int>(GetBitWidth());
		LPBYTE pOrigBytes = GetBytes();
		LPBYTE pNewBytes = pdib->FillIn(newSize, GetBitCount(), (RGBQUAD*)GetColorTable(GetBitCount()));
		int newBitwidth = static_cast<int>(pdib->GetBitWidth());

		if (GetBitCount() == 8)
		{
			for ( int H = 0; H < size.cy; H++)
			{
				for (int W = 0; W < size.cx; W++)
				{
					*(pNewBytes + (size.cx-(W+1))*newBitwidth + H) = *(pOrigBytes + H*bitwidth + W);
				}
			}
		}
		else if (GetBitCount() == 24)
		{
			for ( int H = 0; H < size.cy; H++)
			{
				for (int W = 0; W < size.cx; W++)
				{
					*(pNewBytes + (size.cx-(W+1))*newBitwidth + 3*H) = *(pOrigBytes + H*bitwidth + 3*W);
					*(pNewBytes + (size.cx-(W+1))*newBitwidth + 3*H + 1) = *(pOrigBytes + H*bitwidth + 3*W + 1);
					*(pNewBytes + (size.cx-(W+1))*newBitwidth + 3*H + 2) = *(pOrigBytes + H*bitwidth + 3*W + 2);
				}
			}
		}
		else
			throw std::exception("Transformation only supported for bitmaps with bit counts of 8 and 24 are supported!");
	}

	return pdib;
}

shared_ptr<CDib> CDib::Rotate180Degrees()
{
	shared_ptr<CDib> pdib(new CDib());

	if (IsWidth())
	{
		pdib->Copy(GetDimensions(),GetBitCount(),(RGBQUAD*)GetColorTable(GetBitCount()),GetBytes());

		// Calculate min and max values
		CSize size = GetDimensions();
		int bitwidth = static_cast<int>(GetBitWidth());
		LPBYTE pOrigBytes = GetBytes();
		LPBYTE pNewBytes = pdib->GetBytes();

		if (GetBitCount() == 8)
		{
			for ( int H = 0; H < size.cy; H++)
			{
				for (int W = 0; W < size.cx; W++)
				{
					*(pNewBytes + (size.cy-(H+1))*bitwidth + size.cx - (W+1)) = *(pOrigBytes + H*bitwidth + W);
				}
			}
		}
		else if (GetBitCount() == 24)
		{
			for ( int H = 0; H < size.cy; H++)
			{
				for (int W = 0; W < size.cx; W++)
				{
					*(pNewBytes + (size.cy-(H+1))*bitwidth + 3*(size.cx - W - 1)) = *(pOrigBytes + H*bitwidth + 3*W);
					*(pNewBytes + (size.cy-(H+1))*bitwidth + 3*(size.cx - W - 1) + 1) = *(pOrigBytes + H*bitwidth + 3*W + 1);
					*(pNewBytes + (size.cy-(H+1))*bitwidth + 3*(size.cx - W - 1) + 2) = *(pOrigBytes + H*bitwidth + 3*W + 2);
				}
			}
		}
		else
			throw std::exception("Transformation only supported for bitmaps with bit counts of 8 and 24 are supported!");
	}

	return pdib;
}

shared_ptr<CDib> CDib::Rotate270Degrees()
{
	shared_ptr<CDib> pdib(new CDib());

	if (IsWidth())
	{
		CSize size = GetDimensions();
		CSize newSize(size.cy, size.cx);
		int bitwidth = static_cast<int>(GetBitWidth());
		LPBYTE pOrigBytes = GetBytes();
		LPBYTE pNewBytes = pdib->FillIn(newSize, GetBitCount(), (RGBQUAD*)GetColorTable(GetBitCount()));
		int newBitwidth = static_cast<int>(pdib->GetBitWidth());

		if (GetBitCount() == 8)
		{
			for ( int H = 0; H < size.cy; H++)
			{
				for (int W = 0; W < size.cx; W++)
				{
					*(pNewBytes + W*newBitwidth + (size.cy-(H+1))) = *(pOrigBytes + H*bitwidth + W);
				}
			}
		}
		else if (GetBitCount() == 24)
		{
			for ( int H = 0; H < size.cy; H++)
			{
				for (int W = 0; W < size.cx; W++)
				{
					*(pNewBytes + W*newBitwidth + 3*(size.cy-(H+1))) = *(pOrigBytes + H*bitwidth + 3*W);
					*(pNewBytes + W*newBitwidth + 3*(size.cy-(H+1)) + 1) = *(pOrigBytes + H*bitwidth + 3*W + 1);
					*(pNewBytes + W*newBitwidth + 3*(size.cy-(H+1)) + 2) = *(pOrigBytes + H*bitwidth + 3*W + 2);
				}
			}
		}
		else
			throw std::exception("Transformation only supported for bitmaps with bit counts of 8 and 24 are supported!");
	}

	return pdib;
}

// copy parameters (which came from another dib) to this dib
// called by CDib* Copy()
// Very similar to FillIn() except new memory allocated for image bits
// and all parameters are needed
void CDib::Copy(CSize size, int nBitCount, RGBQUAD* pColorTable, LPBYTE pBytes )
{
	Empty();
	ComputePaletteSize(nBitCount);
	m_lpBMIH = (LPBITMAPINFOHEADER) new 
		char[sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * m_nColorTableEntries];
	m_nBmihAlloc = crtAlloc;
	m_lpBMIH->biSize = sizeof(BITMAPINFOHEADER);
	m_lpBMIH->biWidth = size.cx;
	m_lpBMIH->biHeight = size.cy;
	m_lpBMIH->biPlanes = 1;
	m_lpBMIH->biBitCount = nBitCount;
	m_lpBMIH->biCompression = BI_RGB;
	m_lpBMIH->biSizeImage = 0;
	m_lpBMIH->biXPelsPerMeter = 0;
	m_lpBMIH->biYPelsPerMeter = 0;
	m_lpBMIH->biClrUsed = m_nColorTableEntries;
	m_lpBMIH->biClrImportant = m_nColorTableEntries;
	ComputeMetrics();

	// Set color table
	if(m_lpBMIH->biBitCount == 8 && pColorTable != NULL)
	{
		// make sure lpvcolortable has been allocated first!
		 memcpy(m_lpvColorTable,pColorTable,256*sizeof(RGBQUAD));

	}
	else
		memset(m_lpvColorTable, 0, sizeof(RGBQUAD) * m_nColorTableEntries);

	// Assign image memory
	if(pBytes == NULL)
	{
		m_lpImage = new BYTE[m_dwSizeImage];
		m_nImageAlloc = crtAlloc;
	}
	else	// Use pointer that was passed 
	{		// here's the difference with FillIn()
		m_lpImage = new BYTE[m_dwSizeImage];
		m_nImageAlloc = crtAlloc;
		memcpy(m_lpImage,pBytes,m_dwSizeImage);
	}
	m_BitCount = nBitCount;
}

// DIB section constructor 
// Parameters determine DIB size and number of colors
// Allocates info header memory but not image memory
CDib::CDib(CSize size, int nBitCount)
{
	m_hFile = NULL;
	m_hBitmap = NULL;
	m_hPalette = NULL;
	m_RealWidth = 0;
	m_nBmihAlloc = m_nImageAlloc = noAlloc;
	Empty();
	ComputePaletteSize(nBitCount);
	m_lpBMIH = (LPBITMAPINFOHEADER) new 
		char[sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * m_nColorTableEntries];
	m_nBmihAlloc = crtAlloc;
	m_lpBMIH->biSize = sizeof(BITMAPINFOHEADER);
	m_lpBMIH->biWidth = size.cx;
	m_lpBMIH->biHeight = size.cy;
	m_lpBMIH->biPlanes = 1;
	m_lpBMIH->biBitCount = nBitCount;
	m_lpBMIH->biCompression = BI_RGB;
	m_lpBMIH->biSizeImage = 0;
	m_lpBMIH->biXPelsPerMeter = 0;
	m_lpBMIH->biYPelsPerMeter = 0;
	m_lpBMIH->biClrUsed = m_nColorTableEntries;
	m_lpBMIH->biClrImportant = m_nColorTableEntries;
	m_BitCount = nBitCount;
	ComputeMetrics();
	memset(m_lpvColorTable, 0, sizeof(RGBQUAD) * m_nColorTableEntries);
	m_lpImage = NULL;  // no data yet
}

CDib::~CDib()
{
	Empty();
}

CSize CDib::GetDimensions()
{	
	if(m_lpBMIH == NULL) return CSize(0, 0);
	return CSize((int) m_lpBMIH->biWidth, (int) m_lpBMIH->biHeight);
}

BOOL CDib::AttachMapFile(const wchar_t* strPathname, BOOL bShare) // for reading
{
	// if we open the same file twice, Windows treats it as 2 separate files
	// doesn't work with rare BMP files where # palette entries > biClrUsed
	HANDLE hFile = ::CreateFile(strPathname, GENERIC_WRITE | GENERIC_READ,
		bShare ? FILE_SHARE_READ : 0,
		NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	ASSERT(hFile != INVALID_HANDLE_VALUE);
	DWORD dwFileSize = ::GetFileSize(hFile, NULL);
	HANDLE hMap = ::CreateFileMapping(hFile, NULL, PAGE_READWRITE, 0, 0, NULL);
	DWORD dwErr = ::GetLastError();
	if(hMap == NULL) {
		AfxMessageBox(_T("Empty bitmap file"));
		return FALSE;
	}
	LPVOID lpvFile = ::MapViewOfFile(hMap, FILE_MAP_WRITE, 0, 0, 0); // map whole file
	ASSERT(lpvFile != NULL);
	if(((LPBITMAPFILEHEADER) lpvFile)->bfType != 0x4d42) {
		AfxMessageBox(_T("Invalid bitmap file"));
		DetachMapFile();
		return FALSE;
	}
	AttachMemory((LPBYTE) lpvFile + sizeof(BITMAPFILEHEADER));
	m_lpvFile = lpvFile;
	m_hFile = hFile;
	m_hMap = hMap;
	return TRUE;
}

BOOL CDib::CopyToMapFile(wchar_t* strPathname)
{
	// copies DIB to a new file, releases prior pointers
	// if you previously used CreateSection, the HBITMAP will be NULL (and unusable)
	BITMAPFILEHEADER bmfh;
	bmfh.bfType = 0x4d42;  // 'BM'
	bmfh.bfSize = m_dwSizeImage + sizeof(BITMAPINFOHEADER) +
			sizeof(RGBQUAD) * m_nColorTableEntries + sizeof(BITMAPFILEHEADER);
	// meaning of bfSize open to interpretation
	bmfh.bfReserved1 = bmfh.bfReserved2 = 0;
	bmfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) +
			sizeof(RGBQUAD) * m_nColorTableEntries;	
	// SJR added this to create any nonexisting directories
	MakeDir(strPathname);

	HANDLE hFile = ::CreateFile(strPathname, GENERIC_WRITE | GENERIC_READ, 0, NULL,
		CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hFile == INVALID_HANDLE_VALUE)
		return FALSE;
	ASSERT(hFile != INVALID_HANDLE_VALUE);
	int nSize =  sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) +
				sizeof(RGBQUAD) * m_nColorTableEntries +  m_dwSizeImage;
	HANDLE hMap = ::CreateFileMapping(hFile, NULL, PAGE_READWRITE, 0, nSize, NULL);
	DWORD dwErr = ::GetLastError();
	ASSERT(hMap != NULL);
	LPVOID lpvFile = ::MapViewOfFile(hMap, FILE_MAP_WRITE, 0, 0, 0); // map whole file
	ASSERT(lpvFile != NULL);
	LPBYTE lpbCurrent = (LPBYTE) lpvFile;
	memcpy(lpbCurrent, &bmfh, sizeof(BITMAPFILEHEADER)); // file header
	lpbCurrent += sizeof(BITMAPFILEHEADER);
	LPBITMAPINFOHEADER lpBMIH = (LPBITMAPINFOHEADER) lpbCurrent;
	memcpy(lpbCurrent, m_lpBMIH,
		sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * m_nColorTableEntries); // info
	lpbCurrent += sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * m_nColorTableEntries;
	memcpy(lpbCurrent, m_lpImage, m_dwSizeImage); // bit image
	DWORD dwSizeImage = m_dwSizeImage;
	Empty();
	m_dwSizeImage = dwSizeImage;
	m_nBmihAlloc = m_nImageAlloc = noAlloc;
	m_lpBMIH = lpBMIH;
	m_lpImage = lpbCurrent;
	m_hFile = hFile;
	m_hMap = hMap;
	m_lpvFile = lpvFile;
	ComputePaletteSize(m_lpBMIH->biBitCount);
	ComputeMetrics();
	MakePalette();
	return TRUE;
}

BOOL CDib::AttachMemory(LPVOID lpvMem, BOOL bMustDelete, HGLOBAL hGlobal)
{
	// assumes contiguous BITMAPINFOHEADER, color table, image
	// color table could be zero length
	Empty();
	m_hGlobal = hGlobal;
	if(bMustDelete == FALSE) {
		m_nBmihAlloc = noAlloc;
	}
	else {
		m_nBmihAlloc = ((hGlobal == NULL) ? crtAlloc : heapAlloc);
	}
	try {
		m_lpBMIH = (LPBITMAPINFOHEADER) lpvMem;
		ComputeMetrics();
		ComputePaletteSize(m_lpBMIH->biBitCount);
		m_lpImage = (LPBYTE) m_lpvColorTable + sizeof(RGBQUAD) * m_nColorTableEntries;
		MakePalette();
	}
	catch(CException* pe) {
		AfxMessageBox(_T("AttachMemory error"));
		pe->Delete();
		return FALSE;
	}
	return TRUE;
}

UINT CDib::UsePalette(CDC* pDC, BOOL bBackground /* = FALSE */)
{
	if(m_hPalette == NULL) return 0;
	HDC hdc = pDC->GetSafeHdc();
	::SelectPalette(hdc, m_hPalette, bBackground);
	return ::RealizePalette(hdc);
}

BOOL CDib::Draw(CDC* pDC, CPoint origin, CSize size)
{
	if(m_lpBMIH == NULL) return FALSE;
	if(m_hPalette != NULL) {
		::SelectPalette(pDC->GetSafeHdc(), m_hPalette, TRUE);
	}
	pDC->SetStretchBltMode(COLORONCOLOR);
	::StretchDIBits(pDC->GetSafeHdc(), origin.x, origin.y, size.cx, size.cy,
		0, 0, m_lpBMIH->biWidth, m_lpBMIH->biHeight,
		m_lpImage, (LPBITMAPINFO) m_lpBMIH, DIB_RGB_COLORS, SRCCOPY);
	return TRUE;
}

BOOL CDib::DrawBetter(CDC* pDC, CPoint origin, CSize size)
{
	if(m_lpBMIH == NULL) return FALSE;
	if(m_hPalette != NULL) {
		::SelectPalette(pDC->GetSafeHdc(), m_hPalette, TRUE);
	}
	pDC->SetStretchBltMode(STRETCH_HALFTONE);
	::StretchDIBits(pDC->GetSafeHdc(), origin.x, origin.y, size.cx, size.cy,
		0, 0, m_lpBMIH->biWidth, m_lpBMIH->biHeight,
		m_lpImage, (LPBITMAPINFO) m_lpBMIH, DIB_RGB_COLORS, SRCCOPY);

	::SetBrushOrgEx(pDC->GetSafeHdc(),0,0,NULL);

	return TRUE;
}

// Draws the part of the bitmap in the range provided by the src coordinates
// The source coordinates must be within the bitmaps size
BOOL CDib::DrawPartWidth(CDC* pDC, CPoint destTopLeft, CSize destSize, int srcLeft, int srcRight)
{
	if(m_lpBMIH == NULL || srcLeft == srcRight) 
		return FALSE;

	if(m_hPalette != NULL) 
	{
		::SelectPalette(pDC->GetSafeHdc(), m_hPalette, TRUE);
	}
	srcLeft = std::max<int>( 0, srcLeft );
	srcRight = std::min<int>( srcRight, static_cast<int>(m_lpBMIH->biWidth) );
	if( srcLeft > srcRight )
		std::swap( srcLeft, srcRight );

	pDC->SetStretchBltMode(STRETCH_HALFTONE);
	::StretchDIBits(pDC->GetSafeHdc(), destTopLeft.x, destTopLeft.y, destSize.cx, destSize.cy,
		srcLeft, 0, srcRight-srcLeft, m_lpBMIH->biHeight,
		m_lpImage, (LPBITMAPINFO) m_lpBMIH, DIB_RGB_COLORS, SRCCOPY);

	::SetBrushOrgEx(pDC->GetSafeHdc(),0,0,NULL);

	return TRUE;
}

BOOL CDib::DrawPart(CDC* pDC, CPoint destTopLeft, CSize destSize, int srcLeft, int srcTop, int srcRight, int srcBottom)
{
	if(m_lpBMIH == NULL)
		return FALSE;

	if(m_hPalette != NULL) 
	{
		::SelectPalette(pDC->GetSafeHdc(), m_hPalette, TRUE);
	}
	srcLeft = std::max<int>( 0, srcLeft );
	srcRight = std::min<int>( srcRight, static_cast<int>(m_lpBMIH->biWidth) );
	if( srcLeft > srcRight )
		std::swap( srcLeft, srcRight );
	srcTop = std::max<int>( 0, srcTop );
	srcBottom = std::min<int>( srcBottom, static_cast<int>(m_lpBMIH->biHeight) );
	if( srcTop > srcBottom )
		std::swap( srcTop, srcBottom );

	if(srcLeft == srcRight || srcTop == srcBottom) 
		return FALSE;

	pDC->SetStretchBltMode(STRETCH_HALFTONE);
	::StretchDIBits(pDC->GetSafeHdc(), destTopLeft.x, destTopLeft.y, destSize.cx, destSize.cy,
		srcLeft, srcTop, srcRight-srcLeft, srcBottom-srcTop,
		m_lpImage, (LPBITMAPINFO) m_lpBMIH, DIB_RGB_COLORS, SRCCOPY);

	::SetBrushOrgEx(pDC->GetSafeHdc(),0,0,NULL);

	return TRUE;
}

// BAD DOES NOT WORK LIKE THIS 9/3/99
BOOL CDib::DrawDirect(CDC* pDC, CPoint origin, CSize size)
{
	if(m_lpBMIH == NULL) return FALSE;
	if(m_hPalette != NULL) {
		::SelectPalette(pDC->GetSafeHdc(), m_hPalette, TRUE);
	}
	::SetDIBitsToDevice(pDC->GetSafeHdc(), origin.x, origin.y, size.cx, size.cy,
		0, -size.cy, 0, m_lpBMIH->biHeight,
		m_lpImage, (LPBITMAPINFO) m_lpBMIH, DIB_RGB_COLORS);
	return TRUE;
}

HBITMAP CDib::CreateSection(CDC* pDC /* = NULL */)
{
	if(m_lpBMIH == NULL) return NULL;
	if(m_lpImage != NULL) return NULL; // can only do this if image doesn't exist
	m_hBitmap = ::CreateDIBSection(pDC->GetSafeHdc(), (LPBITMAPINFO) m_lpBMIH,
		DIB_RGB_COLORS,	(LPVOID*) &m_lpImage, NULL, 0);
	ASSERT(m_lpImage != NULL);
	return m_hBitmap;
}

BOOL CDib::MakePalette()
{
	// makes a logical palette (m_hPalette) from the DIB's color table
	// this palette will be selected and realized prior to drawing the DIB
	if(m_nColorTableEntries == 0) return FALSE;
	if(m_hPalette != NULL) ::DeleteObject(m_hPalette);
	TRACE("CDib::MakePalette -- m_nColorTableEntries = %d\n", m_nColorTableEntries);
	LPLOGPALETTE pLogPal = (LPLOGPALETTE) new char[2 * sizeof(WORD) +
		m_nColorTableEntries * sizeof(PALETTEENTRY)];
	pLogPal->palVersion = 0x300;
	pLogPal->palNumEntries = m_nColorTableEntries;
	LPRGBQUAD pDibQuad = (LPRGBQUAD) m_lpvColorTable;
	for(int i = 0; i < m_nColorTableEntries; i++) {
		pLogPal->palPalEntry[i].peRed = pDibQuad->rgbRed;
		pLogPal->palPalEntry[i].peGreen = pDibQuad->rgbGreen;
		pLogPal->palPalEntry[i].peBlue = pDibQuad->rgbBlue;
		pLogPal->palPalEntry[i].peFlags = 0;
		pDibQuad++;
	}
	m_hPalette = ::CreatePalette(pLogPal);
	delete pLogPal;
	return TRUE;
}	

BOOL CDib::SetSystemPalette(CDC* pDC)
{
	// if the DIB doesn't have a color table, we can use the system's halftone palette
	if(m_nColorTableEntries != 0) return FALSE;
	m_hPalette = ::CreateHalftonePalette(pDC->GetSafeHdc());
	return TRUE;
}

HBITMAP CDib::CreateBitmap(CDC* pDC)
{
    if (m_dwSizeImage == 0) return NULL;
    HBITMAP hBitmap = ::CreateDIBitmap(pDC->GetSafeHdc(), m_lpBMIH,
            CBM_INIT, m_lpImage, (LPBITMAPINFO) m_lpBMIH, DIB_RGB_COLORS);
    ASSERT(hBitmap != NULL);
    return hBitmap;
}

BOOL CDib::Compress(CDC* pDC, BOOL bCompress /* = TRUE */)
{
	// 1. makes GDI bitmap from existing DIB
	// 2. makes a new DIB from GDI bitmap with compression
	// 3. cleans up the original DIB
	// 4. puts the new DIB in the object
	if((m_lpBMIH->biBitCount != 4) && (m_lpBMIH->biBitCount != 8)) return FALSE;
		// compression supported only for 4 bpp and 8 bpp DIBs
	if(m_hBitmap) return FALSE; // can't compress a DIB Section!
	TRACE("Compress: original palette size = %d\n", m_nColorTableEntries); 
	HDC hdc = pDC->GetSafeHdc();
	HPALETTE hOldPalette = ::SelectPalette(hdc, m_hPalette, FALSE);
	HBITMAP hBitmap;  // temporary
	if((hBitmap = CreateBitmap(pDC)) == NULL) return FALSE;
	int nSize = sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * m_nColorTableEntries;
	LPBITMAPINFOHEADER lpBMIH = (LPBITMAPINFOHEADER) new char[nSize];
	memcpy(lpBMIH, m_lpBMIH, nSize);  // new header
	if(bCompress) {
		switch (lpBMIH->biBitCount) {
		case 4:
			lpBMIH->biCompression = BI_RLE4;
			break;
		case 8:
			lpBMIH->biCompression = BI_RLE8;
			break;
		default:
			ASSERT(FALSE);
		}
		// calls GetDIBits with null data pointer to get size of compressed DIB
		if(!::GetDIBits(pDC->GetSafeHdc(), hBitmap, 0, (UINT) lpBMIH->biHeight,
						NULL, (LPBITMAPINFO) lpBMIH, DIB_RGB_COLORS)) {
			AfxMessageBox(_T("Unable to compress this DIB"));
			// probably a problem with the color table
	 		::DeleteObject(hBitmap);
			delete [] lpBMIH;
			::SelectPalette(hdc, hOldPalette, FALSE);
			return FALSE; 
		}
		if (lpBMIH->biSizeImage == 0) {
			AfxMessageBox(_T("Driver can't do compression"));
	 		::DeleteObject(hBitmap);
			delete [] lpBMIH;
			::SelectPalette(hdc, hOldPalette, FALSE);
			return FALSE; 
		}
		else {
			m_dwSizeImage = lpBMIH->biSizeImage;
		}
	}
	else {
		lpBMIH->biCompression = BI_RGB; // decompress
		// figure the image size from the bitmap width and height
		DWORD dwBytes = ((DWORD) lpBMIH->biWidth * lpBMIH->biBitCount) / 32;
		if(((DWORD) lpBMIH->biWidth * lpBMIH->biBitCount) % 32) {
			dwBytes++;
		}
		dwBytes *= 4;
		m_dwSizeImage = dwBytes * lpBMIH->biHeight; // no compression
		lpBMIH->biSizeImage = m_dwSizeImage;
	} 
	// second GetDIBits call to make DIB
	LPBYTE lpImage = (LPBYTE) new char[m_dwSizeImage];
	VERIFY(::GetDIBits(pDC->GetSafeHdc(), hBitmap, 0, (UINT) lpBMIH->biHeight,
    		lpImage, (LPBITMAPINFO) lpBMIH, DIB_RGB_COLORS));
    TRACE("dib successfully created - height = %d\n", lpBMIH->biHeight);
	::DeleteObject(hBitmap);
	Empty();
	m_nBmihAlloc = m_nImageAlloc = crtAlloc;
	m_lpBMIH = lpBMIH;
	m_lpImage = lpImage;
	ComputeMetrics();
	ComputePaletteSize(m_lpBMIH->biBitCount);
	MakePalette();
	::SelectPalette(hdc, hOldPalette, FALSE);
	TRACE("Compress: new palette size = %d\n", m_nColorTableEntries); 
	return TRUE;
}

BOOL CDib::Read(CFile* pFile)
{
	// 1. read file header to get size of info hdr + color table
	// 2. read info hdr (to get image size) and color table
	// 3. read image
	// can't use bfSize in file header
	Empty();
	int nCount, nSize;
	BITMAPFILEHEADER bmfh;
	try {
		nCount = pFile->Read((LPVOID) &bmfh, sizeof(BITMAPFILEHEADER));
		if(nCount != sizeof(BITMAPFILEHEADER)) {
			// Updated from generic to genericException when building for VS 2005
			AfxThrowFileException(CFileException::genericException,-1,(LPCTSTR) pFile->GetFileName());
			// throw new CException;
		}
		if(bmfh.bfType != 0x4d42) {
			// Updated from generic to genericException when building for VS 2005
			AfxThrowFileException(CFileException::genericException,-1,(LPCTSTR) pFile->GetFileName());
			// throw new CException;
		}
		nSize = bmfh.bfOffBits - sizeof(BITMAPFILEHEADER);
		m_lpBMIH = (LPBITMAPINFOHEADER) new char[nSize];
		m_nBmihAlloc = m_nImageAlloc = crtAlloc;
		nCount = pFile->Read(m_lpBMIH, nSize); // info hdr & color table
		ComputeMetrics();
		ComputePaletteSize(m_lpBMIH->biBitCount);
		m_BitCount = m_lpBMIH->biBitCount;
		MakePalette();
		m_lpImage = (LPBYTE) new char[m_dwSizeImage];
		nCount = pFile->Read(m_lpImage, m_dwSizeImage); // image only
	}
	catch(CException* pe) {
		AfxMessageBox(_T("Read error"));
		pe->Delete();

		throw std::exception("Cannot read DIB!");
		return FALSE;
	}
	return TRUE;
}

BOOL CDib::ReadSection(CFile* pFile, CDC* pDC /* = NULL */)
{
	// new function reads BMP from disk and creates a DIB section
	//    allows modification of bitmaps from disk
	// 1. read file header to get size of info hdr + color table
	// 2. read info hdr (to get image size) and color table
	// 3. create DIB section based on header parms
	// 4. read image into memory that CreateDibSection allocates
	Empty();
	int nCount, nSize;
	BITMAPFILEHEADER bmfh;
	try {
		nCount = pFile->Read((LPVOID) &bmfh, sizeof(BITMAPFILEHEADER));
		if(nCount != sizeof(BITMAPFILEHEADER)) {
			// Updated from generic to genericException when building for VS 2005
			AfxThrowFileException(CFileException::genericException,-1,(LPCTSTR) pFile->GetFileName());
			// throw new CException;
		}
		if(bmfh.bfType != 0x4d42) {
			// Updated from generic to genericException when building for VS 2005
			AfxThrowFileException(CFileException::genericException,-1,(LPCTSTR) pFile->GetFileName());
			// throw new CException;
		}
		nSize = bmfh.bfOffBits - sizeof(BITMAPFILEHEADER);
		m_lpBMIH = (LPBITMAPINFOHEADER) new char[nSize];
		m_nBmihAlloc = crtAlloc;
		m_nImageAlloc = noAlloc;
		nCount = pFile->Read(m_lpBMIH, nSize); // info hdr & color table
		if(m_lpBMIH->biCompression != BI_RGB) {
			// Updated from generic to genericException when building for VS 2005
			AfxThrowFileException(CFileException::genericException,-1,(LPCTSTR) pFile->GetFileName());
			// throw new CException;
		}
		ComputeMetrics();
		ComputePaletteSize(m_lpBMIH->biBitCount);
		MakePalette();
		UsePalette(pDC);
		m_hBitmap = ::CreateDIBSection(pDC->GetSafeHdc(), (LPBITMAPINFO) m_lpBMIH,
			DIB_RGB_COLORS,	(LPVOID*) &m_lpImage, NULL, 0);
		ASSERT(m_lpImage != NULL);
		nCount = pFile->Read(m_lpImage, m_dwSizeImage); // image only
	}
	catch(CException* pe) {
		AfxMessageBox(_T("ReadSection error"));
		pe->Delete();
		return FALSE;
	}
	return TRUE;
}

BOOL CDib::Write(CFile* pFile)
{
	BITMAPFILEHEADER bmfh;
	bmfh.bfType = 0x4d42;  // 'BM'
	int nSizeHdr = sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * m_nColorTableEntries;
	bmfh.bfSize = 0;
//	bmfh.bfSize = sizeof(BITMAPFILEHEADER) + nSizeHdr + m_dwSizeImage;
	// meaning of bfSize open to interpretation (bytes, words, dwords?) -- we won't use it
	bmfh.bfReserved1 = bmfh.bfReserved2 = 0;
	bmfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) +
			sizeof(RGBQUAD) * m_nColorTableEntries;	
	try {
		pFile->Write((LPVOID) &bmfh, sizeof(BITMAPFILEHEADER));
		pFile->Write((LPVOID) m_lpBMIH,  nSizeHdr);
		pFile->Write((LPVOID) m_lpImage, m_dwSizeImage);
	}
	catch(CException* pe) {
		pe->Delete();
		AfxMessageBox(_T("write error"));
		return FALSE;
	}
	return TRUE;
}

void CDib::Serialize(CArchive& ar)
{
	ULONGLONG dwPos;
	dwPos = ar.GetFile()->GetPosition();
	TRACE("CDib::Serialize -- pos = %d\n", dwPos);
	ar.Flush();
	dwPos = ar.GetFile()->GetPosition();
	TRACE("CDib::Serialize -- pos = %d\n", dwPos);
	if(ar.IsStoring()) 
	{
		Write(ar.GetFile());
	}
	else 
	{
		Read(ar.GetFile());
	}
}

// helper functions
void CDib::ComputePaletteSize(int nBitCount)
{
	if((m_lpBMIH == NULL) || (m_lpBMIH->biClrUsed == 0)) {
		switch(nBitCount) {
			case 1:
				m_nColorTableEntries = 2;
				break;
			case 4:
				m_nColorTableEntries = 16;
				break;
			case 8:
				m_nColorTableEntries = 256;
				break;
			case 16:
			case 24:
			case 32:
				m_nColorTableEntries = 0;
				break;
			default:
				ASSERT(FALSE);
		}
	}
	else {
		m_nColorTableEntries = m_lpBMIH->biClrUsed;
	}
	ASSERT((m_nColorTableEntries >= 0) && (m_nColorTableEntries <= 256)); 
}

void CDib::ComputeMetrics()
{
	if(m_lpBMIH->biSize != sizeof(BITMAPINFOHEADER)) {
		TRACE("Not a valid Windows bitmap -- probably an OS/2 bitmap\n");
		AfxThrowNotSupportedException();
		// throw new CException;
	}
	m_dwSizeImage = m_lpBMIH->biSizeImage;
	if(m_dwSizeImage == 0) {
		DWORD dwBytes = ((DWORD) m_lpBMIH->biWidth * m_lpBMIH->biBitCount) / 32;
		if(((DWORD) m_lpBMIH->biWidth * m_lpBMIH->biBitCount) % 32) {
			dwBytes++;
		}
		dwBytes *= 4;
		m_dwSizeImage = dwBytes * m_lpBMIH->biHeight; // no compression
		// Set realwidth member
		
	}
	m_RealWidth = m_lpBMIH->biHeight == 0 ? 0 : m_dwSizeImage/ m_lpBMIH->biHeight;
	m_lpvColorTable = (LPBYTE) m_lpBMIH + sizeof(BITMAPINFOHEADER);
}

void CDib::Empty()
{
	// this is supposed to clean up whatever is in the DIB
	DetachMapFile();
	if(m_nBmihAlloc == crtAlloc) 
	{
		delete [] m_lpBMIH;
	}
	else if(m_nBmihAlloc == heapAlloc) 
	{
		::GlobalUnlock(m_hGlobal);
		::GlobalFree(m_hGlobal);
	}
	if(m_nImageAlloc == crtAlloc) 
		delete [] m_lpImage;
	if(m_hPalette != NULL) 
		::DeleteObject(m_hPalette);
	if(m_hBitmap != NULL) 
		::DeleteObject(m_hBitmap);
	m_nBmihAlloc = m_nImageAlloc = noAlloc;
	m_hGlobal = NULL;
	m_lpBMIH = NULL;
	m_lpImage = NULL;
	m_lpvColorTable = NULL;
	m_nColorTableEntries = 0;
	m_dwSizeImage = 0;
	m_lpvFile = NULL;
	m_hMap = NULL;
	m_hFile = NULL;
	m_hBitmap = NULL;
	m_hPalette = NULL;
	m_RealWidth = 0;
}

void CDib::DetachMapFile()
{
	if(m_hFile == NULL) return;
	::UnmapViewOfFile(m_lpvFile);
	::CloseHandle(m_hMap);
	::CloseHandle(m_hFile);
	m_hFile = NULL;
}

// Allocates memory for byte pointer to image
void CDib::SetImagePt()
{
//	m_lpImage = (LPBYTE) m_lpvColorTable + 
//		sizeof(RGBQUAD)* m_nColorTableEntries;

	m_lpImage = new BYTE[m_dwSizeImage];

	m_nImageAlloc = crtAlloc;

}

// Temp function for setting image data
void CDib::SetPixels()
{

	for(DWORD x = 0; x < m_dwSizeImage ; x++)
	{
		if(x < m_dwSizeImage/4)
			m_lpImage[x] = 45;
		else if(x < m_dwSizeImage/2)
			m_lpImage[x] = 80;
		else if(x < 3*m_dwSizeImage/4)
			m_lpImage[x] = 118;
		else
			m_lpImage[x] = 255;
	}
}

// Commented out changes made 9/2/99 so function can be recalled

LPBYTE CDib::FillIn(CSize size, int nBitCount, const std::vector<RGBQUAD>& ColorTable, LPBYTE pBytes /*= NULL*/)
{
	RGBQUAD *pQuads = NULL;
	if( nBitCount ==  8 )
	{
		pQuads = new RGBQUAD[256];
		int nColors = std::min<int>( 256, static_cast<int>(ColorTable.size()) );
		for(int i = 0; i < nColors; i++ )
			pQuads[i] = ColorTable[i];
	}
	LPBYTE pOutBytes = FillIn(size, nBitCount, pQuads, pBytes );

	delete [] pQuads;

	return pOutBytes;
}

// Fills in members of CDib created with default constructor
// Returns pointer to image bits
LPBYTE CDib::FillIn(CSize size, int nBitCount, const RGBQUAD* pColorTable, LPBYTE pBytes /*= NULL*/ )
{
	// Block from constructor
//	m_nBmihAlloc = m_nImageAlloc = noAlloc;
	Empty();
	ComputePaletteSize(nBitCount);
	m_lpBMIH = (LPBITMAPINFOHEADER) new 
		char[sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * m_nColorTableEntries];
	m_nBmihAlloc = crtAlloc;
	m_lpBMIH->biSize = sizeof(BITMAPINFOHEADER);
	m_lpBMIH->biWidth = size.cx;
	m_lpBMIH->biHeight = size.cy;
	m_lpBMIH->biPlanes = 1;
	m_lpBMIH->biBitCount = nBitCount;
	m_lpBMIH->biCompression = BI_RGB;
	m_lpBMIH->biSizeImage = 0;
	m_lpBMIH->biXPelsPerMeter = 0;
	m_lpBMIH->biYPelsPerMeter = 0;
	m_lpBMIH->biClrUsed = m_nColorTableEntries;
	m_lpBMIH->biClrImportant = m_nColorTableEntries;
	ComputeMetrics();

	// Set color table
	if(m_lpBMIH->biBitCount == 8)
	{
		// make sure lpvcolortable has been allocated first!
		 memcpy(m_lpvColorTable,pColorTable,256*sizeof(RGBQUAD));

	}
	else
		memset(m_lpvColorTable, 0, sizeof(RGBQUAD) * m_nColorTableEntries);

	// Assign image memory
	if(pBytes == NULL)
	{
		m_lpImage = new BYTE[m_dwSizeImage];
		m_nImageAlloc = crtAlloc;
	}
	else	// Use pointer that was passed
	{
		m_lpImage = pBytes;
		m_nImageAlloc = noAlloc;
	}
	m_BitCount = nBitCount;
	
	return m_lpImage;
}

// Initializes color table
BOOL CDib::SetColorTable(RGBQUAD* pColorTable, int NumOfColors) 
{
	if(m_lpBMIH->biBitCount == 8 )
	{
		if( NumOfColors <= 256 )
		{
			// make sure lpvcolortable has been allocated first!
			memcpy(m_lpvColorTable,pColorTable,NumOfColors*sizeof(RGBQUAD));

			return TRUE;
		}
		else
			return FALSE;
	}
	else
	{
		return FALSE;
	}
}

void CDib::MakeDir( wchar_t* strPathname)
{
	if(strPathname[1] == ':')
	{
		wchar_t path[_MAX_PATH];
	
		wcscpy_s(path,_MAX_PATH,strPathname);

		wchar_t drive[_MAX_DRIVE];
		wchar_t dir[_MAX_DIR];
		wchar_t fname[_MAX_FNAME];
		wchar_t ext[_MAX_EXT];
		_wsplitpath_s(path, drive, _MAX_DRIVE, dir, _MAX_DIR, fname, _MAX_FNAME, ext, _MAX_EXT);
		_wmakepath_s(path, _MAX_PATH, drive, dir, NULL, NULL);

		_wmkdir(path);		// make directory
	}

}

BOOL CDib::ReadFromName(CString FileName)
{
	// 1. read file header to get size of info hdr + color table
	// 2. read info hdr (to get image size) and color table
	// 3. read image
	// can't use bfSize in file header
	CFile file(FileName.GetBuffer(1), CFile::modeRead);

	Empty();
	int nCount, nSize;
	BITMAPFILEHEADER bmfh;
	try {
		nCount = file.Read((LPVOID) &bmfh, sizeof(BITMAPFILEHEADER));
		if(nCount != sizeof(BITMAPFILEHEADER)) {
			// Updated from generic to genericException when building for VS 2005
			AfxThrowFileException(CFileException::genericException,-1,(LPCTSTR) file.GetFileName());
			// throw new CException;
		}
		if(bmfh.bfType != 0x4d42) {
			// Updated from generic to genericException when building for VS 2005
			AfxThrowFileException(CFileException::genericException,-1,(LPCTSTR) file.GetFileName());
			// throw new CException;
		}
		nSize = bmfh.bfOffBits - sizeof(BITMAPFILEHEADER);
		m_lpBMIH = (LPBITMAPINFOHEADER) new char[nSize];
		m_nBmihAlloc = m_nImageAlloc = crtAlloc;
		nCount = file.Read(m_lpBMIH, nSize); // info hdr & color table
		ComputeMetrics();
		ComputePaletteSize(m_lpBMIH->biBitCount);
		m_BitCount = m_lpBMIH->biBitCount;
		MakePalette();
		m_lpImage = (LPBYTE) new char[m_dwSizeImage];
		nCount = file.Read(m_lpImage, m_dwSizeImage); // image only

	/*	char temp;	//TURN IMAGE RIGHT SIDE UP
		for(DWORD half = 0; half < m_dwSizeImage/2 ; half++)
		{
			temp = m_lpImage[m_dwSizeImage - half];
			m_lpImage[m_dwSizeImage - half] = m_lpImage[half];
			m_lpImage[half] = temp;
		}*/

	}
	catch(CException* pe) {
		AfxMessageBox(_T("Read error"));
		pe->Delete();
		return FALSE;
	}
	return TRUE;
}

LPVOID CDib::GetColorTable(int bitcount)
{ 
	switch (bitcount)
	{
		case 1:
			if(m_nColorTableEntries == 2)
				return m_lpvColorTable;
			else
				return NULL;
			break;
		case 4:
			if(	m_nColorTableEntries == 16)
				return m_lpvColorTable;
			else
				return NULL;
			break;
		case 8:
			if( m_nColorTableEntries = 256)
				return m_lpvColorTable;
			else
				return NULL;
			break;
		case 16:
		case 24:
		case 32:
			return NULL;
			break;
		default:
			ASSERT(FALSE);
	}
	return NULL;
}

void CDib::GetColors( int bitcount, std::vector<RGBQUAD>& Colors )
{
	switch (bitcount)
	{
		case 1:
			if(m_nColorTableEntries == 2)
			{
				LPRGBQUAD pDibQuad = (LPRGBQUAD) m_lpvColorTable;
				Colors.resize(2);
				Colors[0] = *pDibQuad;
				++pDibQuad;
				Colors[1] = *pDibQuad;
			}
			else
				Colors.clear();
			break;
		case 4:
			if(	m_nColorTableEntries == 16)
			{
				LPRGBQUAD pDibQuad = (LPRGBQUAD) m_lpvColorTable;
				Colors.resize(16);
				for( int i = 0; i < 16; ++i, ++pDibQuad )
					Colors[i] = *pDibQuad;
			}
			else
				Colors.clear();
			break;
		case 8:
			if( m_nColorTableEntries = 256)
			{
				LPRGBQUAD pDibQuad = (LPRGBQUAD) m_lpvColorTable;
				Colors.resize(256);
				for( int i = 0; i < 256; ++i, ++pDibQuad )
					Colors[i] = *pDibQuad;
			}
			else
				Colors.clear();
			break;
		case 16:
		case 24:
		case 32:
			Colors.clear();
			break;
		default:
			Colors.clear();
			ASSERT(FALSE);
	}
}

// includes header and all total memory stored is returned
// most of the code adapted from Write(CFile *pFile)
// Used for converting file formats CxImage 2/6/04
BYTE* CDib::StoreInBuffer(DWORD &len)
{
	len = 0;

	if(m_lpImage == NULL || m_RealWidth ==0) 
	{
		AfxMessageBox(_T("Empty bitmap file"));
		return NULL;
	}

	BITMAPFILEHEADER bmfh;
	bmfh.bfType = 0x4d42;  // 'BM'
	int nSizeHdr = sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * m_nColorTableEntries;
	bmfh.bfSize = 0;
//	bmfh.bfSize = sizeof(BITMAPFILEHEADER) + nSizeHdr + m_dwSizeImage;
	// meaning of bfSize open to interpretation (bytes, words, dwords?) -- we won't use it
	bmfh.bfReserved1 = bmfh.bfReserved2 = 0;
	bmfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) +
			sizeof(RGBQUAD) * m_nColorTableEntries;	

	// new stuff
	len = sizeof(BITMAPFILEHEADER) + nSizeHdr + m_dwSizeImage;
	BYTE *pBuf = new BYTE[len];
	if(pBuf == NULL)
	{
		AfxMessageBox(_T("Allocation Error"));
		len=0;
		return NULL;
	}

	memset(pBuf,0,len);
	BYTE *pStore = pBuf;

	memcpy(pStore,(LPVOID) &bmfh, sizeof(BITMAPFILEHEADER));
	pStore = pBuf + sizeof(BITMAPFILEHEADER);
	memcpy(pStore,(LPVOID) m_lpBMIH, nSizeHdr);
	pStore = pBuf + sizeof(BITMAPFILEHEADER)+nSizeHdr;
	memcpy(pStore, (LPVOID) m_lpImage, m_dwSizeImage);

	return pBuf;
}

COLORREF CDib::GetColor(CPoint point)
{
	COLORREF color = RGB(0,0,0);
	if(! IsWidth())
		return color;

	int pos;
	CSize size = GetDimensions();
	if(point.x < 0 || point.x > size.cx)
		return color;

	if(point.y < 0 || point.y > size.cy)
		return color;

	if(m_BitCount <= 8)	// use color table
	{
		pos = point.y*m_RealWidth + point.x;
		RGBQUAD *pQuads = (RGBQUAD*)GetColorTable(m_BitCount);
		if(!pQuads)
			return color;
		RGBQUAD MyQUAD = pQuads[ (*(m_lpImage+pos))];
		color = RGB(MyQUAD.rgbRed,MyQUAD.rgbGreen,MyQUAD.rgbBlue);
	}
	else
	{
		pos = point.y*m_RealWidth + 3*point.x;
		color = RGB( (DWORD) *(m_lpImage + pos+2), (DWORD) *(m_lpImage + pos+1), (DWORD) *(m_lpImage + pos) );
	}

	return color;
}