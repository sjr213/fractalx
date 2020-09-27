
// FractalX12View.cpp : implementation of the CFractalX12View class
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "FractalX12.h"
#endif

#include "FractalX12Doc.h"
#include "FractalX12View.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CFractalX12View

IMPLEMENT_DYNCREATE(CFractalX12View, CView)

BEGIN_MESSAGE_MAP(CFractalX12View, CView)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()

// CFractalX12View construction/destruction

CFractalX12View::CFractalX12View() noexcept
{
	// TODO: add construction code here

}

CFractalX12View::~CFractalX12View()
{
}

BOOL CFractalX12View::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CFractalX12View drawing

void CFractalX12View::OnDraw(CDC* /*pDC*/)
{
	CFractalX12Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here
}

void CFractalX12View::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CFractalX12View::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CFractalX12View diagnostics

#ifdef _DEBUG
void CFractalX12View::AssertValid() const
{
	CView::AssertValid();
}

void CFractalX12View::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CFractalX12Doc* CFractalX12View::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CFractalX12Doc)));
	return (CFractalX12Doc*)m_pDocument;
}
#endif //_DEBUG


// CFractalX12View message handlers
