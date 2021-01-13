
// FractalX12View.h : interface of the CFractalX12View class
//

#pragma once

class CFractalX12ViewImpl;

class CFractalX12View : public CView
{
private:
	std::unique_ptr<CFractalX12ViewImpl> m_impl;

protected: // create from serialization only
	CFractalX12View();

	DECLARE_DYNCREATE(CFractalX12View)

// Attributes
public:
	void OnInitialUpdate() override;
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
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnStartStopTimer();
	afx_msg void OnUpdateStartStopTimer(CCmdUI* ccmdUI);
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in FractalX12View.cpp
inline CFractalX12Doc* CFractalX12View::GetDocument() const
   { return reinterpret_cast<CFractalX12Doc*>(m_pDocument); }
#endif

