#pragma once
#include <SeedTriangles.h>


// CVertexPage

class CVertexPage : public CMFCPropertyPage
{
	DECLARE_DYNAMIC(CVertexPage)

public:
	CVertexPage();
	virtual ~CVertexPage();

	enum { IDD = IDD_VERTEX_PAGE };

	void SetVertexIterations(int iterations);

	int GetVertexIterations() const;

	void SetSeedTriangles(DXF::SeedTriangles seedTriangles);

	DXF::SeedTriangles GetSeedTriangles() const;

protected:
	DECLARE_MESSAGE_MAP()

	virtual void DoDataExchange(CDataExchange* pDX);

	virtual BOOL OnSetActive();

	void OnOK() override;

	void CalculateStatistics();

	void PopulateMSeedTriangleCombo();

	afx_msg void OnKillfocusIterationEdit();

	afx_msg void OnCbnSelchangeSeedTrianglesCombo();

	CComboBox m_SeedTrianglesCombo;

	// Data

	int m_vertexIterations  = 3;

	DXF::SeedTriangles m_seedTriangles =  DXF::SeedTriangles::Eight;

	int m_vertexNumber = 0;

	int m_triangleNumber = 0;
};


