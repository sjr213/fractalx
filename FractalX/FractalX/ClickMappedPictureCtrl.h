// initial draft from https://www.go4expert.com/articles/display-bitmap-picture-box-using-mfc-t350/

#pragma once
#include <list>
#include <optional>

struct PictureClickTarget
{
	CRect TargetRect;
	int TargetId;
};


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

