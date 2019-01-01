
#include "stdafx.h"
#include "ClickMappedPictureCtrl.h"
#include "Messages.h"


IMPLEMENT_DYNAMIC(CClickMappedPictureCtrl, CStatic)
CClickMappedPictureCtrl::CClickMappedPictureCtrl()
	: m_parent(nullptr)
	, m_sendMousePositionMsg(false)
	, m_sendClickMsg(false)
{
}

CClickMappedPictureCtrl::~CClickMappedPictureCtrl()
{
}

void CClickMappedPictureCtrl::SetBitmap(CString strBitmap)
{
	m_bmpPath = strBitmap;

	// This expects a path to a .bmp file
	HBITMAP hBitmap = (HBITMAP) ::LoadImage(NULL, // AfxGetInstanceHandle(),
		m_bmpPath, IMAGE_BITMAP, 0, 0,
		LR_LOADFROMFILE | LR_CREATEDIBSECTION);

	// Do we have a valid handle for the loaded image?
	if (hBitmap)
	{
		if (m_bitmap.DeleteObject())
			m_bitmap.Detach();

		m_bitmap.Attach(hBitmap);
	}
	m_bitmap.GetBitmap(&m_bmpHeader);
	Invalidate();
}

void CClickMappedPictureCtrl::SendMousePositionMessage(bool sendMousePositionMessage)
{
	m_sendMousePositionMsg = sendMousePositionMessage;
}

void CClickMappedPictureCtrl::SendClickMessage(bool sendClickMessage)
{
	m_sendClickMsg = sendClickMessage;
}

void CClickMappedPictureCtrl::SetParent(CWnd* pWnd)
{
	m_parent = pWnd;
}

void CClickMappedPictureCtrl::SetClickTargets(const std::list<PictureClickTarget>& targets)
{
	m_targets = targets;
}

BEGIN_MESSAGE_MAP(CClickMappedPictureCtrl, CStatic)
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()


void CClickMappedPictureCtrl::ShowBitmap(CPaintDC *pdc)
{
	//Create a device context to load the bitmap into
	CDC dcMem;
	dcMem.CreateCompatibleDC(pdc);

	//Get the Display area available
	CRect lRect;
	GetClientRect(lRect);
	lRect.NormalizeRect();

	//select the bitmap into compatible device context
	CBitmap* pOldBitmap = (CBitmap*)dcMem.SelectObject(&m_bitmap);

	//copy & resize the window to the dialog window
	pdc->StretchBlt(0, 0, lRect.Width(), lRect.Height(), &dcMem, 0, 0, m_bmpHeader.bmWidth, m_bmpHeader.bmHeight, SRCCOPY);
}

void CClickMappedPictureCtrl::OnPaint()
{
	CPaintDC dc(this); 

	RECT rect;
	GetClientRect(&rect);
	dc.FillSolidRect(&rect, RGB(255, 255, 255));

	if (m_bmpPath != "")
		ShowBitmap(&dc);
}

void CClickMappedPictureCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_parent && m_sendMousePositionMsg)
	{
		m_parent->PostMessage(cMessage::tm_mouseCoords, point.x, point.y);
	}

	CStatic::OnMouseMove(nFlags, point);
}


void CClickMappedPictureCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_parent && m_sendClickMsg)
	{
		for (const PictureClickTarget& target : m_targets)
		{
			if (target.TargetRect.PtInRect(point))
				m_parent->PostMessage(cMessage::tm_clickIDs, target.TargetId);
		}
	}

	CStatic::OnLButtonUp(nFlags, point);
}
