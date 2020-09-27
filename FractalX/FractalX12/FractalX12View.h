
// FractalX12View.h : interface of the CFractalX12View class
//

#pragma once


class CFractalX12View : public CView
{
protected: // create from serialization only
	CFractalX12View() noexcept;
	DECLARE_DYNCREATE(CFractalX12View)

// Attributes
public:
	CFractalX12Doc* GetDocument() const;

// Operations
public:

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

// Implementation
public:
	virtual ~CFractalX12View();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in FractalX12View.cpp
inline CFractalX12Doc* CFractalX12View::GetDocument() const
   { return reinterpret_cast<CFractalX12Doc*>(m_pDocument); }
#endif

