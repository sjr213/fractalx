
// FractalXDoc.h : interface of the CFractalXDoc class
//


#pragma once

struct FractalXDocImpl;

namespace DXF {
	struct ModelData;
	struct DxVertexData;
	struct TraceParams;
}

namespace DxColor
{
	struct PinPalette;
	struct ColorContrast;
}


class CFractalXDoc : public CDocument
{
protected: // create from serialization only
	CFractalXDoc();
	DECLARE_DYNCREATE(CFractalXDoc)

// Attributes
public:
	const DXF::ModelData& GetModelData() const;
	void SetModelData(const DXF::ModelData& modelData);

	const std::shared_ptr<DXF::DxVertexData> GetVertexData() const;
	void SetVertexData(std::shared_ptr<DXF::DxVertexData>& vertexData);

	DXF::TraceParams GetTraceParams() const;
	void SetTraceParams(const DXF::TraceParams& params);

	DxColor::PinPalette GetPalette() const;
	void SetPalette(const DxColor::PinPalette& palette);

	DxColor::ColorContrast GetContrast() const;
	void SetContrast(const DxColor::ColorContrast& contrast);

// Operations
public:

// Overrides
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// Implementation
public:
	virtual ~CFractalXDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

	void SaveImage(CString filename);

// Generated message map functions

	DECLARE_MESSAGE_MAP()

	std::unique_ptr<FractalXDocImpl> m_impl;

#ifdef SHARED_HANDLERS
	// Helper function that sets search content for a Search Handler
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS

	afx_msg void OnFileSaveAs();
	afx_msg void OnUpdateFileSaveAs(CCmdUI *pCmdUI);
	afx_msg void OnUpdateFileSave(CCmdUI *pCmdUI);
};
