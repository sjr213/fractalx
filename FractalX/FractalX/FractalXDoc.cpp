
// FractalXDoc.cpp : implementation of the CFractalXDoc class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "FractalX.h"
#endif

#include "FractalXDoc.h"

#include "ColorContrast.h"
#include "ColorPin.h"
#include "DxColorsSerialization.h"
#include "DxSerialize.h"
#include "ModelData.h"
#include <mutex>
#include "Palettes.h"
#include <propkey.h>
#include "TraceParams.h"
#include "TraceParamsSerialize.h"
#include "VertexData.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace DXF;

struct FractalXDocImpl
{
	std::mutex VertexDataMutex;
	std::mutex TraceParamsMutex;

	ModelData ModelDesc;
	std::shared_ptr<DxVertexData> VertexData;
	TraceParams CalculationParams;
	DxColor::PinPalette Palette;
	DxColor::ColorContrast Contrast;
};

// CFractalXDoc

IMPLEMENT_DYNCREATE(CFractalXDoc, CDocument)

BEGIN_MESSAGE_MAP(CFractalXDoc, CDocument)
END_MESSAGE_MAP()


// CFractalXDoc construction/destruction

CFractalXDoc::CFractalXDoc()
	: m_impl(new FractalXDocImpl)
{
	m_impl->VertexData = std::make_shared<DxVertexData>();
	m_impl->Palette = *DxColor::GetRainbow();
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
	return m_impl->ModelDesc;
}
	
void CFractalXDoc::SetModelData(const DXF::ModelData& modelData)
{
	m_impl->ModelDesc = modelData;
	SetModifiedFlag();
}

const std::shared_ptr<DXF::DxVertexData> CFractalXDoc::GetVertexData() const
{
	std::lock_guard<std::mutex> lock(m_impl->VertexDataMutex);

	return m_impl->VertexData;
}

void CFractalXDoc::SetVertexData(std::shared_ptr<DXF::DxVertexData>& vertexData)
{
	std::lock_guard<std::mutex> lock(m_impl->VertexDataMutex);
	m_impl->VertexData = vertexData;
	SetModifiedFlag();
}

DXF::TraceParams CFractalXDoc::GetTraceParams() const
{
	std::lock_guard<std::mutex> lock(m_impl->TraceParamsMutex);

	return m_impl->CalculationParams;
}

void CFractalXDoc::SetTraceParams(const DXF::TraceParams& params)
{
	std::lock_guard<std::mutex> lock(m_impl->TraceParamsMutex);

	m_impl->CalculationParams = params;
	SetModifiedFlag();
}

DxColor::PinPalette CFractalXDoc::GetPalette() const
{
	return m_impl->Palette;
}

void CFractalXDoc::SetPalette(const DxColor::PinPalette& palette)
{
	m_impl->Palette = palette;
	SetModifiedFlag();
}

DxColor::ColorContrast CFractalXDoc::GetContrast() const
{
	return m_impl->Contrast;
}

void CFractalXDoc::SetContrast(const DxColor::ColorContrast& contrast)
{
	m_impl->Contrast = contrast;
	SetModifiedFlag();
}

// CFractalXDoc serialization

void CFractalXDoc::Serialize(CArchive& ar)
{
	const int FractalXVersion = 2;

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

	SerializeModelData(ar, m_impl->ModelDesc);

	m_impl->VertexDataMutex.lock();

	if (!m_impl->VertexData)
		m_impl->VertexData = std::make_shared<DxVertexData>();

	SerializeVertexData(ar, *(m_impl->VertexData));

	m_impl->VertexDataMutex.unlock();

	std::lock_guard<std::mutex> lock(m_impl->TraceParamsMutex);

	DXF::Serialize(ar, m_impl->CalculationParams);
	DxColor::SerializePalette(ar, m_impl->Palette);

	if (version > 1)
		DxColor::Serialization::Serialize(ar, m_impl->Contrast);
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
