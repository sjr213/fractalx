// cdib.h declaration for Inside Visual C++ CDib class
// changed 9/12/04 for param movie - added SetBytes()

#ifndef _INSIDE_VISUAL_CPP_CDIB
#define _INSIDE_VISUAL_CPP_CDIB

class CDib : public CObject
{
protected:
	enum Alloc {noAlloc, crtAlloc, heapAlloc};
	DECLARE_SERIAL(CDib)

private:
	LPVOID m_lpvColorTable;
	DWORD m_RealWidth;
	
	HBITMAP m_hBitmap;
	LPBYTE m_lpImage;				// starting address of DIB bits
	LPBITMAPINFOHEADER m_lpBMIH;	//  buffer containing the BITMAPINFOHEADER


	HGLOBAL m_hGlobal;				// For external windows we need to free;
									//  could be allocated by this class or allocated externally
	Alloc m_nBmihAlloc;
	Alloc m_nImageAlloc;
	DWORD m_dwSizeImage;			// of bits -- not BITMAPINFOHEADER or BITMAPFILEHEADER
	int m_nColorTableEntries;
	
	HANDLE m_hFile;
	HANDLE m_hMap;
	LPVOID m_lpvFile;
	HPALETTE m_hPalette;
	int m_BitCount;

public:
	CDib();
	CDib(CSize size, int nBitCount);	// builds BITMAPINFOHEADER
	~CDib();

	// Fills in the rest of a CDib created with default constructor
	// Returns pointer to image bits
	LPBYTE FillIn(CSize size, int nBitCount, const RGBQUAD* pColorTable, LPBYTE pBytes = NULL);
	LPBYTE FillIn(CSize size, int nBitCount, const std::vector<RGBQUAD>& ColorTable, LPBYTE pBytes = NULL);

	void Empty();

	// Accessors
	//////////////////////////////////////////////////////////////////////////////////////////
	bool IsWidth(){ 
		if(m_RealWidth > 0) 
			return true;
		else 
			return false;
	}

	int GetSizeImage() {return m_dwSizeImage;} 

	int GetBitCount(){return m_BitCount;}

	int GetSizeHeader() {return sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * m_nColorTableEntries;}
	
	CSize GetDimensions();

	DWORD GetBitWidth() {return m_RealWidth;}

	// Memory
	///////////////////////////////////////////////////////////////////////////////////////////
	BOOL AttachMapFile(const wchar_t* strPathname, BOOL bShare = FALSE);
	
	BOOL CopyToMapFile( wchar_t* strPathname);
	
	BOOL AttachMemory(LPVOID lpvMem, BOOL bMustDelete = FALSE, HGLOBAL hGlobal = NULL);

	HBITMAP CreateSection(CDC* pDC = NULL);

	BOOL Compress(CDC* pDC, BOOL bCompress = TRUE);			// FALSE means decompress

	HBITMAP CreateBitmap(CDC* pDC);

	void DetachMapFile();

	// File / serialization
	//////////////////////////////////////////////////////////////////////////////////////////
	BOOL Read(CFile* pFile);

	BOOL ReadFromName(CString FileName);

	BOOL ReadSection(CFile* pFile, CDC* pDC = NULL);

	BOOL Write(CFile* pFile);

	void Serialize(CArchive& ar);

	// Used for converting file formats 2/6/04 CxImage
	BYTE* StoreInBuffer(DWORD &len);

	void MakeDir(wchar_t* strPathname);	// Makes the directory if it doesn't exist

	// Drawing
	///////////////////////////////////////////////////////////////////////////////////////////
	BOOL Draw(CDC* pDC, CPoint origin, CSize size);			// until we implemnt CreateDibSection

	BOOL DrawBetter(CDC* pDC, CPoint origin, CSize size);	// added for printing 1/2/04
	BOOL DrawDirect(CDC* pDC, CPoint origin, CSize size);

	// Draws the part of the bitmap in the range provided by the src coordinates
	// The source coordinates must be within the bitmaps size
	BOOL DrawPartWidth(CDC* pDC, CPoint destTopLeft, CSize destSize, int srcLeft, int srcRight);
	BOOL DrawPart(CDC* pDC, CPoint destTopLeft, CSize destSize, int srcLeft, int srcTop, int srcRight, int srcBottom);

	// Palette
	///////////////////////////////////////////////////////////////////////////////////////////
	UINT UsePalette(CDC* pDC, BOOL bBackground = FALSE);

	BOOL MakePalette();

	BOOL SetSystemPalette(CDC* pDC);

	// Colors
	///////////////////////////////////////////////////////////////////////////////////////////
	BOOL SetColorTable(RGBQUAD* pColorTable, int NumOfColors);		// Initializes color table

	LPVOID GetColorTable(int bitcount);								// New for fractals 9/20/99

	void GetColors( int bitcount, std::vector<RGBQUAD>& Colors );

	COLORREF GetColor(CPoint point);

	int GetNumColors() { return m_nColorTableEntries; };
	
	// Image 
	///////////////////////////////////////////////////////////////////////////////////////////
	// My additional functions
	LPBYTE GetBytes(){ return m_lpImage;};

	void SetImagePt();			// Allocates memory for byte image pointer
	void SetPixels();			// Probably temporary

	// for param movie 9/12/04
	// for some reason assigning the pointer outside of the dib doesn't work
	// be careful - no checking - assumes calling code knowns length and all
	void SetBytes(LPBYTE pBytes){ m_lpImage = pBytes; }

	// user must delete the dib object returned from each of these
	////////////////////////////////////////////////////////////
	// makes a copy of the dib
	std::shared_ptr<CDib> Copy();

	// These make a new dib with the image orientation transformed relative to the original
	std::shared_ptr<CDib> FlipHorizontal();
	std::shared_ptr<CDib> FlipVertical();
	std::shared_ptr<CDib> Rotate90Degrees();
	std::shared_ptr<CDib> Rotate180Degrees();
	std::shared_ptr<CDib> Rotate270Degrees();
	////////////////////////////////////////////////////////////



protected:
	// copy parameters (which came from another dib) to this dib
	// called by CDib* Copy()
	// Very similar to FillIn() except new memory allocated for image bits
	// and all parameters are needed
	void Copy(CSize size, int nBitCount, RGBQUAD* pColorTable, LPBYTE pBytes );

private:
	void ComputePaletteSize(int nBitCount);
	void ComputeMetrics();
};
#endif // _INSIDE_VISUAL_CPP_CDIB
