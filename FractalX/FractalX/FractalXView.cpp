
// FractalXView.cpp : implementation of the CFractalXView class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "FractalX.h"
#endif

#include <future>
#include <thread>

#include "CancelException.h"
#include "ColorContrast.h"
#include "DxfColorFactory.h"
#include "FractalXDoc.h"
#include "FractalXView.h"
#include "Messages.h"
#include "ModelData.h"
#include "ModelSheet.h"
#include "Renderer.h"
#include "PerspectiveDlg.h"
#include "ProgressCancelSignaling.h"
#include "ProgressDlg.h"
#include "RotationParams.h"
#include "RotationDlg.h"
#include "TextureDlg.h"
#include "TraceParams.h"
#include "vertexFactory.h"
#include "ViewDlg.h"
#include "Palettes.h"
#include <vcruntime_exception.h>
#include "ColorUtilities.h"
#include "PaletteViewDlg.h"
#include "TriangleLoader.h"

// can remove after exporting triangles
#include "DxSerialize.h"


using namespace DXF;
using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static void CalculateModelFunc(promise<shared_ptr<DxVertexData>> result_promise, ModelData modelData, TraceParams traceParams,
	HWND hwnd, const function<void(double)>& setProgress) noexcept
{
	WPARAM state = cMessage::CalcFinished;
	try
	{
		auto triangles = TriangleLoader::GetTriangles(modelData, [&](double progress)
		{
			setProgress(progress / 3.0);
		});

		auto pVertexData = CreateBulb(*triangles, traceParams, [&](double progress)
		{
			setProgress(0.333 + 2.0*progress / 3.0);
		});

		result_promise.set_value(pVertexData);
	}
	catch (CCancelException)
	{
		state = cMessage::CalcCanceled;
	}

	CWnd* pWnd = CWnd::FromHandle(hwnd);

	pWnd->PostMessage(cMessage::tm_finish, state, 0);
}

// CFractalXView

class CFractalXViewImp
{
public:
	CFractalXViewImp(CFractalXView* pThis)
		: m_pThis(pThis)
		, m_cancelEvent(new CancelEvent)
	{}

	virtual ~CFractalXViewImp()
	{}

	RotationParams GetRotationParams() const
	{
		return m_rotationParams;
	}

	void Initialize(HWND hnd, CSize clientSize)
	{
		auto pDoc = m_pThis->GetDocument();
		if (!pDoc)
			return;

		m_renderer->Initialize(hnd, clientSize.cx, clientSize.cy);
		InitializeColors();

		RotationParams rp;
		rp.Action = RotationAction::RotateX;
		RunRotationParams(rp);

		m_renderer->SetModel(*pDoc->GetVertexData());
		m_renderer->ResetModel();
	}

	void InitializeColors()
	{
		auto pDoc = m_pThis->GetDocument();
		if (!pDoc)
			return;

		// need to rethink this
		DxColor::PinPalette palette = pDoc->GetPalette();
		auto contrast = pDoc->GetContrast();

		m_colors = fx::ColorUtilities::CalculatePaletteColors(palette, m_numberOfColors, contrast);

		m_renderer->SetTextureColors(m_colors);
	}

	void ResizeRenderer(int cx, int cy)
	{
		if (m_renderer)
			m_renderer->OnWindowSizeChanged(cx, cy);
	}

	void UpdateRotation()
	{
		if (m_rotationParams.Action == RotationAction::Fixed)
		{
			m_renderer->RefreshRender(0);
		}
		else
		{
			m_renderer->Tick();
		}
	}

	void RenderIfFixed()
	{
		if (m_rotationParams.Action == RotationAction::Fixed)
		{
			m_renderer->RefreshRender(0);
		}
	}

	void RunRotationParams(RotationParams rp)
	{
		m_rotationParams = rp;
		m_renderer->SetRotationParams(m_rotationParams);

		SetRotationTimer();
		UpdateRotation();
	}

	void OnColors()
	{
		CTextureDlg dlg;
		if (dlg.DoModal() == IDOK)
		{
			int textId = dlg.GetTextureId();
			if (textId < 0 || textId >= GetNumberOfTextures())
				return;

			std::shared_ptr<DxColor::PinPalette> palette;

			if (textId == 0)
				palette = DxColor::GetRainbow();
			else if (textId == 1)
				palette = DxColor::GetStripes();
			else
				throw std::exception("Unknown palette");

			DxColor::ValidatePalette(*palette);

			auto pDoc = m_pThis->GetDocument();
			if (!pDoc)
				return;

			m_colors = fx::ColorUtilities::CalculatePaletteColors(*palette, m_numberOfColors, pDoc->GetContrast());

			pDoc->SetPalette(*palette);

			m_renderer->SetTextureColors(m_colors);

			RenderIfFixed();
		}
	}

	void OnEditPerspective()
	{
		float nearView = m_renderer->GetNear();
		float farView = m_renderer->GetFar();

		CPerspectiveDlg dlg;
		dlg.SetNear(nearView);
		dlg.SetFarDistance(farView);

		if (dlg.DoModal() == IDOK)
		{
			nearView = dlg.GetNearDistance();
			farView = dlg.GetFarDistance();

			m_renderer->SetPerspective(nearView, farView);
			RenderIfFixed();
		}
	}

	void OnEditView()
	{
		auto camera = m_renderer->GetCamera();
		auto target = m_renderer->GetTarget();

		CViewDlg dlg;
		dlg.SetCamera(camera);
		dlg.SetTarget(target);

		if (dlg.DoModal() == IDOK)
		{
			camera = dlg.GetCamera();
			target = dlg.GetTarget();

			m_renderer->SetView(camera, target);
			RenderIfFixed();
		}
	}

	void OnEditSetModel()
	{
		auto pDoc = m_pThis->GetDocument();
		if (!pDoc)
			return;

		CModelSheet modelSheet(_T("Model"), m_pThis);
		modelSheet.SetModelData(pDoc->GetModelData());
		modelSheet.SetTraceParams(pDoc->GetTraceParams());

		if (modelSheet.DoModal() == IDOK)
		{
			pDoc->SetModelData(modelSheet.GetModelData());
			pDoc->SetTraceParams(modelSheet.GetTraceParams());

			CalculateModel();
		}
	}

	void OnEditPalette()
	{
		if (m_paletteViewDlg)
			return;

		auto pDoc = m_pThis->GetDocument();
		if (!pDoc)
			return;

		m_paletteViewDlg = CPaletteViewDlg::CreatePaletteViewDlg(pDoc->GetPalette(), pDoc->GetContrast(), m_pThis);

		m_paletteViewDlg->SetNewPaletteMethod([&](const DxColor::PinPalette& palette, const DxColor::ColorContrast& contrast)
		{
			SetNewPalette(palette, contrast);
		});

		m_paletteViewDlg->Create(IDD_PALETTE_VIEW_DLG, NULL);
	}

	void OnKillPaletteView()
	{
		if (! m_paletteViewDlg)
			return;

		m_paletteViewDlg.reset();
	}

	bool CanRender() const
	{
		return (m_renderer && m_renderer->IsReady());
	}

	LRESULT OnUpdateProgress(WPARAM wparam)
	{
		if (!m_progressDlg)
			return 0;

		double progress = cMessage::MakeDoubleProgress(wparam);
		m_progressDlg->SetProgress(progress);

		return 0;
	}

	LRESULT OnCalcFinished(WPARAM wparam)
	{
		m_progressDlg.reset();

		if (wparam == cMessage::CalcFinished)
		{
			auto pDoc = m_pThis->GetDocument();
			if (!pDoc)
				return 0;

			// update the model
			auto vertexData = m_futureVertexData.get();

			if (!vertexData)
				return 0;

			// a little risky since we're in another thread
			// consider locking data
			pDoc->SetVertexData(vertexData);

			m_renderer->SetModel(*vertexData);

			m_renderer->ResetModel();

			auto traceParams = pDoc->GetTraceParams();
			traceParams.Stretch = vertexData->StretchParams;
			pDoc->SetTraceParams(traceParams);
		}

		m_calcThread->join();

		m_calcThread.reset();

		return 0;
	}

	LRESULT OnRender()
	{
		// Only need to render here if fixed because there will be no timer
		if (m_rotationParams.Action == RotationAction::Fixed)
		{
			m_renderer->RefreshRender(0);
		}

		return 0;
	}

	std::shared_ptr<CancelEvent> GetCancelEvent()
	{
		return m_cancelEvent;
	}

	void OnExportTriangles()
	{
		CString fileName = _T("test1");
		CString fileExt(_T("triangles"));
		CString fileType;
		fileType.Format(_T("triangle files (*%s)|*%s|ALL Files |*.*||"), fileExt, fileExt);

		// Open a Save As dialog to get a name
		CFileDialog exportDlg(FALSE, fileExt, fileName, OFN_PATHMUSTEXIST, fileType);

		// Display as modal 
		if (exportDlg.DoModal() != IDOK)
			return;

		fileName = exportDlg.GetPathName();

		// Create a CFile and then a CArchive
		CFile triFile;

		// do something about exceptions
		CFileException FileExcept;
		if (!triFile.Open(fileName, CFile::modeCreate | CFile::modeWrite, &FileExcept))
		{
			CString error;
			wchar_t message[100] = { 0 };
			FileExcept.GetErrorMessage(message, 100);
			error.Format(_T("Error opening file: %s: %s"), fileName, message);
			AfxMessageBox(error, MB_ICONWARNING);
			return;
		}

		// Get Triangles
		ModelData model;
		model.VertexIterations = 7;
		model.TriangleSeeds = SeedTriangles::Eight;

		std::shared_ptr<TriangleData> triangles = TriangleLoader::CreateTriangles(model, [&](double /*progress*/) {});

		CArchive ar(&triFile, CArchive::store);

		Serialize(ar, *triangles);

		ar.Close();
		triFile.Close();

		AfxMessageBox(_T("Done exporting"));
	}

protected:

	void SetRotationTimer()
	{
		if (m_rotationParams.Action == RotationAction::Fixed)
		{
			if (m_timerId != 0)
			{
				if (m_pThis->KillTimer(m_timerId))
					m_timerId = 0;
			}
		}
		else
		{
			if (m_timerId == 0)
			{
				m_timerId = m_pThis->SetTimer(1, 200, nullptr);
			}
		}
	}

	void CalculateModel()
	{
		if (m_calcThread)
			m_calcThread.reset();

		auto pDoc = m_pThis->GetDocument();
		if (!pDoc)
			return;

		m_progressDlg = make_unique<CProgressDlg>(m_pThis);
		m_progressDlg->Create(IDD_PROGRESS_DLG, NULL);
		m_progressDlg->SetCancelEvent(m_cancelEvent);

		// start calculation
		promise<shared_ptr<DxVertexData>> promisedVertexData;
		m_futureVertexData = promisedVertexData.get_future();

		auto hnd = m_pThis->GetSafeHwnd();

		m_calcThread = make_unique<thread>(CalculateModelFunc, move(promisedVertexData), pDoc->GetModelData(), pDoc->GetTraceParams(), 
			hnd, [hnd, this](double progress)
		{
			if (m_cancelEvent->Canceled)
			{
				m_cancelEvent->Canceled = false;
				throw CCancelException("Calculation Canceled");
			}
			
			WPARAM prog = cMessage::MakeWPARAMProgress(progress);
			CWnd* pWnd = CWnd::FromHandle(hnd);

			pWnd->PostMessage(cMessage::tm_updateprogress, prog, 0);
		});
	}

	void SetNewPalette(const DxColor::PinPalette& pinPalette, const DxColor::ColorContrast& contrast)
	{
		auto pDoc = m_pThis->GetDocument();
		if (!pDoc)
			return;

		DxColor::PinPalette palette = pinPalette;
		DxColor::ValidatePalette(palette);
		pDoc->SetContrast(contrast);
		m_colors = fx::ColorUtilities::CalculatePaletteColors(palette, m_numberOfColors, pDoc->GetContrast());
		pDoc->SetPalette(palette);
		m_renderer->SetTextureColors(m_colors);
		RenderIfFixed();
	}

private:

	CFractalXView * m_pThis;

	shared_ptr<Renderer> m_renderer = Renderer::CreateRenderer();
	UINT_PTR m_timerId = 0;
	RotationParams m_rotationParams;
	unique_ptr<CProgressDlg> m_progressDlg;
	unique_ptr<thread> m_calcThread;
	future<shared_ptr<DxVertexData>> m_futureVertexData;
	std::shared_ptr<CancelEvent> m_cancelEvent;
	int m_numberOfColors = 10000;
	std::vector<uint32_t> m_colors;
	std::shared_ptr<CPaletteViewDlg> m_paletteViewDlg;
};

IMPLEMENT_DYNCREATE(CFractalXView, CView)

BEGIN_MESSAGE_MAP(CFractalXView, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CFractalXView::OnFilePrintPreview)
	ON_COMMAND(ID_EDIT_ROTATION, &CFractalXView::OnChangeRotation)
	ON_COMMAND(ID_EDIT_SETCOLORS, &CFractalXView::OnColors)
	ON_COMMAND(ID_EDIT_SETPERSPECTIVE, &CFractalXView::OnEditPerspective)
	ON_COMMAND(ID_EDIT_SETVIEW, &CFractalXView::OnEditView)
	ON_COMMAND(ID_EDIT_SETMODEL, &CFractalXView::OnEditSetModel)
	ON_COMMAND(ID_EDIT_PALETTE, &CFractalXView::OnEditPalette)
	ON_COMMAND(ID_EXPORTTRIANGLES, &CFractalXView::OnExportTriangles)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_UPDATE_COMMAND_UI(ID_EDIT_ROTATION, &CFractalXView::OnUpdateEditRotation)
	ON_UPDATE_COMMAND_UI(ID_EDIT_SETCOLORS, &CFractalXView::OnUpdateEditSetcolors)
	ON_UPDATE_COMMAND_UI(ID_EDIT_SETPERSPECTIVE, &CFractalXView::OnUpdateEditSetPerspective)
	ON_UPDATE_COMMAND_UI(ID_EDIT_SETVIEW, &CFractalXView::OnUpdateEditSetView)
	ON_UPDATE_COMMAND_UI(ID_EDIT_SETMODEL, &CFractalXView::OnUpdateEditSetModel)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PALETTE, &CFractalXView::OnUpdateEditPalette)
	ON_REGISTERED_MESSAGE(cMessage::tm_updateprogress, &CFractalXView::OnUpdateProgress)
	ON_REGISTERED_MESSAGE(cMessage::tm_finish, &CFractalXView::OnCalcFinished)
	ON_REGISTERED_MESSAGE(cMessage::tm_render, &CFractalXView::OnRender)
	ON_REGISTERED_MESSAGE(cMessage::tm_killPaletteView, &CFractalXView::OnKillPaletteView)
END_MESSAGE_MAP()

CFractalXView::CFractalXView()
	: m_pImp(new CFractalXViewImp(this))
{}

CFractalXView::~CFractalXView()
{}

BOOL CFractalXView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CFractalXView drawing

void CFractalXView::OnDraw(CDC* /*pDC*/)
{
	CFractalXDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
}


// CFractalXView printing

void CFractalXView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CFractalXView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CFractalXView diagnostics

#ifdef _DEBUG
void CFractalXView::AssertValid() const
{
	CView::AssertValid();
}

void CFractalXView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CFractalXDoc* CFractalXView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CFractalXDoc)));
	return (CFractalXDoc*)m_pDocument;
}
#endif //_DEBUG


// CFractalXView message handlers

void CFractalXView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	auto hnd = GetSafeHwnd();
	if (!hnd)
		return;

	CRect clientRect;
	GetClientRect(clientRect);

	auto size = clientRect.Size();

	m_pImp->Initialize(hnd, size);
}

void CFractalXView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	m_pImp->ResizeRenderer(cx, cy);
}

void CFractalXView::OnTimer(UINT_PTR nIDEvent)
{
	m_pImp->UpdateRotation();

	CView::OnTimer(nIDEvent);
}

void CFractalXView::OnChangeRotation()
{
	CRotationDlg rotationDlg;
	rotationDlg.SetRotationParams(m_pImp->GetRotationParams());

	if (rotationDlg.DoModal() == IDOK)
	{
		m_pImp->RunRotationParams(rotationDlg.GetRotationParams());
	}
}

void CFractalXView::OnColors()
{
	m_pImp->OnColors();
}

void CFractalXView::OnEditPerspective()
{
	m_pImp->OnEditPerspective();
}

void CFractalXView::OnEditView()
{
	m_pImp->OnEditView();
}

void CFractalXView::OnEditSetModel()
{
	m_pImp->OnEditSetModel();
}

void CFractalXView::OnUpdateEditRotation(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_pImp->CanRender());
}

void CFractalXView::OnUpdateEditSetcolors(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_pImp->CanRender());
}

void CFractalXView::OnUpdateEditSetPerspective(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_pImp->CanRender());
}

void CFractalXView::OnUpdateEditSetView(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_pImp->CanRender());
}

void CFractalXView::OnUpdateEditSetModel(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_pImp->CanRender());
}

void CFractalXView::OnEditPalette()
{
	m_pImp->OnEditPalette();
}

void CFractalXView::OnUpdateEditPalette(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_pImp->CanRender());
}

LRESULT CFractalXView::OnKillPaletteView(WPARAM, LPARAM)
{
	m_pImp->OnKillPaletteView();
	return 0;
}

LRESULT CFractalXView::OnUpdateProgress(WPARAM wparam, LPARAM)
{
	return m_pImp->OnUpdateProgress(wparam);
}

LRESULT CFractalXView::OnCalcFinished(WPARAM wparam, LPARAM)
{
	auto result = m_pImp->OnCalcFinished(wparam);

	// Need to force draw if fixed (no rotation therefore no timer)
	PostMessage(cMessage::tm_render);

	return result;
}

LRESULT CFractalXView::OnRender(WPARAM, LPARAM)
{
	return m_pImp->OnRender();
}

void CFractalXView::OnExportTriangles()
{
	m_pImp->OnExportTriangles();
}