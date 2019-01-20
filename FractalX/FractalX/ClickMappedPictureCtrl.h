// initial draft from https://www.go4expert.com/articles/display-bitmap-picture-box-using-mfc-t350/

#pragma once
#include <list>
#include <optional>

// When the mouse goes down within a TargetRect 
// The ShadeRect area is shaded using m_shaderFunction until the mouse comes up
// Then a cMessage::tm_clickIDs is posted to the parent window with the TargetID in wparam
// So the shader area can be different then the clicked area.
// The shader function can do anything on the RGB pixels within the rect.
// The hard coded functions darken none white pixels or invert the color of non-white pixels
struct PictureClickTarget
{
	CRect TargetRect;
	CRect ShadeRect;
	int TargetId;
};

// The idea of this control is to provide a hot-mapped image. When you click on certain areas a message is sent 
// to the parent. The image can be changed while the mouse is down to indicate what function was clicked. 
// To receive tm_clickIDs messages you need to call SendClickMessage(true)
// When developing you can determine the position of the mouse by calling SendMousePositionMessage(true)
// That will result in the control sending a tm_mouseCoords when the mouse moves with the x and y coordinate
// in lparam and lparam respectively.

class CClickMappedPictureCtrl : public CStatic
{
	DECLARE_DYNAMIC(CClickMappedPictureCtrl)

public:
	CClickMappedPictureCtrl();
	virtual ~CClickMappedPictureCtrl();
	void SetBitmap(CString strBitmap);
	void SetParent(CWnd* pWnd);
	void SendMousePositionMessage(bool sendMousePositionMessage);
	void SendClickMessage(bool sendClickMessage);
	void SetClickTargets(const std::list<PictureClickTarget>& targets);
	void SetShaderFunction(const std::function<void(byte*)>& shaderFunction);

protected:
	DECLARE_MESSAGE_MAP()

	void DrawBitmap(CPaintDC &dc);
	void DrawBitmapWithMask(CPaintDC &dc);

	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);

	CString m_bmpPath;
	CBitmap m_bitmap;
	BITMAP m_bmpHeader;
	CWnd* m_parent;
	bool m_sendMousePositionMsg;
	bool m_sendClickMsg;
	std::list<PictureClickTarget> m_targets;
	std::optional<CRect> m_maskRect;

	std::unique_ptr<Gdiplus::Bitmap> m_gBitmap;
	std::function<void(byte*)> m_shaderFunction;

public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};

