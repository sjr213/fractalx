
// FractalXDoc.cpp : implementation of the CFractalXDoc class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "FractalX.h"
#endif

#include "FractalXDoc.h"

#include "BackgroundVertexData.h"
#include "ColorArgb.h"
#include "ColorContrast.h"
#include "ColorPin.h"
#include "DefaultFields.h"
#include "DxColorsSerialization.h"
#include "..\DxCore\SerializationException.h"
#include "DxSerialize.h"
#include "EffectColors.h"
#include "FractalXView.h"
#include "Light.h"
#include "ModelData.h"
#include <mutex>
#include "Palettes.h"
#include "Perspective.h"
#include <propkey.h>
#include "RotationGroup.h"
#include "TraceParams.h"
#include "TraceParamsSerialize.h"
#include "VertexData.h"
#include "VertexSerialization.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace DxColor;
using namespace DXF;

struct FractalXDocImpl
{
	std::mutex VertexDataMutex;
	std::mutex TraceParamsMutex;
	std::mutex GeneralFieldMutex;

	ModelData m_modelDesc;
	std::shared_ptr<DxVertexData> m_vertexData;
	std::shared_ptr<TraceParams> m_calculationParams;
	PinPalette m_palette;
	ColorContrast m_contrast;
	CSize m_size;
	RotationGroup m_rotationGroup;
	Vertex<float> m_camera;
	Vertex<float> m_target;	// model position
	Vertex<float> m_targetBackground;
	DXF::DxPerspective m_perspective;
	ColorArgb m_backgroundColor;
	EffectColors m_effectColors;
	Lights m_lights;
	Vertex<float> m_worldScale;
	std::shared_ptr<DxBackgroundVertexData> m_backgroundVertexData;

	// methods
	bool CanSave() const
	{
		return m_vertexData != nullptr && m_vertexData->Vertices.size() > 0;
	}
};

// CFractalXDoc

IMPLEMENT_DYNCREATE(CFractalXDoc, CDocument)

BEGIN_MESSAGE_MAP(CFractalXDoc, CDocument)
	ON_COMMAND(ID_FILE_SAVE_AS, &CFractalXDoc::OnFileSaveAs)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_AS, &CFractalXDoc::OnUpdateFileSaveAs)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, &CFractalXDoc::OnUpdateFileSave)
END_MESSAGE_MAP()


// CFractalXDoc construction/destruction

CFractalXDoc::CFractalXDoc()
	: m_impl(new FractalXDocImpl)
{
	m_impl->m_vertexData = std::make_shared<DxVertexData>();
	m_impl->m_calculationParams = std::make_shared<TraceParams>();
	m_impl->m_palette = *GetRainbow();
	m_impl->m_size = CSize(820, 843);
	m_impl->m_camera = GetDefaultCamera();
	m_impl->m_target = GetDefaultTarget();
	m_impl->m_targetBackground = GetDefaultTarget();
	m_impl->m_backgroundColor = ColorArgb(255, 100, 148, 240);
	m_impl->m_worldScale = GetDefaultWorldScale();
	m_impl->m_backgroundVertexData = std::make_shared<DxBackgroundVertexData>();
}

CFractalXDoc::~CFractalXDoc()
{
}

BOOL CFractalXDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}


const DXF::ModelData& CFractalXDoc::GetModelData() const
{
	return m_impl->m_modelDesc;
}
	
void CFractalXDoc::SetModelData(const DXF::ModelData& modelData)
{
	m_impl->m_modelDesc = modelData;
	SetModifiedFlag();
}

const std::shared_ptr<DXF::DxVertexData> CFractalXDoc::GetVertexData() const
{
	std::lock_guard<std::mutex> lock(m_impl->VertexDataMutex);

	return m_impl->m_vertexData;
}

void CFractalXDoc::SetVertexData(std::shared_ptr<DXF::DxVertexData>& vertexData)
{
	std::lock_guard<std::mutex> lock(m_impl->VertexDataMutex);
	m_impl->m_vertexData = vertexData;
	SetModifiedFlag();
}

const std::shared_ptr<DXF::DxBackgroundVertexData> CFractalXDoc::GetBackgroundVertexData() const
{
	std::lock_guard<std::mutex> lock(m_impl->VertexDataMutex);

	return m_impl->m_backgroundVertexData;
}

void CFractalXDoc::SetBackgroundVertexData(std::shared_ptr<DXF::DxBackgroundVertexData>& bkgndVertexData)
{
	std::lock_guard<std::mutex> lock(m_impl->VertexDataMutex);
	m_impl->m_backgroundVertexData = bkgndVertexData;
	SetModifiedFlag();
}

std::shared_ptr<const DXF::TraceParams> CFractalXDoc::GetTraceParams() const
{
	std::lock_guard<std::mutex> lock(m_impl->TraceParamsMutex);

	return m_impl->m_calculationParams;
}

void CFractalXDoc::SetTraceParams(std::shared_ptr<DXF::TraceParams>& params)
{
	std::lock_guard<std::mutex> lock(m_impl->TraceParamsMutex);
	*m_impl->m_calculationParams = *params;
	SetModifiedFlag();
}

PinPalette CFractalXDoc::GetPalette() const
{
	std::lock_guard<std::mutex> lock(m_impl->GeneralFieldMutex);
	return m_impl->m_palette;
}

void CFractalXDoc::SetPalette(const PinPalette& palette)
{
	std::lock_guard<std::mutex> lock(m_impl->GeneralFieldMutex);
	m_impl->m_palette = palette;
	SetModifiedFlag();
}

ColorContrast CFractalXDoc::GetContrast() const
{
	std::lock_guard<std::mutex> lock(m_impl->GeneralFieldMutex);
	return m_impl->m_contrast;
}

void CFractalXDoc::SetContrast(const ColorContrast& contrast)
{
	std::lock_guard<std::mutex> lock(m_impl->GeneralFieldMutex);
	m_impl->m_contrast = contrast;
	SetModifiedFlag();
}

CSize CFractalXDoc::GetSize() const
{
	std::lock_guard<std::mutex> lock(m_impl->GeneralFieldMutex);
	return m_impl->m_size;
}

void CFractalXDoc::SetSize(CSize size)
{
	std::lock_guard<std::mutex> lock(m_impl->GeneralFieldMutex);
	m_impl->m_size = size;
	SetModifiedFlag();
}

DXF::RotationGroup CFractalXDoc::GetRotationGroup() const
{
	std::lock_guard<std::mutex> lock(m_impl->GeneralFieldMutex);
	return m_impl->m_rotationGroup;
}

void CFractalXDoc::SetRotationGroup(const DXF::RotationGroup& rotationGroup)
{
	std::lock_guard<std::mutex> lock(m_impl->GeneralFieldMutex);
	m_impl->m_rotationGroup = rotationGroup;
	SetModifiedFlag();
}

Vertex<float> CFractalXDoc::GetCamera() const
{
	std::lock_guard<std::mutex> lock(m_impl->GeneralFieldMutex);
	return m_impl->m_camera;
}

void CFractalXDoc::SetCamera(const Vertex<float>& camera)
{
	std::lock_guard<std::mutex> lock(m_impl->GeneralFieldMutex);
	m_impl->m_camera = camera;
	SetModifiedFlag();
}

Vertex<float> CFractalXDoc::GetTarget() const
{
	std::lock_guard<std::mutex> lock(m_impl->GeneralFieldMutex);
	return m_impl->m_target;
}

void CFractalXDoc::SetTarget(const Vertex<float>& target)
{
	std::lock_guard<std::mutex> lock(m_impl->GeneralFieldMutex);
	m_impl->m_target = target;
	SetModifiedFlag();
}

DXF::Vertex<float> CFractalXDoc::GetTargetBackground() const
{
	std::lock_guard<std::mutex> lock(m_impl->GeneralFieldMutex);
	return m_impl->m_targetBackground;
}

void CFractalXDoc::SetTargetBackground(const DXF::Vertex<float>& targetBkgnd)
{
	std::lock_guard<std::mutex> lock(m_impl->GeneralFieldMutex);
	m_impl->m_targetBackground = targetBkgnd;
	SetModifiedFlag();
}

DXF::DxPerspective CFractalXDoc::GetPerspective() const
{
	std::lock_guard<std::mutex> lock(m_impl->GeneralFieldMutex);
	return m_impl->m_perspective;
}

void CFractalXDoc::SetPerspective(const DXF::DxPerspective& perspective)
{
	std::lock_guard<std::mutex> lock(m_impl->GeneralFieldMutex);
	m_impl->m_perspective = perspective;
	SetModifiedFlag();
}

ColorArgb CFractalXDoc::GetBackgroundColor() const
{
	std::lock_guard<std::mutex> lock(m_impl->GeneralFieldMutex);
	return m_impl->m_backgroundColor;
}

void CFractalXDoc::SetBackgroundColor(ColorArgb bkColor)
{
	std::lock_guard<std::mutex> lock(m_impl->GeneralFieldMutex);
	m_impl->m_backgroundColor = bkColor;
	SetModifiedFlag();
}

DxColor::EffectColors CFractalXDoc::GetEffectColors() const
{
	std::lock_guard<std::mutex> lock(m_impl->GeneralFieldMutex);
	return m_impl->m_effectColors;
}

void CFractalXDoc::SetEffectColors(const DxColor::EffectColors& effectColors)
{
	std::lock_guard<std::mutex> lock(m_impl->GeneralFieldMutex);
	m_impl->m_effectColors = effectColors;
	SetModifiedFlag();
}

DxColor::Lights CFractalXDoc::GetLights() const
{
	std::lock_guard<std::mutex> lock(m_impl->GeneralFieldMutex);
	return m_impl->m_lights;
}

void CFractalXDoc::SetLights(const DxColor::Lights lights)
{
	std::lock_guard<std::mutex> lock(m_impl->GeneralFieldMutex);
	m_impl->m_lights = lights;
	SetModifiedFlag();
}

Vertex<float> CFractalXDoc::GetWorldScale() const
{
	std::lock_guard<std::mutex> lock(m_impl->GeneralFieldMutex);
	return m_impl->m_worldScale;
}

void CFractalXDoc::SetWorldScale(const Vertex<float>& worldScale)
{
	std::lock_guard<std::mutex> lock(m_impl->GeneralFieldMutex);
	m_impl->m_worldScale = worldScale;
	SetModifiedFlag();
}

// CFractalXDoc serialization

void CFractalXDoc::Serialize(CArchive& ar)
{
	const int FractalXVersion = 4;

	int version = FractalXVersion;

	if (ar.IsStoring())
	{
		ar << FractalXVersion;
	}
	else
	{
		ar >> version;

		assert(version <= FractalXVersion);
		if (version > FractalXVersion || version < 1)
			return;
	}

	try
	{
		if (IsVersionSupported(version, 1, FractalXVersion))
			SerializeModelData(ar, m_impl->m_modelDesc);

		m_impl->VertexDataMutex.lock();
		if (!m_impl->m_vertexData)
			m_impl->m_vertexData = std::make_shared<DxVertexData>();
		if (IsVersionSupported(version, 1, FractalXVersion))
			SerializeVertexData(ar, *(m_impl->m_vertexData));
		m_impl->VertexDataMutex.unlock();

		m_impl->TraceParamsMutex.lock();
		if (IsVersionSupported(version, 1, FractalXVersion))
			DXF::Serialize(ar, *m_impl->m_calculationParams);
		m_impl->TraceParamsMutex.unlock();

		if (IsVersionSupported(version, 1, FractalXVersion))
		{
			m_impl->GeneralFieldMutex.lock();
			SerializePalette(ar, m_impl->m_palette);
			DxColor::Serialize(ar, m_impl->m_contrast);

			if (ar.IsStoring())
				ar << m_impl->m_size;
			else
				ar >> m_impl->m_size;

			if (version < 4)
				DXF::Serialize(ar, m_impl->m_rotationGroup.RotationParamsMain);

			DXF::Serialize(ar, m_impl->m_camera);
			DXF::Serialize(ar, m_impl->m_target);
			DXF::Serialize(ar, m_impl->m_perspective);
			SerializeColorArgb(ar, m_impl->m_backgroundColor);
			m_impl->GeneralFieldMutex.unlock();
		}

		if (IsVersionSupported(version, 2, FractalXVersion))
		{
			m_impl->GeneralFieldMutex.lock();
			DxColor::SerializeEffectColors(ar, m_impl->m_effectColors);
			DxColor::SerializeLights(ar, m_impl->m_lights);
			m_impl->GeneralFieldMutex.unlock();
		}

		if (IsVersionSupported(version, 3, FractalXVersion))
		{
			m_impl->GeneralFieldMutex.lock();
			DXF::Serialize(ar, m_impl->m_worldScale);
			m_impl->GeneralFieldMutex.unlock();
		}

		if (IsVersionSupported(version, 4, FractalXVersion))
		{
			m_impl->GeneralFieldMutex.lock();
			DXF::Serialize(ar, m_impl->m_rotationGroup);
			DXF::Serialize(ar, *m_impl->m_backgroundVertexData);
			DXF::Serialize(ar, m_impl->m_targetBackground);
			m_impl->GeneralFieldMutex.unlock();
		}
	}
	catch (DxCore::CSerializationException & ex)
	{
		CString msg("Serialization failed: ");
		msg += ex.what();

		AfxMessageBox(msg, MB_OK);
	}

}

#ifdef SHARED_HANDLERS

// Support for thumbnails
void CFractalXDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// Modify this code to draw the document's data
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// Support for Search Handlers
void CFractalXDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// Set search contents from document's data.
	// The content parts should be separated by ";"

	// For example:  strSearchContent = _T("point;rectangle;circle;ole object;");
	SetSearchContent(strSearchContent);
}

void CFractalXDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = NULL;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != NULL)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CFractalXDoc diagnostics

#ifdef _DEBUG
void CFractalXDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CFractalXDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CFractalXDoc commands


void CFractalXDoc::OnFileSaveAs()
{
	CString fileName = GetPathName();
	CString title = GetTitle();

	if (fileName.IsEmpty())
		fileName = title;

	wchar_t* logextension = _T(".frc");

	// Create a CFileDialog for opening dialog
	CFileDialog saveDlg(FALSE, logextension, title, OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		_T("FractalX files(*.frc)|*.frc|Windows bitmap(*.bmp)|*.bmp|Jpeg file(*.jpg)|*.jpg|PNG file(*.png)|*.png|"));

	// Display as modal 
	if (saveDlg.DoModal() != IDOK)
		return;

	fileName = saveDlg.GetPathName();
	CString ext = saveDlg.GetFileExt();
	ext = ext.MakeLower();

	if (ext.CompareNoCase(_T("frc")) == 0)
	{
		OnSaveDocument(fileName);
	}
	else
	{
		auto pView = GetView();
		if (!pView)
		{
			AfxMessageBox(_T("Could not get view!"), IDOK);
			return;
		}
		pView->SaveImage(fileName, ext);
	}
}

void CFractalXDoc::OnUpdateFileSaveAs(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_impl->CanSave());
}


void CFractalXDoc::OnUpdateFileSave(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_impl->CanSave());
}

CFractalXView* CFractalXDoc::GetView()
{
	POSITION pos = GetFirstViewPosition();
	while (pos != NULL)
	{
		CView* pView = GetNextView(pos);
		CFractalXView* pFractalView = dynamic_cast<CFractalXView*>(pView);
		if (pFractalView)
		{
			return pFractalView;
		}
	}

	return nullptr;
}
