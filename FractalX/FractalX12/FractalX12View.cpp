
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

#include "Renderer12.h"

//#include "LandAndWaves/Renderer12LandAndWaves.h"
#include "Stencil\Renderer12Stencil.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

class CFractalX12ViewImpl
{
private:
	CFractalX12View* m_pThis;
	std::shared_ptr<Renderer12> m_renderer = CreateRenderer12Stencil(); // CreateRenderer12LandAndWaves();
	UINT_PTR m_timerId = 0;

public:
	CFractalX12ViewImpl(CFractalX12View* pThis)
		: m_pThis(pThis)
	{}

	virtual ~CFractalX12ViewImpl()
	{}

	void Initialize()
	{
//		auto pDoc = m_pThis->GetDocument();
//		if (!pDoc)
//			return;

// 		CSize size = pDoc->GetSize();
		CSize size(800, 800);
		InitializeWindow(size);

		auto hnd = m_pThis->GetSafeHwnd();
		if (!hnd)
			return;

		// SEE IF WE CAN GROUP THESE
		m_renderer->Initialize(hnd, size.cx, size.cy);
	}

	void ResizeRenderer(int cx, int cy)
	{
		if (m_renderer)
			m_renderer->OnWindowSizeChanged(cx, cy);
	}

	void InitializeWindow(CSize size)
	{
		auto mainFrame = AfxGetMainWnd();
		if (!mainFrame)
			return;

		auto pFrame = m_pThis->GetParentFrame();
		if (pFrame)
		{
			CRect windRect;
			pFrame->GetWindowRect(windRect);
			mainFrame->ScreenToClient(windRect);

			// We subtract a constant value from top to get the window drawn as close to the top as possible
			pFrame->MoveWindow(windRect.left, windRect.top - 50, size.cx, size.cy);
		}
	}

	void Draw()
	{
		m_renderer->Draw();
	}

	void MouseDown(WPARAM btnState, int x, int y)
	{
		m_renderer->MouseDown(btnState, x, y);
	}

	void MouseUp(WPARAM btnState, int x, int y)
	{
		m_renderer->MouseUp(btnState, x, y);
	}

	void MouseMove(WPARAM btnState, int x, int y)
	{
		m_renderer->MouseMove(btnState, x, y);
	}

	void OnTimer()
	{
		m_renderer->OnTimer(m_timerId != 0);
	}

	void StartStopTimer()
	{
		if (m_timerId == 0)
		{
			m_timerId = m_pThis->SetTimer(1, 50, nullptr);
			m_renderer->OnTimer(m_timerId != 0);
		}
		else
		{
			if (m_pThis->KillTimer(m_timerId))
				m_timerId = 0;

			m_renderer->OnTimer(m_timerId != 0);
		}
	}
};

// CFractalX12View

IMPLEMENT_DYNCREATE(CFractalX12View, CView)

BEGIN_MESSAGE_MAP(CFractalX12View, CView)
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_TIMER()	// ID_TIMER_START
	ON_COMMAND(ID_TIMER_START, OnStartStopTimer)
	ON_UPDATE_COMMAND_UI(ID_TIMER_START, OnUpdateStartStopTimer)
END_MESSAGE_MAP()

// CFractalX12View construction/destruction

CFractalX12View::CFractalX12View()
	: m_impl(new CFractalX12ViewImpl(this))
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

void CFractalX12View::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	m_impl->Initialize();
}

// CFractalX12View drawing

void CFractalX12View::OnDraw(CDC* /*pDC*/)
{
	CFractalX12Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	m_impl->Draw();
}

void CFractalX12View::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	m_impl->ResizeRenderer(cx, cy);
}

void CFractalX12View::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}

void CFractalX12View::OnMouseMove(UINT nFlags, CPoint point)
{
	WPARAM btnState = nFlags;

	m_impl->MouseMove(btnState, point.x, point.y);

	Invalidate(TRUE);

	CView::OnMouseMove(nFlags, point);
}

void CFractalX12View::OnLButtonDown(UINT nFlags, CPoint point)
{
	SetCapture();

	WPARAM btnState = nFlags;
	m_impl->MouseDown(btnState, point.x, point.y);

	Invalidate(TRUE);

	CView::OnLButtonDown(nFlags, point);
}

void CFractalX12View::OnLButtonUp(UINT nFlags, CPoint point)
{
	WPARAM btnState = nFlags;
	m_impl->MouseUp(btnState, point.x, point.y);

	ReleaseCapture();

	Invalidate(TRUE);

	CView::OnLButtonUp(nFlags, point);
}

void CFractalX12View::OnRButtonDown(UINT nFlags, CPoint point)
{
	SetCapture();

	WPARAM btnState = nFlags;
	m_impl->MouseDown(btnState, point.x, point.y);

	Invalidate(TRUE);

	CView::OnRButtonDown(nFlags, point);
}

void CFractalX12View::OnRButtonUp(UINT nFlags, CPoint point)
{
	WPARAM btnState = nFlags;
	m_impl->MouseUp(btnState, point.x, point.y);

	ReleaseCapture();

	Invalidate(TRUE);

	ClientToScreen(&point);
	OnContextMenu(this, point);

	CView::OnRButtonUp(nFlags, point);
}

void CFractalX12View::OnTimer(UINT_PTR nIDEvent)
{
	m_impl->OnTimer();

	CView::OnTimer(nIDEvent);
}

void CFractalX12View::OnStartStopTimer()
{
	m_impl->StartStopTimer();
}

void CFractalX12View::OnUpdateStartStopTimer(CCmdUI* ccmdUI)
{
	ccmdUI->Enable(TRUE);
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
