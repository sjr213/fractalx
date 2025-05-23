
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

#include "BackgroundDlg.h"
#include "BackgroundVertexData.h"
#include "BackgroundModelFactory.h"
#include "CancelException.h"
#include "ChildFrm.h"
#include "ColorContrast.h"
#include "ColorUtilities.h"
#include "ColorUtils.h"
#include "ConversionUtil.h"
#include "CustomeHints.h"
#include "DefaultFields.h"
#include "RendererDx12.h"
#include "DxfColorFactory.h"
#include "EffectColors.h"
#include "FractalXDoc.h"
#include "FractalXView.h"
#include "ImageSizeDlg.h"
#include "Light.h"
#include "MainFrm.h"
#include "Messages.h"
#include "ModelData.h"
#include "ModelSheet.h"
#include "Palettes.h"
#include "PaletteViewDlg.h"
#include "PaletteSelectionDlg.h"
#include "PositionAngleDlg.h"
#include "ProgressCancelSignaling.h"
#include "ProgressDlg.h"
#include "Renderer.h"
#include "RendererDx11.h"
#include "RotationParams.h"
#include "RotationDlg.h"
#include "StringUtil.h"
#include "TextureDlg.h"
#include "TraceParams.h"
#include "TriangleLoader.h"
#include "vertexFactory.h"
#include "ViewUtil.h"
#include "WorldScaleDlg.h"

using namespace DXF;
using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace
{
	// move calc file
	void CalculateModelFunc(promise<shared_ptr<DxVertexData>> result_promise, ModelData modelData, TraceParams traceParams,
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
			pFrame->MoveWindow(windRect.left - 2, windRect.top - 50, size.cx, size.cy);
		}	
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

		if (!m_renderer)
			return;

		m_renderer->SetTextureColors(m_colors);
	}

	void Initialize(bool updateWindow)
	{
		auto pDoc = m_pThis->GetDocument();
		if (!pDoc)
			return;

		CSize size = pDoc->GetSize();

		auto hnd = m_pThis->GetSafeHwnd();
		if (!hnd)
			return;

		if (updateWindow)
			InitializeWindow(size);

		// new stuff
		const DXF::ModelData& modelData = pDoc->GetModelData();
		if (modelData.DirectXVersion == DxVersion::Dx11)
			InitializeDx11(*pDoc, hnd, size);
		else if (modelData.DirectXVersion == DxVersion::Dx12)
			InitializeDx12(*pDoc);
	}

	void InitializeDx11(CFractalXDoc& doc, HWND hWnd, const CSize& size)
	{
		m_dxVersion = DxVersion::Dx11;
		m_renderer = DXF::CreateRendererDx11();

		// SEE IF WE CAN GROUP THESE
		m_renderer->Initialize(hWnd, size.cx, size.cy);
		InitializeColors();

		m_renderer->SetView(doc.GetCamera(), doc.GetTarget(), doc.GetTargetBackground());
		m_renderer->SetPerspective(doc.GetPerspective());

		RunRotationParams(doc.GetRotationGroup());

		m_renderer->SetBackgroundColor(ColorUtils::ConvertToDxColor(doc.GetBackgroundColor()));
		m_renderer->SetEffectColors(ConversionUtil::ConvertEffectColors(doc.GetEffectColors()));
		m_renderer->SetLights(ConversionUtil::ConvertLights(doc.GetLights()));
		m_renderer->SetWorldScale(doc.GetWorldScale());
		m_renderer->SetBackgroundScale(doc.GetBackgroundScale());

		m_renderer->SetModel(*doc.GetVertexData());
		m_renderer->SetModel2(*doc.GetBackgroundVertexData());
	}

	void InitializeDx12(CFractalXDoc& doc)
	{
		m_dxVersion = DxVersion::Dx12;
		m_renderer = DXF::CreateRendererDx12();

		InitializeColors();

		m_renderer->SetView(doc.GetCamera(), doc.GetTarget(), doc.GetTargetBackground());
		m_renderer->SetPerspective(doc.GetPerspective());

		RunRotationParams(doc.GetRotationGroup());

		m_renderer->SetBackgroundColor(ColorUtils::ConvertToDxColor(doc.GetBackgroundColor()));
		m_renderer->SetEffectColors(ConversionUtil::ConvertEffectColors(doc.GetEffectColors()));
		m_renderer->SetLights(ConversionUtil::ConvertLights(doc.GetLights()));
		m_renderer->SetWorldScale(doc.GetWorldScale());
		m_renderer->SetBackgroundScale(doc.GetBackgroundScale());

		auto model = doc.GetVertexData();
		m_renderer->SetModel(*model);

		if (! model->Indices.empty() || !model->Vertices.empty())
			FinishedDx12Initialization(doc);
	}

	void FinishedDx12Initialization(CFractalXDoc& doc)
	{
		CSize size = doc.GetSize();

		auto hnd = m_pThis->GetSafeHwnd();
		if (!hnd)
			return;

		if (!m_renderer)
			return;

		m_renderer->Initialize(hnd, size.cx, size.cy);
	}

	void ResizeRenderer(int cx, int cy)
	{
		// We don't want to change the size if the frame is being resized
		CChildFrame* pFrame = dynamic_cast<CChildFrame*>(m_pThis->GetParentFrame());
		if (pFrame == nullptr)
			return;

		if (pFrame->IsLeftMouseDown())
			return;

		if (m_renderer)
			m_renderer->OnWindowSizeChanged(cx, cy);
	}

	void ForceModelRender()
	{
		if (!m_renderer)
			return;

		auto pDoc = m_pThis->GetDocument();
		auto rg = pDoc->GetRotationGroup();
		if (rg.RotationParamsMain.Action == RotationAction::Fixed)
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
		auto pDoc = m_pThis->GetDocument();
		auto rg = pDoc->GetRotationGroup();

		if (!m_renderer)
			return;

		if (rg.RotationParamsMain.Action == RotationAction::Fixed)
		{
			m_renderer->RefreshRender(0);
		}
	}

	void RunRotationParams(RotationGroup rg)
	{
		if (!m_renderer)
			return;

		m_renderer->SetRotationGroup(rg);

		SetRotationTimer();
		ForceModelRender();
	}

	void AutoZoom()
	{
		bool enableAutoZoom = m_topLeft.has_value() && m_bottomRight.has_value();
		if (!enableAutoZoom)
			return;

		if (!m_renderer)
			return;

		auto screenSize = m_renderer->GetScreenSize();

		// Get current zoom
		auto pDoc = m_pThis->GetDocument();
		if (!pDoc)
			return;
		auto worldScale = pDoc->GetWorldScale();

		auto camera = pDoc->GetCamera();

		// calculate new x & y
		int newWidth = m_bottomRight->x - m_topLeft->x;
		int newHeight = m_bottomRight->y - m_topLeft->y;
		if (newWidth <= 0 || newHeight <= 0)
			return;

		float newScaleX = worldScale.X * static_cast<float>(screenSize.cx) / static_cast<float>(newWidth);
		float newScaleY = worldScale.Y * static_cast<float>(screenSize.cy) / static_cast<float>(newHeight);
		float newScale = (newScaleX + newScaleY) / 2.0f;

		float deltaZscale = newScale - worldScale.Z;

		// set
		worldScale.X = newScale;
		worldScale.Y = newScale;
		worldScale.Z = newScale;

		pDoc->SetWorldScale(worldScale);

		camera.Z += deltaZscale;
		pDoc->SetCamera(camera);
		m_renderer->SetView(camera, pDoc->GetTarget(), pDoc->GetTargetBackground());
	}

	void AutoTranslate(Vertex<float> oldWorldScale)
	{
		bool enableAutoTranslate = m_topLeft.has_value() && m_bottomRight.has_value();
		if (!enableAutoTranslate)
			return;

		if (!m_renderer)
			return;

		auto screenSize = m_renderer->GetScreenSize();

		// Get current zoom
		auto pDoc = m_pThis->GetDocument();
		if (!pDoc)
			return;
		auto target = pDoc->GetTarget();
		auto worldScale = pDoc->GetWorldScale();

		// calculate new x & y
		int newWidth = m_bottomRight->x - m_topLeft->x;
		int newHeight = m_bottomRight->y - m_topLeft->y;
		if (newWidth <= 0 || newHeight <= 0)
			return;

		const double TransFactor = 0.085;

		// Translate X
		int centerXold = screenSize.cx / 2;
		int centerXnew = m_topLeft->x + (m_bottomRight->x - m_topLeft->x)/2;
		double deltaX = (static_cast<double>(centerXold) - centerXnew)/static_cast<double>(centerXold);
		double scaleFactorX = worldScale.X / oldWorldScale.X;
		double translateX = scaleFactorX * (TransFactor *  deltaX +  m_translateX);
		m_translateX = translateX;
		target.X = static_cast<float>(m_translateX);

		// Translate Y
		int centerYold = screenSize.cy / 2;
		int centerYnew = m_topLeft->y + (m_bottomRight->y - m_topLeft->y) / 2;
		double deltaY = (static_cast<double>(centerYnew) - centerYold) / static_cast<double>(centerYold);
		double scaleFactorY = worldScale.Y / oldWorldScale.Y;
		double translateY = scaleFactorY * (TransFactor * deltaY +  m_translateY);
		m_translateY = translateY;
		target.Y = static_cast<float>(m_translateY);

		pDoc->SetTarget(target);
		m_renderer->SetTarget(target);
	}

	void ResetRenderer(const ModelData& oldModelData, const ModelData& newModelData)
	{
		if (oldModelData.DirectXVersion == newModelData.DirectXVersion &&
			newModelData.DirectXVersion == DxVersion::Dx11)
			return;
	
		m_renderer.reset();

		Initialize(false);
	}

	void OnEditSetModel()
	{
		auto pDoc = m_pThis->GetDocument();
		if (!pDoc)
			return;

		if (!m_renderer)
			return;

		std::unique_ptr<CModelSheet> modelSheet = std::make_unique<CModelSheet>(_T("Model"), m_pThis);
		modelSheet->SetModelData(pDoc->GetModelData());
		modelSheet->SetTraceParams(pDoc->GetTraceParams());

		if (modelSheet->DoModal() == IDOK)
		{
			auto oldModelData = pDoc->GetModelData();
			auto newModelData = modelSheet->GetModelData();			
			pDoc->SetModelData(newModelData);
			ResetRenderer(oldModelData, newModelData);
		
			pDoc->SetTraceParams(modelSheet->GetTraceParams());

			if (modelSheet->GetDefaultTargetAndWorkScale())
			{
				ResetTranslationValues();
				pDoc->SetWorldScale(GetDefaultWorldScale());
				pDoc->SetTarget(GetDefaultTarget());
				m_renderer->SetTarget(GetDefaultTarget());
			}

			auto worldScaleOriginal = pDoc->GetWorldScale();
			if (modelSheet->GetAutoZoom())			
				AutoZoom();		

			if (modelSheet->GetAutoTranslate())	
				AutoTranslate(worldScaleOriginal);

			CalculateModel();
		}
	}

	void OnEditSetBackground()
	{
		auto pDoc = m_pThis->GetDocument();
		if (!pDoc)
			return;

		if (!m_renderer)
			return;

		// We need filename, showbackground and model vertexes DXF::Vertex<float>>
	//	auto backgndData = *pDoc->GetBackgroundVertexData();
		auto traceParams = std::make_shared<TraceParams>(*pDoc->GetTraceParams());
		auto filename = traceParams->Background.ImageFilename;
		auto showBkgnd = traceParams->Background.ShowBackgroundModel;
		auto bkgndModel = traceParams->Background.BackgroundModel;

		auto bkgndDlg = CBackgroundDlg::CreateBackgroundDlg(m_pThis);
		bkgndDlg->SetFilename(filename);
		bkgndDlg->SetShowBackground(showBkgnd);
		bkgndDlg->SetModelVertices(bkgndModel);

		if (bkgndDlg->DoModal() == IDOK)
		{
			traceParams->Background.BackgroundModel = bkgndDlg->GetModelVertices();
			traceParams->Background.ImageFilename = bkgndDlg->GetFilename();
			traceParams->Background.ShowBackgroundModel = bkgndDlg->GetShowBackground();

			auto vData2 = DXF::BackgroundModelFactory::CreateModel(bkgndDlg->GetModelVertices());
			std::wstring textureFilename = traceParams->Background.ImageFilename.c_str();
			bool show = traceParams->Background.ShowBackgroundModel;
			auto bkgndVertexData = std::make_shared<DxBackgroundVertexData>(show, *vData2, textureFilename);
			pDoc->SetBackgroundVertexData(bkgndVertexData);
			pDoc->SetTraceParams(traceParams);
			m_renderer->SetModel2(*bkgndVertexData);
			ForceModelRender();
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

	void OnEditSelectPalette()
	{
		auto pDoc = m_pThis->GetDocument();
		if (!pDoc)
			return;

		auto pPaletteDlg = CPaletteSelectionDlg::CreatePaletteSelectionDlg(pDoc->GetContrast(), m_pThis);
		pPaletteDlg->SetNewPaletteMethod([&](const DxColor::PinPalette& palette, const DxColor::ColorContrast& contrast)
			{
				SetNewPalette(palette, contrast);
			});

		if (pPaletteDlg->DoModal() == IDOK)
		{
			auto palette = pPaletteDlg->GetSelectedPalette();
			if (!palette)
				return;

			SetNewPalette(*palette, pPaletteDlg->GetContrast());
		}
	}

	void OnKillPaletteView()
	{
		if (! m_paletteViewDlg)
			return;

		m_paletteViewDlg.reset();
	}

	void OnEditAnglePosition()
	{
		if (m_positionAngleDlg)
			return;

		auto pDoc = m_pThis->GetDocument();

		auto target = pDoc->GetTarget();
		auto targetBackgnd = pDoc->GetTargetBackground();
	
		m_positionAngleDlg = CPositionAngleDlg::CreatePositionAngleDlg(pDoc->GetRotationGroup(), target, targetBackgnd, m_pThis);

		m_positionAngleDlg->Create(IDD_POSITION_ANGLE_DLG, NULL);
	}

	void OnKillAnglePosition(WPARAM result)
	{
		if (!m_positionAngleDlg)
			return;

		if (!m_renderer)
			return;

		if (result == cMessage::DlgCancel)
		{		
			auto target = m_positionAngleDlg->GetTarget();	
			auto targetBackgnd = m_positionAngleDlg->GetTargetBackground();

			auto pDoc = m_pThis->GetDocument();
			pDoc->SetTarget(target);
			pDoc->SetTargetBackground(targetBackgnd);
			m_renderer->SetTarget(target);
			m_renderer->SetTargetBackground(targetBackgnd);
		
			auto rg = pDoc->GetRotationGroup();
			if (rg.RotationParamsMain.Action == RotationAction::Fixed)
			{
				rg = *m_positionAngleDlg->GetRotationGroup();
				pDoc->SetRotationGroup(rg);
				m_renderer->SetRotationGroup(rg);

				SetRotationTimer();
				ForceModelRender();
			}
		}

		m_positionAngleDlg.reset();
	}

	void OnStopRotation()
	{
		if (!m_renderer)
			return;

		auto pDoc = m_pThis->GetDocument();
		auto rg = m_renderer->GetRotationGroup();

		if (rg.RotationParamsMain.Action != RotationAction::Fixed)
		{
			rg.RotationParamsMain.Action = RotationAction::Fixed;
			pDoc->SetRotationGroup(rg);
			m_renderer->SetRotationGroup(rg);

			SetRotationTimer();
			ForceModelRender();
		}
	}

	bool IsRotating()
	{
		if (!m_renderer)
			return false;

		auto rg = m_renderer->GetRotationGroup();
		return rg.RotationParamsMain.Action != RotationAction::Fixed;
	}

	void OnModelPositionChanged()
	{
		if (!m_positionAngleDlg)
			return;

		if (!m_renderer)
			return;

		auto target = m_positionAngleDlg->GetTarget();
		auto targetBackgnd = m_positionAngleDlg->GetTargetBackground();

		auto pDoc = m_pThis->GetDocument();
		pDoc->SetTarget(target);
		pDoc->SetTargetBackground(targetBackgnd);
		m_renderer->SetTarget(target);
		m_renderer->SetTargetBackground(targetBackgnd);

		ForceModelRender();
	}

	void OnModelAngleChanged()
	{
		if (!m_positionAngleDlg)
			return;

		if (!m_renderer)
			return;

		auto pDoc = m_pThis->GetDocument();
		auto rg = pDoc->GetRotationGroup();
		if (rg.RotationParamsMain.Action != RotationAction::Fixed)
			return;

		rg = *m_positionAngleDlg->GetRotationGroup();
		pDoc->SetRotationGroup(rg);
		m_renderer->SetRotationGroup(rg);

		ForceModelRender();
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

	bool IsRendererReady()
	{
		if (!m_renderer)
			return false;

		if (m_renderer->IsReady())
			return true;

		auto pDoc = m_pThis->GetDocument();
		if (!pDoc)
			return false;

		CSize size = pDoc->GetSize();
		auto hnd = m_pThis->GetSafeHwnd();
		if (!hnd)
			return false;

		m_renderer->OnDeviceLost();

		return m_renderer->IsReady();
	}

	LRESULT OnCalcFinished(WPARAM wparam)
	{
		m_progressDlg.reset();

		auto pDoc = m_pThis->GetDocument();
		if (!pDoc)
			return 0;

		if (wparam == cMessage::CalcFinished)
		{
			// update the model
			auto vertexData = m_futureVertexData.get();

			if (vertexData)
			{
				if (!IsRendererReady())
					return 0;

				// a little risky since we're in another thread
				// consider locking data
				pDoc->SetVertexData(vertexData);

				m_renderer->SetWorldScale(pDoc->GetWorldScale());

				m_renderer->SetModel(*vertexData);

				std::shared_ptr<DXF::TraceParams> traceParams = std::make_shared<TraceParams>();
				*traceParams = *pDoc->GetTraceParams();

				// Model 2
				auto vData2 = DXF::BackgroundModelFactory::CreateModel(traceParams->Background.BackgroundModel);
				std::wstring textureFilename = traceParams->Background.ImageFilename.c_str();
				bool show = traceParams->Background.ShowBackgroundModel;
				auto bkgndVertexData = std::make_shared<DxBackgroundVertexData>(show, *vData2, textureFilename);
				pDoc->SetBackgroundVertexData(bkgndVertexData);
				m_renderer->SetModel2(*bkgndVertexData);

				traceParams->Stretch = vertexData->StretchParams;
				pDoc->SetTraceParams(traceParams);	
			}
		}

		m_calcThread->join();

		m_calcThread.reset();

		ResetResizePoints();

		if (wparam == cMessage::CalcFinished && m_dxVersion == DxVersion::Dx12)
			FinishedDx12Initialization(*pDoc);

		return 0;
	}

	LRESULT OnRender()
	{
		auto pDoc = m_pThis->GetDocument();
		auto rg = pDoc->GetRotationGroup();

		// Only need to render here if fixed because there will be no timer
		if (rg.RotationParamsMain.Action == RotationAction::Fixed)
		{
			if(m_renderer)
				m_renderer->RefreshRender(0);
		}

		return 0;
	}

	Renderer* GetRenderer()
	{
		return m_renderer.get();
	}

	bool IsInitialized() const 
	{
		return 	m_isInitialized;
	}

	void SetInitialized(bool init)
	{
		m_isInitialized = init;
	}

	optional<DXF::Vertex<float>> GetVertex(CPoint point)
	{
		if (!m_renderer)
			return nullopt;

		return m_renderer->Map2Dto3D(point.x, point.y);
	}

	std::optional<DXF::Vertex<float>> GetFirstVertex()
	{
		return m_firstVertex;
	}

	void SetFirstVertex(const std::optional<DXF::Vertex<float>>& firstVertex)
	{
		m_firstVertex = firstVertex;
	}

	std::optional<DXF::Vertex<float>> GetLastVertex()
	{
		return m_lastVertex;
	}

	void SetLastVertex(const std::optional<DXF::Vertex<float>>& lastVertex)
	{
		m_lastVertex = lastVertex;
	}

	void SaveVertices(CPoint pt1, CPoint pt2) 
	{ 
		if (!m_renderer)
			return;

		int left = min<int>(pt1.x, pt2.x);
		int right = max<int>(pt1.x, pt2.x);
		int top = min<int>(pt1.y, pt2.y);
		int bottom = max<int>(pt1.y, pt2.y);

		if (left == right || top == bottom)
		{
			AfxMessageBox(_T("Can't save vertices because they don't form a rectangle!"));
			return;
		}

		auto vertexTL = m_renderer->Map2Dto3D(left, top);
		if (!vertexTL.has_value())
		{
			AfxMessageBox(_T("Can't save vertices because top left vertex is outside the model!"));
			return;
		}

		auto vertexTR = m_renderer->Map2Dto3D(right, top);
		if (!vertexTR.has_value())
		{
			AfxMessageBox(_T("Can't save vertices because top right vertex is outside the model!"));
			return;
		}

		auto vertexBL = m_renderer->Map2Dto3D(left, bottom);
		if (!vertexBL.has_value())
		{
			AfxMessageBox(_T("Can't save vertices because bottom left vertex is outside the model!"));
			return;
		}

		auto vertexBR = m_renderer->Map2Dto3D(right, bottom);
		if (!vertexBR.has_value())
		{
			AfxMessageBox(_T("Can't save vertices because bottom right vertex is outside the model!"));
			return;
		}

		Vertex<float> vTL = NormalizeVector(vertexTL.value());
		Vertex<float> vTR = NormalizeVector(vertexTR.value());
		Vertex<float> vBL = NormalizeVector(vertexBL.value());
		Vertex<float> vBR = NormalizeVector(vertexBR.value());

		auto pDoc = m_pThis->GetDocument();
		if (!pDoc)
			return;

		m_topLeft = CPoint(left, top);
		m_bottomRight = CPoint(right, bottom);

		auto md= pDoc->GetModelData();
		md.VertexTL = vTL;
		md.VertexTR = vTR;
		md.VertexBL = vBL;
		md.VertexBR = vBR;

		pDoc->SetModelData(md);
	}

	void ResetResizePoints()
	{
		m_topLeft.reset();
		m_bottomRight.reset();
	}

	void ResetTranslationValues()
	{
		m_translateX = 0.0;
		m_translateY = 0.0;
	}

	DXF::DxVersion DxVersion()
	{ return m_dxVersion; }

protected:

	void SetRotationTimer()
	{
		auto pDoc = m_pThis->GetDocument();
		auto rg = pDoc->GetRotationGroup();
		if (rg.RotationParamsMain.Action == RotationAction::Fixed)
		{
			if (m_timerId != 0)
			{
				if (m_pThis->KillTimer(m_timerId))
					m_timerId = 0;
			}
		}
		else
		{
			if (m_timerId != 0)
			{
				if (m_pThis->KillTimer(m_timerId))
					m_timerId = 0;
			}

			if (m_timerId == 0)
			{
				int speed = GetRotationSpeedMsec(rg.RotationParamsMain.Speed);
				m_timerId = m_pThis->SetTimer(1, speed, nullptr);
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

		m_calcThread = make_unique<thread>(CalculateModelFunc, move(promisedVertexData), pDoc->GetModelData(), *pDoc->GetTraceParams(), 
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

		if (!m_renderer)
			return;

		DxColor::PinPalette palette = pinPalette;
		DxColor::ValidatePalette(palette);
		pDoc->SetContrast(contrast);
		m_colors = fx::ColorUtilities::CalculatePaletteColors(palette, m_numberOfColors, pDoc->GetContrast());
		pDoc->SetPalette(palette);
		m_renderer->SetTextureColors(m_colors);
		m_renderer->ReloadTextures();
		RenderIfFixed();
	}

public:

	CPoint m_firstPoint;
	CPoint m_lastPoint;
	CPen m_whitePen;
	CPen m_blackDashPen;
	bool m_drawRect = false;

protected:

	CFractalXView * m_pThis;

	DXF::DxVersion m_dxVersion = DXF::DxVersion::Dx11;
	unique_ptr<Renderer> m_renderer;
	UINT_PTR m_timerId = 0;
	unique_ptr<CProgressDlg> m_progressDlg;
	unique_ptr<thread> m_calcThread;
	future<shared_ptr<DxVertexData>> m_futureVertexData;
	std::shared_ptr<CancelEvent> m_cancelEvent;
	int m_numberOfColors = 10000;
	std::vector<uint32_t> m_colors;
	std::shared_ptr<CPaletteViewDlg> m_paletteViewDlg;
	std::shared_ptr<CPositionAngleDlg> m_positionAngleDlg;
	bool m_isInitialized = false;
	std::optional<DXF::Vertex<float>> m_firstVertex;
	std::optional<DXF::Vertex<float>> m_lastVertex;
	std::optional<CPoint> m_topLeft;
	std::optional<CPoint> m_bottomRight;
	double m_translateX = 0.0;
	double m_translateY = 0.0;
};

IMPLEMENT_DYNCREATE(CFractalXView, CView)

BEGIN_MESSAGE_MAP(CFractalXView, CView)
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_WM_RBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_CONTEXTMENU()
	ON_WM_PAINT()

	ON_COMMAND(ID_EDIT_SETMODEL, &CFractalXView::OnEditSetModel)
	ON_UPDATE_COMMAND_UI(ID_EDIT_SETMODEL, &CFractalXView::OnUpdateEditSetModel)
	ON_COMMAND(ID_EDIT_SET_BACKGROUND, &CFractalXView::OnEditSetBackground)
	ON_UPDATE_COMMAND_UI(ID_EDIT_SET_BACKGROUND, &CFractalXView::OnUpdateEditSetBackground)
	ON_COMMAND(ID_EDIT_ANGLEPOSITION, &CFractalXView::OnEditAnglePosition)
	ON_UPDATE_COMMAND_UI(ID_EDIT_ANGLEPOSITION, &CFractalXView::OnUpdateEditAnglePosition)
	ON_COMMAND(ID_EDIT_LIGHTS, &CFractalXView::OnEditLights)
	ON_UPDATE_COMMAND_UI(ID_EDIT_LIGHTS, &CFractalXView::OnUpdateEditLights)
	ON_COMMAND(ID_EDIT_ROTATION, &CFractalXView::OnEditUpdateRotation)
	ON_UPDATE_COMMAND_UI(ID_EDIT_ROTATION, &CFractalXView::OnUpdateEditRotation)
	ON_COMMAND(ID_EDIT_STOPROTATION, &CFractalXView::OnEditStopRotation)
	ON_UPDATE_COMMAND_UI(ID_EDIT_STOPROTATION, &CFractalXView::OnUpdateEditStopRotation)
	ON_COMMAND(ID_EDIT_SETPERSPECTIVE, &CFractalXView::OnEditPerspective)
	ON_UPDATE_COMMAND_UI(ID_EDIT_SETPERSPECTIVE, &CFractalXView::OnUpdateEditSetPerspective)
	ON_COMMAND(ID_EDIT_SETVIEW, &CFractalXView::OnEditView)
	ON_UPDATE_COMMAND_UI(ID_EDIT_SETVIEW, &CFractalXView::OnUpdateEditSetView)
	ON_COMMAND(ID_EDIT_BACKGROUND_COLOR, &CFractalXView::OnEditBackgroundColor)
	ON_UPDATE_COMMAND_UI(ID_EDIT_BACKGROUND_COLOR, &CFractalXView::OnUpdateEditBackgroundColor)
	ON_COMMAND(ID_EDIT_PALETTE, &CFractalXView::OnEditPalette)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PALETTE, &CFractalXView::OnUpdateEditPalette)
	ON_COMMAND(ID_EDIT_EFFECTCOLORS, &CFractalXView::OnEditEffectColors)
	ON_UPDATE_COMMAND_UI(ID_EDIT_EFFECTCOLORS, &CFractalXView::OnUpdateEditEffectColors)
	ON_COMMAND(ID_EDIT_SELECTPALETTE, &CFractalXView::OnEditSelectPalette)
	ON_UPDATE_COMMAND_UI(ID_EDIT_SELECTPALETTE, &CFractalXView::OnUpdateEditSelectPalette)
	ON_COMMAND(ID_EXPORTTRIANGLES, &CFractalXView::OnExportTriangles)
	ON_COMMAND(ID_EDIT_SETSELECTEDVERTICES, &CFractalXView::OnSaveSelectedVertices)
	ON_UPDATE_COMMAND_UI(ID_EDIT_SETSELECTEDVERTICES, &CFractalXView::OnUpdateSaveSelectedVertices)
	ON_COMMAND(ID_EDIT_SETWORLDSCALE, &CFractalXView::OnEditSetWorldScale)
	ON_UPDATE_COMMAND_UI(ID_EDIT_SETWORLDSCALE, &CFractalXView::OnUpdateEditSetWorldScale)
	
	ON_REGISTERED_MESSAGE(cMessage::tm_updateprogress, &CFractalXView::OnUpdateProgress)
	ON_REGISTERED_MESSAGE(cMessage::tm_finish, &CFractalXView::OnCalcFinished)
	ON_REGISTERED_MESSAGE(cMessage::tm_render, &CFractalXView::OnRender)
	ON_REGISTERED_MESSAGE(cMessage::tm_killPaletteView, &CFractalXView::OnKillPaletteView)
	ON_REGISTERED_MESSAGE(cMessage::tm_killPositionAngleDlg, &CFractalXView::OnKillPositionAngleDlg)
	ON_REGISTERED_MESSAGE(cMessage::tm_modelPositionChanged, &CFractalXView::OnModelPositionChanged)
	ON_REGISTERED_MESSAGE(cMessage::tm_modelAngleChanged, &CFractalXView::OnModelAngleChanged)

END_MESSAGE_MAP()

CFractalXView::CFractalXView()
	: m_impl(new CFractalXViewImp(this))
{}

CFractalXView::~CFractalXView()
{}

/*
void CFractalXView::OnContextMenu(CWnd* pWnd, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}
*/

void CFractalXView::ShowContextMenu(CWnd* /* pWnd */, CPoint point)
{
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
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

void CFractalXView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	m_impl->Initialize(true);
	m_impl->m_whitePen.CreatePen(PS_SOLID, 4, RGB(255, 255, 255));
	m_impl->m_blackDashPen.CreatePen(PS_DOT, 2, RGB(0, 0, 0));
}

void CFractalXView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	if (m_impl->IsInitialized())
	{
		CSize size(cx, cy);
		GetDocument()->SetSize(size);
	}

	m_impl->ResizeRenderer(cx, cy);
}

void CFractalXView::OnTimer(UINT_PTR nIDEvent)
{
	m_impl->ForceModelRender();

	CView::OnTimer(nIDEvent);
}

void CFractalXView::OnRButtonUp(UINT nFlags, CPoint point)
{
	if (nFlags & MK_CONTROL)
	{
		int deltaX = point.x - m_impl->m_lastPoint.x;
		int deltaY = point.y - m_impl->m_lastPoint.y;

		auto renderer = m_impl->GetRenderer();
		if (renderer)
		{
			ViewUtil::MoveModel(*GetDocument(), *renderer, deltaX, -1 * deltaY);
			m_impl->ForceModelRender();
		}
		m_impl->m_lastPoint = point;
	}
	else
	{
		ClientToScreen(&point);
		ShowContextMenu(this, point);
	}

	ReleaseCapture();

	CView::OnRButtonUp(nFlags, point);
}

void CFractalXView::OnRButtonDown(UINT nFlags, CPoint point)
{
	m_impl->m_drawRect = false;
	Invalidate(TRUE);

	if (nFlags & MK_CONTROL)
	{
		SetCapture();
		m_impl->m_lastPoint = point;
	}

	CView::OnRButtonDown(nFlags, point);
}

void CFractalXView::OnMouseMove(UINT nFlags, CPoint point)
{
	m_impl->m_drawRect = false;

	if (nFlags & MK_CONTROL)
	{
		if(nFlags & MK_RBUTTON)
		{
			int deltaX = point.x - m_impl->m_lastPoint.x;
			int deltaY = point.y - m_impl->m_lastPoint.y;

			auto renderer = m_impl->GetRenderer();
			if (renderer)
			{
				ViewUtil::MoveModel(*GetDocument(), *renderer, deltaX, -1 * deltaY);
				m_impl->ForceModelRender();
			}

			m_impl->m_lastPoint = point;
		}
		else if (nFlags & MK_LBUTTON)
		{
			m_impl->m_lastPoint = point;
			m_impl->SetLastVertex(m_impl->GetVertex(point));

			CChildFrame* pViewFrame = (CChildFrame*)GetParentFrame();
			if (pViewFrame)
			{
				auto msg = StringUtil::MakeStatusMessage(m_impl->m_firstPoint, m_impl->m_lastPoint, m_impl->GetFirstVertex(), m_impl->GetLastVertex());
				pViewFrame->SetStatusText1(msg);
			}

			m_impl->m_drawRect = true;
			Invalidate(FALSE);
		}

	}

	CView::OnMouseMove(nFlags, point);
}

void CFractalXView::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_impl->m_drawRect = false;
	Invalidate(TRUE);

	if (nFlags & MK_CONTROL)
	{
		SetCapture();
		m_impl->m_firstPoint = point;
		m_impl->m_lastPoint = point;

		m_impl->SetFirstVertex(m_impl->GetVertex(point));
		m_impl->SetLastVertex(m_impl->GetFirstVertex());

		m_impl->m_drawRect = true;

		m_impl->ResetResizePoints();
	}

	CView::OnLButtonDown(nFlags, point);
}

void CFractalXView::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (nFlags & MK_CONTROL)
	{
		m_impl->m_lastPoint = point;
		m_impl->SetLastVertex(m_impl->GetVertex(point));

		CChildFrame* pViewFrame = (CChildFrame*)GetParentFrame();
		if (pViewFrame)
		{
			auto msg = StringUtil::MakeStatusMessage(m_impl->m_firstPoint, m_impl->m_lastPoint, m_impl->GetFirstVertex(), m_impl->GetLastVertex());
			pViewFrame->SetStatusText1(msg);
		}

		if (AfxMessageBox(_T("Do you want to save the new coordinates?"), MB_YESNO) == IDYES)
		{
			OnSaveSelectedVertices();
		}
	}

	ReleaseCapture();

	CView::OnLButtonUp(nFlags, point);
}

BOOL CFractalXView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	if (nFlags & MK_CONTROL)
	{
		// Negative value means the wheel is moving towards the user but this means moving +Z
		int distance = -1 * zDelta / WHEEL_DELTA;

		auto renderer = m_impl->GetRenderer();
		if (renderer)
		{
			ViewUtil::MoveModelAlongZAxis(*GetDocument(), *renderer, distance);
			m_impl->ForceModelRender();
		}
	}

	return CView::OnMouseWheel(nFlags, zDelta, pt);
}

void CFractalXView::OnEditSetModel()
{
	m_impl->OnEditSetModel();
}

void CFractalXView::OnUpdateEditSetModel(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_impl->GetRenderer() != nullptr);
}

void CFractalXView::OnEditSetBackground()
{
	m_impl->OnEditSetBackground();
}

void CFractalXView::OnUpdateEditSetBackground(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_impl->CanRender());
}

void CFractalXView::OnEditAnglePosition()
{
	m_impl->OnEditAnglePosition();
}

void CFractalXView::OnUpdateEditAnglePosition(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_impl->CanRender());
}

void CFractalXView::OnEditLights()
{
	if (m_impl->GetRenderer() && ViewUtil::EditLights(*GetDocument(), *m_impl->GetRenderer(), this))
		m_impl->ForceModelRender();
}

void CFractalXView::OnUpdateEditLights(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_impl->CanRender());
}

void CFractalXView::OnEditUpdateRotation()
{
	CRotationDlg rotationDlg;
	auto pDoc = GetDocument();

	rotationDlg.SetRotationGroup(pDoc->GetRotationGroup());

	if (rotationDlg.DoModal() == IDOK)
	{
		auto rg = rotationDlg.GetRotationGroup();
		pDoc->SetRotationGroup(rg);
		m_impl->RunRotationParams(rg);
	}
}

void CFractalXView::OnUpdateEditRotation(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_impl->CanRender());
}

void CFractalXView::OnEditStopRotation()
{
	m_impl->OnStopRotation();
}

void CFractalXView::OnUpdateEditStopRotation(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_impl->IsRotating());
}

void CFractalXView::OnEditPerspective()
{
	if (m_impl->GetRenderer() && ViewUtil::EditPerspective(*GetDocument(), *m_impl->GetRenderer()))
		m_impl->RenderIfFixed();
}

void CFractalXView::OnUpdateEditSetPerspective(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_impl->CanRender());
}

void CFractalXView::OnEditView()
{
	if (m_impl->GetRenderer() && ViewUtil::EditView(*GetDocument(), *m_impl->GetRenderer()))
		m_impl->RenderIfFixed();
}

void CFractalXView::OnUpdateEditSetView(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_impl->CanRender());
}

void CFractalXView::OnEditBackgroundColor()
{
	if (m_impl->GetRenderer() && ViewUtil::EditBackgroundColor(*GetDocument(), *m_impl->GetRenderer(), this))
		m_impl->ForceModelRender();
}

void CFractalXView::OnUpdateEditBackgroundColor(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_impl->CanRender());
}

void CFractalXView::OnEditPalette()
{
	m_impl->OnEditPalette();
}

void CFractalXView::OnUpdateEditPalette(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_impl->CanRender());
}

void CFractalXView::OnEditEffectColors()
{
	if (m_impl->GetRenderer() && ViewUtil::EditEffectColors(*GetDocument(), *m_impl->GetRenderer(), this))
		m_impl->ForceModelRender();
}

void CFractalXView::OnUpdateEditEffectColors(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_impl->CanRender());
}

void CFractalXView::OnEditSelectPalette()
{
	m_impl->OnEditSelectPalette();
}

void CFractalXView::OnUpdateEditSelectPalette(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_impl->CanRender());
}

void CFractalXView::OnExportTriangles()
{
	ViewUtil::OnExportTriangles();
}

LRESULT CFractalXView::OnKillPositionAngleDlg(WPARAM wparam, LPARAM)
{
	m_impl->OnKillAnglePosition(wparam);

	return 0;
}

LRESULT CFractalXView::OnModelPositionChanged(WPARAM, LPARAM)
{
	m_impl->OnModelPositionChanged();

	return 0;
}

LRESULT CFractalXView::OnModelAngleChanged(WPARAM, LPARAM)
{
	m_impl->OnModelAngleChanged();

	return 0;
}

LRESULT CFractalXView::OnKillPaletteView(WPARAM, LPARAM)
{
	m_impl->OnKillPaletteView();
	return 0;
}

LRESULT CFractalXView::OnCalcFinished(WPARAM wparam, LPARAM)
{
	auto result = m_impl->OnCalcFinished(wparam);

	// Need to force draw if fixed (no rotation therefore no timer)
	PostMessage(cMessage::tm_render);

	return result;
}

LRESULT CFractalXView::OnUpdateProgress(WPARAM wparam, LPARAM)
{
	return m_impl->OnUpdateProgress(wparam);
}

LRESULT CFractalXView::OnRender(WPARAM, LPARAM)
{
	return m_impl->OnRender();
}

static const GUID GetGdiplusImageFormat(const CString& ext)
{
	if (ext.CompareNoCase(_T("png")) == 0)
		return Gdiplus::ImageFormatPNG;

	if (ext.CompareNoCase(_T("jpg")) == 0)
		return Gdiplus::ImageFormatJPEG;

	return Gdiplus::ImageFormatBMP;
}

// ext should be "bmp", "jpg" or "png" 
void CFractalXView::SaveImage(const CString& filename, const CString& ext)
{
	CPaintDC dc(this);

	CDC dcBitmap;
	CBitmap bitmap;
	dcBitmap.CreateCompatibleDC(&dc);

	auto renderer = m_impl->GetRenderer();
	if (!renderer)
		return;

	CSize imageSize = renderer->GetScreenSize();

	auto imageSizeDlg = CImageSizeDlg::CreateImageSizeDlg(imageSize.cx, imageSize.cy, this);
	if (imageSizeDlg->DoModal() != IDOK)
		return;

	CSize newSize = imageSizeDlg->GetSize();

	bitmap.CreateCompatibleBitmap(&dc, newSize.cx, newSize.cy);
	dcBitmap.SelectObject(&bitmap);

	if (m_impl->DxVersion() == DxVersion::Dx11)
	{
		if (!renderer->DrawImage(dcBitmap, newSize))
		{
			AfxMessageBox(_T("Could not capture image!"), MB_OK);
			return;
		}
	}
	else  // dx 12
	{
//		dcBitmap.BitBlt(0, 0, imageSize.cx, imageSize.cy,
//			&dc, 0, 0, SRCCOPY);
	}

	const GUID imageFormat = GetGdiplusImageFormat(ext);
	CImage image;
	image.Attach(bitmap);
	image.Save(filename, imageFormat);
}

void CFractalXView::OnPaint()
{
	CPaintDC dc(this); 
	OnPrepareDC(&dc);

	OnDraw(&dc);		
}

void CFractalXView::OnDraw(CDC* pDC)
{
	m_impl->SetInitialized(true);
	m_impl->ForceModelRender();

	if (!m_impl->m_drawRect)
		return;

	CRect hatchedRect = ConversionUtil::MakeRect(m_impl->m_firstPoint, m_impl->m_lastPoint);

	CBrush* pOldBrush = (CBrush*)pDC->SelectStockObject(NULL_BRUSH);
	CPen* pOldPen = pDC->SelectObject(&m_impl->m_whitePen);

	pDC->Rectangle(hatchedRect);

	pDC->SelectObject(&m_impl->m_blackDashPen);
	pDC->Rectangle(hatchedRect);

	pDC->SelectObject(pOldPen);
	pDC->SelectObject(pOldBrush);
}

void CFractalXView::OnSaveSelectedVertices()
{
	m_impl->SaveVertices(m_impl->m_firstPoint, m_impl->m_lastPoint);
}

void CFractalXView::OnUpdateSaveSelectedVertices(CCmdUI* pCmdUI)
{
	auto firstVertex = m_impl->GetFirstVertex();
	auto lastVertex = m_impl->GetLastVertex();
	bool enable = firstVertex.has_value() && lastVertex.has_value();
	if (!enable)
	{
		pCmdUI->Enable(enable);
		return;
	}
	
	pCmdUI->Enable(firstVertex.value() != lastVertex.value());
}

void CFractalXView::OnEditSetWorldScale()
{
	auto renderer = m_impl->GetRenderer();
	if (!renderer)
		return;

	auto pDoc = GetDocument();
	auto worldScale = pDoc->GetWorldScale();
	auto backgndScale = pDoc->GetBackgroundScale();
	auto worldScaleDlg = CWorldScaleDlg::CreateWorldScaleDlg(worldScale, backgndScale, this);

	if (worldScaleDlg->DoModal() == IDOK)
	{
		worldScale = worldScaleDlg->GetWorldScale();
		backgndScale = worldScaleDlg->GetBackgroundScale();
		pDoc->SetWorldScale(worldScale);
		pDoc->SetBackgroundScale(backgndScale);
		renderer->SetWorldScale(worldScale);
		renderer->SetBackgroundScale(backgndScale);
		m_impl->ForceModelRender();

	}		
}

void CFractalXView::OnUpdateEditSetWorldScale(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_impl->CanRender());
}

void CFractalXView::OnUpdate(CView* /*sender*/, LPARAM lHint, CObject* /*pHint*/)
{
	if (lHint == FrameResizeDone)
	{
		CSize size = GetDocument()->GetSize();
		m_impl->ResizeRenderer(size.cx, size.cy);
	}
}