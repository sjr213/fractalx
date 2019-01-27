
// FractalXView.h : interface of the CFractalXView class
//

#pragma once

class CFractalXViewImp;

class CFractalXView : public CView
{
protected: // create from serialization only

	CFractalXView();

	DECLARE_DYNCREATE(CFractalXView)

	virtual ~CFractalXView();

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

private:
	std::unique_ptr<CFractalXViewImp> m_pImp;
	CPoint m_lastPoint;

// Attributes
public:
	CFractalXDoc* GetDocument() const;

	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

protected:

	virtual void OnInitialUpdate();

	void ShowContextMenu(CWnd* /* pWnd */, CPoint point);

// Generated message map functions

	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
//	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnChangeRotation();
	afx_msg void OnUpdateEditRotation(CCmdUI *pCmdUI);
	afx_msg void OnEditPerspective();
	afx_msg void OnUpdateEditSetPerspective(CCmdUI *pCmdUI);
	afx_msg void OnEditView();
	afx_msg void OnUpdateEditSetView(CCmdUI *pCmdUI);
	afx_msg void OnEditSetModel();
	afx_msg void OnUpdateEditSetModel(CCmdUI *pCmdUI);
	afx_msg void OnEditPalette();
	afx_msg void OnUpdateEditPalette(CCmdUI *pCmdUI);
	afx_msg void OnEditAnglePosition();
	afx_msg void OnUpdateEditAnglePosition(CCmdUI *pCmdUI);

	// temp
	afx_msg void OnExportTriangles();
	
	DECLARE_MESSAGE_MAP()

	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	afx_msg LRESULT OnUpdateProgress(WPARAM, LPARAM);
	afx_msg LRESULT OnCalcFinished(WPARAM, LPARAM);
	afx_msg LRESULT OnKillPaletteView(WPARAM, LPARAM);
	afx_msg LRESULT OnKillPositionAngleDlg(WPARAM wparam, LPARAM);
	afx_msg LRESULT OnModelPositionChanged(WPARAM, LPARAM);
	afx_msg LRESULT OnModelAngleChanged(WPARAM, LPARAM);

	LRESULT OnRender(WPARAM, LPARAM);

	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
};

#ifndef _DEBUG  // debug version in FractalXView.cpp
inline CFractalXDoc* CFractalXView::GetDocument() const
   { return reinterpret_cast<CFractalXDoc*>(m_pDocument); }
#endif

