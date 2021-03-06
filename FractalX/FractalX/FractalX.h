
// FractalX.h : main header file for the FractalX application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols
#include <gdiplus.h>

// CFractalXApp:
// See FractalX.cpp for the implementation of this class
//



class CFractalXApp : public CWinAppEx
{
public:
	CFractalXApp();

// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation
	UINT  m_nAppLook;
	BOOL  m_bHiColorIcons;

	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

	void ReadPalettePath(CString &str);
	void WritePalettePath(const CString& str);

	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()

protected:
	ULONG_PTR m_pGDI;
	Gdiplus::GdiplusStartupInput m_gdiplusStartupInput;
};

extern CFractalXApp theApp;
