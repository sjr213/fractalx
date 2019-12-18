
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
#include <iosfwd>
#include <sstream>

#include "CancelException.h"
#include "ChildFrm.h"
#include "ColorContrast.h"
#include "ColorSelectorDlg.h"
#include "ColorUtilities.h"
#include "ColorUtils.h"
#include "DefaultFields.h"
#include "DxfColorFactory.h"
#include "DxEffectColors.h"
#include "DxLight.h"
#include "EffectColors.h"
#include "EffectColorDlg.h"
#include "FractalXDoc.h"
#include "FractalXView.h"
#include "ImageSizeDlg.h"
#include "Light.h"
#include "LightsDlg.h"
#include "MainFrm.h"
#include "Messages.h"
#include "ModelData.h"
#include "ModelSheet.h"
#include "Palettes.h"
#include "PaletteViewDlg.h"
#include "PaletteSelectionDlg.h"
#include "PerspectiveDlg.h"
#include "PositionAngleDlg.h"
#include "ProgressCancelSignaling.h"
#include "ProgressDlg.h"
#include "Renderer.h"
#include "RotationParams.h"
#include "RotationDlg.h"
#include "TextureDlg.h"
#include "TraceParams.h"
#include "TriangleLoader.h"
#include "Vertex.h"
#include "vertexFactory.h"
#include "ViewDlg.h"
#include "WorldScaleDlg.h"

// can remove after exporting triangles
#include "DxSerialize.h"


using namespace DXF;
using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace
{
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

	DXF::DxEffectColors ConvertEffectColors(const DxColor::EffectColors& effectColors)
	{
		DXF::DxEffectColors dxColors;
		dxColors.AmbientColor = ColorUtils::ConvertToDxColor(effectColors.Ambient);
		dxColors.DiffuseColor = ColorUtils::ConvertToDxColor(effectColors.Diffuse);
		dxColors.SpecularColor = ColorUtils::ConvertToDxColor(effectColors.Specular);
		dxColors.EmissiveColor = ColorUtils::ConvertToDxColor(effectColors.Emissive);
		dxColors.SpecularPower = effectColors.SpecularPower;

		return dxColors;
	}

	DXF::DxLight ConvertLight(const DxColor::Light& light)
	{
		DXF::DxLight dxLight;
		dxLight.Enable = light.Enable;
		dxLight.DiffuseColor = ColorUtils::ConvertToDxColor(light.Diffuse);
		dxLight.SpectacularColor = ColorUtils::ConvertToDxColor(light.Spectacular);
		dxLight.Direction = DirectX::SimpleMath::Vector3(std::get<0>(light.Direction), std::get<1>(light.Direction), std::get<2>(light.Direction));

		return dxLight;
	}

	DXF::DxLights ConvertLights(const DxColor::Lights& lights)
	{
		DXF::DxLights dxLights;
		dxLights.DefaultLights = lights.DefaultLights;
		dxLights.PerPixelLighting = lights.PerPixelLighting;
		dxLights.Alpha = lights.Alpha;

		dxLights.Lights.at(0) = ConvertLight(lights.Light1);
		dxLights.Lights.at(1) = ConvertLight(lights.Light2);
		dxLights.Lights.at(2) = ConvertLight(lights.Light3);

		return dxLights;
	}

	CString MakeStatusMessage(CPoint start, CPoint end, optional<DXF::Vertex<float>>& startVertex, optional<DXF::Vertex<float>>& endVertex)
	{
		wstringstream ss;
		ss << L'(' << start.x << L", " << start.y << L") ";
		if (startVertex.has_value())
		{
			ss << L'[' << startVertex->X << L", " << startVertex->Y << L", " << startVertex->Z << L"] ";
		}
		ss << L"to ";
		ss << L'(' << end.x << L", " << end.y << L')';
		if (endVertex.has_value())
		{
			ss << L'[' << endVertex->X << L", " << endVertex->Y << L", " << endVertex->Z << L"] ";
		}

		return ss.str().c_str();
	}

	CRect MakeRect(CPoint pt1, CPoint pt2)
	{
		int lf = std::min<int>(pt1.x, pt2.x);
		int rt = std::max<int>(pt1.x, pt2.x);
		int tp = std::min<int>(pt1.y, pt2.y);
		int bt = std::max<int>(pt1.y, pt2.y);

		return CRect(lf, tp, rt, bt);
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

	RotationParams GetRotationParams() const
	{
		return m_pThis->GetDocument()->GetRotationParams();
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
			pFrame->MoveWindow(windRect.left, windRect.top -50, size.cx, size.cy);
		}
			
	}

	void Initialize()
	{
		auto pDoc = m_pThis->GetDocument();
		if (!pDoc)
			return;

		CSize size = pDoc->GetSize();
		InitializeWindow(size);

		auto hnd = m_pThis->GetSafeHwnd();
		if (!hnd)
			return;

		m_renderer->Initialize(hnd, size.cx, size.cy);
		InitializeColors();

		m_renderer->SetView(pDoc->GetCamera(), pDoc->GetTarget());
		m_renderer->SetPerspective(pDoc->GetPerspective());

		RunRotationParams(pDoc->GetRotationParams());

		m_renderer->SetBackgroundColor(ColorUtils::ConvertToDxColor(pDoc->GetBackgroundColor()));
		m_renderer->SetEffectColors(ConvertEffectColors(pDoc->GetEffectColors()));
		m_renderer->SetLights(ConvertLights(pDoc->GetLights()));
		m_renderer->SetWorldScale(pDoc->GetWorldScale());

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

	void ForceModelRender()
	{
		if (!m_renderer)
			return;

		auto pDoc = m_pThis->GetDocument();
		auto rp = pDoc->GetRotationParams();
		if (rp.Action == RotationAction::Fixed)
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
		auto rp = pDoc->GetRotationParams();
		if (rp.Action == RotationAction::Fixed)
		{
			m_renderer->RefreshRender(0);
		}
	}

	void RunRotationParams(RotationParams rp)
	{
		m_renderer->SetRotationParams(rp);

		SetRotationTimer();
		ForceModelRender();
	}

	void OnEditPerspective()
	{
		auto pDoc = m_pThis->GetDocument();
		auto perspective = pDoc->GetPerspective();

		CPerspectiveDlg dlg;
		dlg.SetNear(perspective.NearPlane);
		dlg.SetFarDistance(perspective.FarPlane);

		if (dlg.DoModal() == IDOK)
		{
			perspective.NearPlane = dlg.GetNearDistance();
			perspective.FarPlane = dlg.GetFarDistance();

			m_renderer->SetPerspective(perspective);

			// Get the perspective back from the renderer in case it changed anything
			pDoc->SetPerspective(m_renderer->GetPerspective());

			RenderIfFixed();
		}
	}

	void OnEditView()
	{
		auto pDoc = m_pThis->GetDocument();
		auto camera = pDoc->GetCamera();
		auto target = pDoc->GetTarget();

		CViewDlg dlg;
		dlg.SetCamera(camera);
		dlg.SetTarget(target);

		if (dlg.DoModal() == IDOK)
		{
			camera = dlg.GetCamera();
			target = dlg.GetTarget();

			pDoc->SetCamera(camera);
			pDoc->SetTarget(target);

			m_renderer->SetView(camera, target);
			RenderIfFixed();
		}
	}

	void AutoZoom()
	{
		bool enableAutoZoom = m_topLeft.has_value() && m_bottomRight.has_value();
		if (!enableAutoZoom)
			return;

		int oldWidth = 0;
		int oldHeight = 0;
		m_renderer->GetWindowSize(oldWidth, oldHeight);

		// Get current zoom
		auto pDoc = m_pThis->GetDocument();
		if (!pDoc)
			return;
		auto worldScale = pDoc->GetWorldScale();

		// calculate new x & y
		int newWidth = m_bottomRight->x - m_topLeft->x;
		int newHeight = m_bottomRight->y - m_topLeft->y;
		if (newWidth <= 0 || newHeight <= 0)
			return;

		float newScaleX = worldScale.X * static_cast<float>(oldWidth) / static_cast<float>(newWidth);
		float newScaleY = worldScale.Y * static_cast<float>(oldHeight) / static_cast<float>(newHeight);

		// set
		worldScale.X = newScaleX;
		worldScale.Y = newScaleY;

		pDoc->SetWorldScale(worldScale);
	}

	void AutoTranslate(Vertex<float> oldWorldScale)
	{
		bool enableAutoTranslate = m_topLeft.has_value() && m_bottomRight.has_value();
		if (!enableAutoTranslate)
			return;

		int oldWidth = 0;
		int oldHeight = 0;
		m_renderer->GetWindowSize(oldWidth, oldHeight);

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
		int centerXold = oldWidth / 2;
		int centerXnew = m_topLeft->x + (m_bottomRight->x - m_topLeft->x)/2;
		double deltaX = (static_cast<double>(centerXold) - centerXnew)/static_cast<double>(centerXold);
		double scaleFactorX = worldScale.X / oldWorldScale.X;
		double translateX = scaleFactorX * (TransFactor *  deltaX +  m_translateX);
		m_translateX = translateX;
		target.X = static_cast<float>(m_translateX);

		// Translate Y
		int centerYold = oldHeight / 2;
		int centerYnew = m_topLeft->y + (m_bottomRight->y - m_topLeft->y) / 2;
		double deltaY = (static_cast<double>(centerYnew) - centerYold) / static_cast<double>(centerYold);
		double scaleFactorY = worldScale.Y / oldWorldScale.Y;
		double translateY = scaleFactorY * (TransFactor * deltaY +  m_translateY);
		m_translateY = translateY;
		target.Y = static_cast<float>(m_translateY);

		pDoc->SetTarget(target);
		m_renderer->SetTarget(target);
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
			auto modelData = modelSheet.GetModelData();
			pDoc->SetModelData(modelData);
			pDoc->SetTraceParams(modelSheet.GetTraceParams());

			if (modelData.SourceVertices == VertexSource::Spherical)
			{
				ResetTranslationValues();
				pDoc->SetWorldScale(GetDefaultWorldScale());
				pDoc->SetTarget(GetDefaultTarget());
				m_renderer->SetTarget(GetDefaultTarget());
			}

			if (modelSheet.GetAutoZoom())
			{
				auto worldScaleOriginal = pDoc->GetWorldScale();
				AutoZoom();
				AutoTranslate(worldScaleOriginal);
			}

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

	void OnEditBackgroundColor()
	{
		auto pDoc = m_pThis->GetDocument();
		if (!pDoc)
			return;

		auto bkColor = pDoc->GetBackgroundColor();

		std::shared_ptr<CColorSelectorDlg> pColorDlg = CColorSelectorDlg::CreateColorSelectorDlg(bkColor, m_pThis);
			
		if (pColorDlg->DoModal() == IDOK)
		{
			auto newColor = pColorDlg->GetColor();
			pDoc->SetBackgroundColor(newColor);
			m_renderer->SetBackgroundColor(ColorUtils::ConvertToDxColor(newColor));

			ForceModelRender();
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
	
		m_positionAngleDlg = CPositionAngleDlg::CreatePositionAngleDlg(pDoc->GetRotationParams(), target, m_pThis);

		m_positionAngleDlg->Create(IDD_POSITION_ANGLE_DLG, NULL);
	}

	void OnKillAnglePosition(WPARAM result)
	{
		if (!m_positionAngleDlg)
			return;

		if (result == cMessage::DlgCancel)
		{		
			auto target = m_positionAngleDlg->GetTarget();	

			auto pDoc = m_pThis->GetDocument();
			pDoc->SetTarget(target);
			m_renderer->SetTarget(target);
		
			auto rp = pDoc->GetRotationParams();
			if (rp.Action == RotationAction::Fixed)
			{
				rp = *m_positionAngleDlg->GetRotationParams();
				pDoc->SetRotationParams(rp);
				m_renderer->SetRotationParams(rp);

				SetRotationTimer();
				ForceModelRender();
			}
		}

		m_positionAngleDlg.reset();
	}

	void OnStopRotation()
	{
		auto pDoc = m_pThis->GetDocument();
		auto rp = m_renderer->GetRotationParams();

		if (rp.Action != RotationAction::Fixed)
		{
			rp.Action = RotationAction::Fixed;
			pDoc->SetRotationParams(rp);
			m_renderer->SetRotationParams(rp);

			SetRotationTimer();
			ForceModelRender();
		}
	}

	bool IsRotating()
	{
		auto rp = m_renderer->GetRotationParams();
		return rp.Action != RotationAction::Fixed;
	}

	void OnModelPositionChanged()
	{
		if (!m_positionAngleDlg)
			return;

		auto target = m_positionAngleDlg->GetTarget();

		auto pDoc = m_pThis->GetDocument();
		pDoc->SetTarget(target);
		m_renderer->SetTarget(target);

		ForceModelRender();
	}

	void MoveModel(int deltaX, int deltaY)
	{
		constexpr float MouseDistance = 200.0f;

		auto pDoc = m_pThis->GetDocument();
		auto target = pDoc->GetTarget();

		target.X = target.X + static_cast<float>(deltaX) / MouseDistance;
		target.Y = target.Y + static_cast<float>(deltaY) / MouseDistance;

		pDoc->SetTarget(target);
		m_renderer->SetTarget(target);

		ForceModelRender();
	}

	void MoveModelAlongZAxis(int deltaZ)
	{
		constexpr float MouseDistanceZ = 50.0f;

		auto pDoc = m_pThis->GetDocument();
		auto target = pDoc->GetTarget();

		target.Z = target.Z + static_cast<float>(deltaZ) / MouseDistanceZ;

		pDoc->SetTarget(target);
		m_renderer->SetTarget(target);

		ForceModelRender();
	}

	void OnModelAngleChanged()
	{
		if (!m_positionAngleDlg)
			return;

		auto pDoc = m_pThis->GetDocument();
		auto rp = pDoc->GetRotationParams();
		if (rp.Action != RotationAction::Fixed)
			return;

		rp = *m_positionAngleDlg->GetRotationParams();
		pDoc->SetRotationParams(rp);
		m_renderer->SetRotationParams(rp);

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

			if (vertexData)
			{
				// a little risky since we're in another thread
				// consider locking data
				pDoc->SetVertexData(vertexData);

				m_renderer->SetWorldScale(pDoc->GetWorldScale());

				m_renderer->SetModel(*vertexData);

				m_renderer->ResetModel();

				std::shared_ptr<DXF::TraceParams> traceParams = std::make_shared<TraceParams>();
				*traceParams = *pDoc->GetTraceParams();
				traceParams->Stretch = vertexData->StretchParams;
				pDoc->SetTraceParams(traceParams);
			}
		}

		m_calcThread->join();

		m_calcThread.reset();

		ResetResizePoints();

		return 0;
	}

	LRESULT OnRender()
	{
		auto pDoc = m_pThis->GetDocument();
		auto rp = pDoc->GetRotationParams();

		// Only need to render here if fixed because there will be no timer
		if (rp.Action == RotationAction::Fixed)
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
		model.VertexIterations = 10;
		model.TriangleSeeds = SeedTriangles::Four;

		std::shared_ptr<TriangleData> triangles = TriangleLoader::CreateTriangles(model, [&](double /*progress*/) {});

		CArchive ar(&triFile, CArchive::store);

		Serialize(ar, *triangles);

		ar.Close();
		triFile.Close();

		AfxMessageBox(_T("Done exporting"));
	}

	shared_ptr<Renderer> GetRenderer()
	{
		return m_renderer;
	}

	bool IsInitialized() const 
	{
		return 	m_isInitialized;
	}

	void SetInitialized(bool init)
	{
		m_isInitialized = init;
	}

	void OnEditEffectColors()
	{
		auto pDoc = m_pThis->GetDocument();
		if (!pDoc)
			return;

		auto effectColors = pDoc->GetEffectColors();
		auto colorsDlg = CEffectColorDlg::CreateEffectColorDlg(effectColors, m_pThis);

		if (colorsDlg->DoModal() == IDOK)
		{
			effectColors = colorsDlg->GetColors();
			pDoc->SetEffectColors(effectColors);
			m_renderer->SetEffectColors(ConvertEffectColors(effectColors));
			ForceModelRender();
		}
	}

	void OnEditLights()
	{
		auto pDoc = m_pThis->GetDocument();
		if (!pDoc)
			return;

		auto lights = pDoc->GetLights();
		auto lightsDlg = CLightsDlg::CreateLightsDlg(lights, m_pThis);
		if (lightsDlg->DoModal())
		{
			lights = lightsDlg->GetLights();
			pDoc->SetLights(lights);
			m_renderer->SetLights(ConvertLights(lights));
			ForceModelRender();
		}
	}

	optional<DXF::Vertex<float>> GetVertex(CPoint point)
	{
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

	Vertex<float> GetWorldScale() const 
	{
		return m_renderer->GetWorldScale();
	}

	void SetWorldScale(const Vertex<float>& worldScale)
	{
		m_renderer->SetWorldScale(worldScale);
		ForceModelRender();
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

protected:

	void SetRotationTimer()
	{
		auto pDoc = m_pThis->GetDocument();
		auto rp = pDoc->GetRotationParams();
		if (rp.Action == RotationAction::Fixed)
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
				int speed = GetRotationSpeedMsec(rp.Speed);
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

		DxColor::PinPalette palette = pinPalette;
		DxColor::ValidatePalette(palette);
		pDoc->SetContrast(contrast);
		m_colors = fx::ColorUtilities::CalculatePaletteColors(palette, m_numberOfColors, pDoc->GetContrast());
		pDoc->SetPalette(palette);
		m_renderer->SetTextureColors(m_colors);
		RenderIfFixed();
	}

protected:

	CFractalXView * m_pThis;

	shared_ptr<Renderer> m_renderer = Renderer::CreateRenderer();
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

	m_pImp->Initialize();

	m_whitePen.CreatePen(PS_SOLID, 4, RGB(255, 255, 255));
	m_blackDashPen.CreatePen(PS_DOT, 2, RGB(0, 0, 0));
}

void CFractalXView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	if (m_pImp->IsInitialized())
	{
		CSize size(cx, cy);
		GetDocument()->SetSize(size);
	}

	m_pImp->ResizeRenderer(cx, cy);
}

void CFractalXView::OnTimer(UINT_PTR nIDEvent)
{
	m_pImp->ForceModelRender();

	CView::OnTimer(nIDEvent);
}

void CFractalXView::OnRButtonUp(UINT nFlags, CPoint point)
{
	if (nFlags & MK_CONTROL)
	{
		int deltaX = point.x - m_lastPoint.x;
		int deltaY = point.y - m_lastPoint.y;

		m_pImp->MoveModel(deltaX, -1 * deltaY);

		m_lastPoint = point;
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
	m_drawRect = false;
	Invalidate(TRUE);

	if (nFlags & MK_CONTROL)
	{
		SetCapture();
		m_lastPoint = point;
	}

	CView::OnRButtonDown(nFlags, point);
}

void CFractalXView::OnMouseMove(UINT nFlags, CPoint point)
{
	m_drawRect = false;

	if (nFlags & MK_CONTROL)
	{
		if(nFlags & MK_RBUTTON)
		{
			int deltaX = point.x - m_lastPoint.x;
			int deltaY = point.y - m_lastPoint.y;

			m_pImp->MoveModel(deltaX, -1 * deltaY);

			m_lastPoint = point;
		}
		else if (nFlags & MK_LBUTTON)
		{
			m_lastPoint = point;
			m_pImp->SetLastVertex(m_pImp->GetVertex(point));

			CChildFrame* pViewFrame = (CChildFrame*)GetParentFrame();
			if (pViewFrame)
			{
				auto msg = MakeStatusMessage(m_firstPoint, m_lastPoint, m_pImp->GetFirstVertex(), m_pImp->GetLastVertex());
				pViewFrame->SetStatusText1(msg);
			}

			m_drawRect = true;
			Invalidate(TRUE);
		}

	}

	CView::OnMouseMove(nFlags, point);
}

void CFractalXView::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_drawRect = false;
	Invalidate(TRUE);

	if (nFlags & MK_CONTROL)
	{
		SetCapture();
		m_firstPoint = point;
		m_lastPoint = point;

		m_pImp->SetFirstVertex(m_pImp->GetVertex(point));
		m_pImp->SetLastVertex(m_pImp->GetFirstVertex());

		m_drawRect = true;

		m_pImp->ResetResizePoints();
	}

	CView::OnLButtonDown(nFlags, point);
}

void CFractalXView::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (nFlags & MK_CONTROL)
	{
		m_lastPoint = point;
		m_pImp->SetLastVertex(m_pImp->GetVertex(point));

		CChildFrame* pViewFrame = (CChildFrame*)GetParentFrame();
		if (pViewFrame)
		{
			auto msg = MakeStatusMessage(m_firstPoint, m_lastPoint, m_pImp->GetFirstVertex(), m_pImp->GetLastVertex());
			pViewFrame->SetStatusText1(msg);
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
		m_pImp->MoveModelAlongZAxis(distance);
	}

	return CView::OnMouseWheel(nFlags, zDelta, pt);
}

void CFractalXView::OnEditSetModel()
{
	m_pImp->OnEditSetModel();
}

void CFractalXView::OnUpdateEditSetModel(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_pImp->CanRender());
}

void CFractalXView::OnEditAnglePosition()
{
	m_pImp->OnEditAnglePosition();
}

void CFractalXView::OnUpdateEditAnglePosition(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_pImp->CanRender());
}

void CFractalXView::OnEditLights()
{
	m_pImp->OnEditLights();
}

void CFractalXView::OnUpdateEditLights(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_pImp->CanRender());
}

void CFractalXView::OnEditUpdateRotation()
{
	CRotationDlg rotationDlg;
	auto pDoc = GetDocument();

	rotationDlg.SetRotationParams(pDoc->GetRotationParams());

	if (rotationDlg.DoModal() == IDOK)
	{
		auto rp = rotationDlg.GetRotationParams();
		pDoc->SetRotationParams(rp);
		m_pImp->RunRotationParams(rp);
	}
}

void CFractalXView::OnUpdateEditRotation(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_pImp->CanRender());
}

void CFractalXView::OnEditStopRotation()
{
	m_pImp->OnStopRotation();
}

void CFractalXView::OnUpdateEditStopRotation(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_pImp->IsRotating());
}

void CFractalXView::OnEditPerspective()
{
	m_pImp->OnEditPerspective();
}

void CFractalXView::OnUpdateEditSetPerspective(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_pImp->CanRender());
}

void CFractalXView::OnEditView()
{
	m_pImp->OnEditView();
}

void CFractalXView::OnUpdateEditSetView(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_pImp->CanRender());
}

void CFractalXView::OnEditBackgroundColor()
{
	m_pImp->OnEditBackgroundColor();
}

void CFractalXView::OnUpdateEditBackgroundColor(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_pImp->CanRender());
}

void CFractalXView::OnEditPalette()
{
	m_pImp->OnEditPalette();
}

void CFractalXView::OnUpdateEditPalette(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_pImp->CanRender());
}

void CFractalXView::OnEditEffectColors()
{
	m_pImp->OnEditEffectColors();
}

void CFractalXView::OnUpdateEditEffectColors(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_pImp->CanRender());
}

void CFractalXView::OnEditSelectPalette()
{
	m_pImp->OnEditSelectPalette();
}

void CFractalXView::OnUpdateEditSelectPalette(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_pImp->CanRender());
}

void CFractalXView::OnExportTriangles()
{
	m_pImp->OnExportTriangles();
}

LRESULT CFractalXView::OnKillPositionAngleDlg(WPARAM wparam, LPARAM)
{
	m_pImp->OnKillAnglePosition(wparam);

	return 0;
}

LRESULT CFractalXView::OnModelPositionChanged(WPARAM, LPARAM)
{
	m_pImp->OnModelPositionChanged();

	return 0;
}

LRESULT CFractalXView::OnModelAngleChanged(WPARAM, LPARAM)
{
	m_pImp->OnModelAngleChanged();

	return 0;
}

LRESULT CFractalXView::OnKillPaletteView(WPARAM, LPARAM)
{
	m_pImp->OnKillPaletteView();
	return 0;
}

LRESULT CFractalXView::OnCalcFinished(WPARAM wparam, LPARAM)
{
	auto result = m_pImp->OnCalcFinished(wparam);

	// Need to force draw if fixed (no rotation therefore no timer)
	PostMessage(cMessage::tm_render);

	return result;
}

LRESULT CFractalXView::OnUpdateProgress(WPARAM wparam, LPARAM)
{
	return m_pImp->OnUpdateProgress(wparam);
}

LRESULT CFractalXView::OnRender(WPARAM, LPARAM)
{
	return m_pImp->OnRender();
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

	shared_ptr<Renderer> renderer = m_pImp->GetRenderer();
	CSize imageSize = renderer->GetScreenSize();

	auto imageSizeDlg = CImageSizeDlg::CreateImageSizeDlg(imageSize.cx, imageSize.cy, this);
	if (imageSizeDlg->DoModal() != IDOK)
		return;

	CSize newSize = imageSizeDlg->GetSize();

	bitmap.CreateCompatibleBitmap(&dc, newSize.cx, newSize.cy);
	dcBitmap.SelectObject(&bitmap);

	if (!renderer->DrawImage(dcBitmap, newSize))
	{
		AfxMessageBox(_T("Could not capture image!"), MB_OK);
		return;
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
	m_pImp->SetInitialized(true);
	m_pImp->ForceModelRender();

	if (!m_drawRect)
		return;

	CRect hatchedRect = MakeRect(m_firstPoint, m_lastPoint);

	CBrush* pOldBrush = (CBrush*)pDC->SelectStockObject(NULL_BRUSH);
	CPen* pOldPen = pDC->SelectObject(&m_whitePen);

	pDC->Rectangle(hatchedRect);

	pDC->SelectObject(&m_blackDashPen);
	pDC->Rectangle(hatchedRect);

	pDC->SelectObject(pOldPen);
	pDC->SelectObject(pOldBrush);
}

void CFractalXView::OnSaveSelectedVertices()
{
	m_pImp->SaveVertices(m_firstPoint, m_lastPoint);
}

void CFractalXView::OnUpdateSaveSelectedVertices(CCmdUI* pCmdUI)
{
	auto firstVertex = m_pImp->GetFirstVertex();
	auto lastVertex = m_pImp->GetLastVertex();
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
	auto pDoc = GetDocument();
	auto worldScale = pDoc->GetWorldScale();
	auto worldScaleDlg = CWorldScaleDlg::CreateWorldScaleDlg(worldScale, this);

	if (worldScaleDlg->DoModal() == IDOK)
	{
		worldScale = worldScaleDlg->GetWorldScale();
		pDoc->SetWorldScale(worldScale);
		m_pImp->SetWorldScale(worldScale);
	}		
}

void CFractalXView::OnUpdateEditSetWorldScale(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_pImp->CanRender());
}