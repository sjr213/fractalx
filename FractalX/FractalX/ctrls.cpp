// ctrls.cpp : implementation file
//

#include "stdafx.h"
//#include "ImageApp.h"
#include "ctrls.h"

CColorEdit::CColorEdit():m_TextColor(RGB(255,0,0)), m_BkColor(RGB(255,255,255))
{}

CColorEdit::~CColorEdit()
{}

BEGIN_MESSAGE_MAP(CColorEdit, CEdit)
//	ON_WM_PAINT()
//	ON_WM_CTLCOLOR()
	ON_WM_CTLCOLOR_REFLECT()
END_MESSAGE_MAP()

// CRedEdit message handlers

void CColorEdit::SetTextColor(COLORREF color)
{
	m_TextColor = color;
	Invalidate(TRUE);
}

void CColorEdit::SetBkColor(COLORREF color)
{
	m_BkColor = color;

	if(m_BkBrush.GetSafeHandle())
		m_BkBrush.DeleteObject();

	m_BkBrush.CreateSolidBrush(color);

	Invalidate();

}
HBRUSH CColorEdit::CtlColor(CDC* pDC, UINT /*nCtlColor*/)
{
	pDC->SetTextColor(m_TextColor);
	pDC->SetBkColor(m_BkColor);

	HBRUSH hbr;
	if(! m_BkBrush.GetSafeHandle())
		m_BkBrush.CreateSolidBrush(m_BkColor);

	hbr = m_BkBrush;

	// TODO:  Return a different brush if the default is not desired
	return hbr;
}
