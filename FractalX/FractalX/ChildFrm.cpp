
// ChildFrm.cpp : implementation of the CChildFrame class
//

#include "stdafx.h"
#include "FractalX.h"

#include "ChildFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CChildFrame

IMPLEMENT_DYNCREATE(CChildFrame, CMDIChildWndEx)

BEGIN_MESSAGE_MAP(CChildFrame, CMDIChildWndEx)
	ON_WM_CREATE()
END_MESSAGE_MAP()

// CChildFrame construction/destruction

CChildFrame::CChildFrame()
{}

CChildFrame::~CChildFrame()
{}

BOOL CChildFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying the CREATESTRUCT cs
	if( !CMDIChildWndEx::PreCreateWindow(cs) )
		return FALSE;

	return TRUE;
}

// CChildFrame diagnostics

#ifdef _DEBUG
void CChildFrame::AssertValid() const
{
	CMDIChildWndEx::AssertValid();
}

void CChildFrame::Dump(CDumpContext& dc) const
{
	CMDIChildWndEx::Dump(dc);
}
#endif //_DEBUG

// CChildFrame message handlers
int CChildFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIChildWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	// Create and set up the status bar 
	m_StatusBar.Create(this);

	static UINT indicators[] = { ID_INDICATOR_TEXT };
	m_StatusBar.SetIndicators(indicators, sizeof(indicators) / sizeof(UINT));
	m_StatusBar.SetPaneStyle(0, SBPS_STRETCH);

	return 0;
}

void CChildFrame::SetStatusText1(CString str)
{
	m_StatusBar.SetPaneText(0, str);
}
