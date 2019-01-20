
#include "stdafx.h"
#include "ClickMappedPictureCtrl.h"
#include "Messages.h"

namespace
{
	void InvertColors(byte* pixel)
	{
		if (pixel[0] != 255 || pixel[1] != 255 || pixel[2] != 255)
		{
			pixel[0] = 255 - pixel[0];
			pixel[1] = 255 - pixel[1];
			pixel[2] = 255 - pixel[2];
		}
	}

	void Darken(byte* pixel)
	{
		const int dif = 40;
		if (pixel[0] != 255 || pixel[1] != 255 || pixel[2] != 255)
		{
			pixel[0] = pixel[0] > dif ? pixel[0] - dif : 0;
			pixel[1] = pixel[1] > dif ? pixel[1] - dif : 0;
			pixel[2] = pixel[2] > dif ? pixel[2] - dif : 0;
		}
	}
}


IMPLEMENT_DYNAMIC(CClickMappedPictureCtrl, CStatic)
CClickMappedPictureCtrl::CClickMappedPictureCtrl()
	: m_parent(nullptr)
	, m_sendMousePositionMsg(false)
	, m_sendClickMsg(false)
	, m_shaderFunction(Darken)
{
}

CClickMappedPictureCtrl::~CClickMappedPictureCtrl()
{
}

void CClickMappedPictureCtrl::SetBitmap(CString strBitmap)
{
	m_bmpPath = strBitmap;

	m_gBitmap = std::make_unique<Gdiplus::Bitmap>(m_bmpPath);

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

void CClickMappedPictureCtrl::SetShaderFunction(const std::function<void(byte*)>& shaderFunction)
{
	m_shaderFunction = shaderFunction;
}

BEGIN_MESSAGE_MAP(CClickMappedPictureCtrl, CStatic)
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

COLORREF Invert(COLORREF inColor)
{
	auto red = GetRValue(inColor);
	auto green = GetGValue(inColor);
	auto blue = GetBValue(inColor);

	return RGB(255 - red, 255 - green, 255 - blue);
}

void CClickMappedPictureCtrl::DrawBitmap(CPaintDC &dc)
{
	Gdiplus::Graphics graphics(dc);

	CRect lRect;
	GetClientRect(lRect);
	lRect.NormalizeRect();

	Gdiplus::Rect gRect(lRect.left, lRect.top, lRect.Width(), lRect.Height());

	graphics.DrawImage(m_gBitmap.get(), gRect);
}

static Gdiplus::Rect GetGdiRect(const CRect& winRect)
{
	return Gdiplus::Rect(winRect.left, winRect.top, winRect.Width(), winRect.Height());
}

// I started off using win32 bitmaps/dibs but Gdiplus was much easier
static std::shared_ptr<Gdiplus::Bitmap> GetShadedBitmap(const Gdiplus::Rect& shadeRect, CString bmpPath, const std::function<void(byte*)>& shader)
{
	// I tried copying the original bitmap but ran into problems. Since this seems efficient enough I left as is.
	std::shared_ptr<Gdiplus::Bitmap> shadingBmp = std::make_shared<Gdiplus::Bitmap>(bmpPath);
	if (!shadingBmp)
		return nullptr;

	Gdiplus::Rect fullRect(0, 0, shadingBmp->GetWidth(), shadingBmp->GetHeight());

	Gdiplus::BitmapData bitmapData;
	auto status = shadingBmp->LockBits(&fullRect, Gdiplus::ImageLockModeRead | Gdiplus::ImageLockModeWrite, PixelFormat24bppRGB, &bitmapData);
	if (status != Gdiplus::Status::Ok)
		return shadingBmp;

	byte* start = (byte*)(void*)bitmapData.Scan0;
	int stride = bitmapData.Stride;

	for (int y = shadeRect.GetTop(); y < shadeRect.GetBottom(); ++y)
	{
		for (int x = shadeRect.GetLeft(); x < shadeRect.GetRight(); ++x)
		{
			int pos = y*stride + 3*x;
			byte* pixel = start + pos;

			shader(pixel);
		}
	}

	shadingBmp->UnlockBits(&bitmapData);

	return shadingBmp;
}

void CClickMappedPictureCtrl::DrawBitmapWithMask(CPaintDC &dc)
{
	Gdiplus::Graphics graphics(dc);

	CRect lRect;
	GetClientRect(lRect);
	lRect.NormalizeRect();

	Gdiplus::Rect gRect = GetGdiRect(lRect);

	auto maskRect = GetGdiRect(m_maskRect.value());

	auto shadedBmp = GetShadedBitmap(maskRect, m_bmpPath, m_shaderFunction);
	if (!shadedBmp)
		return;

	graphics.DrawImage(shadedBmp.get(), gRect);
}

void CClickMappedPictureCtrl::OnPaint()
{
	CPaintDC dc(this); 

	if (m_gBitmap)
	{
		if (m_maskRect.has_value())
			DrawBitmapWithMask(dc);
		else
			DrawBitmap(dc);	
	}
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

	m_maskRect = std::nullopt;
	Invalidate(FALSE);

	CStatic::OnLButtonUp(nFlags, point);
}


void CClickMappedPictureCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (m_parent && m_sendClickMsg)
	{
		for (const PictureClickTarget& target : m_targets)
		{
			if (target.TargetRect.PtInRect(point))
			{
				m_maskRect = target.ShadeRect;
				Invalidate(FALSE);
				break;
			}
		}
	}

	CStatic::OnLButtonDown(nFlags, point);
}
