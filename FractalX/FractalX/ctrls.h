#pragma once

// CColorEdit

class AFX_EXT_CLASS CColorEdit : public CEdit
{
	DECLARE_DYNAMIC(CColorEdit)

public:
	CColorEdit();
	virtual ~CColorEdit();

	void SetTextColor(COLORREF color);
	void SetBkColor(COLORREF color);

	COLORREF m_TextColor;
	COLORREF m_BkColor;

	CBrush m_BkBrush;

protected:
	DECLARE_MESSAGE_MAP()

public:
	afx_msg HBRUSH CtlColor(CDC* /*pDC*/, UINT /*nCtlColor*/);
};


