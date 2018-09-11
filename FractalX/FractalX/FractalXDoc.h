
// FractalXDoc.h : interface of the CFractalXDoc class
//


#pragma once

struct FractalXDocImpl;

namespace DXF {
	struct ModelData;
	struct DxVertexData;
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

	std::shared_ptr<const DXF::DxVertexData> GetVertexData() const;
	void SetVertexData(const DXF::DxVertexData& vertexData);


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

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

	std::unique_ptr<FractalXDocImpl> m_impl;

#ifdef SHARED_HANDLERS
	// Helper function that sets search content for a Search Handler
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
};
