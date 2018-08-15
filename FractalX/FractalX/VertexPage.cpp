// VertexPage.cpp : implementation file
//

#include "stdafx.h"
#include "FractalX.h"
#include "VertexPage.h"
#include "ModelSheet.h"
#include "VertexStatistics.h"

using namespace DXF;

// CVertexPage

IMPLEMENT_DYNAMIC(CVertexPage, CMFCPropertyPage)

CVertexPage::CVertexPage()
	: CMFCPropertyPage(CVertexPage::IDD)
{}

CVertexPage::~CVertexPage()
{}

BEGIN_MESSAGE_MAP(CVertexPage, CMFCPropertyPage)
	ON_EN_KILLFOCUS(IDC_VERTEX_ITERATIONS_EDIT, &CVertexPage::OnKillfocusIterationEdit)
	ON_CBN_SELCHANGE(IDC_SEED_TRIANGLES_COMBO, &CVertexPage::OnCbnSelchangeSeedTrianglesCombo)
END_MESSAGE_MAP()

void CVertexPage::DoDataExchange(CDataExchange* pDX)
{
	CMFCPropertyPage::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_SEED_TRIANGLES_COMBO, m_SeedTrianglesCombo);

	DDX_Text(pDX, IDC_VERTEX_ITERATIONS_EDIT, m_vertexIterations);
	DDV_MinMaxInt(pDX, m_vertexIterations, 1, 10);

	DDX_Text(pDX, IDC_VERTEX_NUMBER_EDIT, m_vertexNumber);
	DDX_Text(pDX, IDC_TRIANGLE_NUMBER_EDIT, m_triangleNumber);
}

BOOL CVertexPage::OnSetActive()
{
	PopulateMSeedTriangleCombo();

	return CPropertyPage::OnSetActive();
}

void CVertexPage::PopulateMSeedTriangleCombo()
{
	m_SeedTrianglesCombo.ResetContent();

	m_SeedTrianglesCombo.AddString(GetSeedTriangleString(SeedTriangles::One));
	m_SeedTrianglesCombo.AddString(GetSeedTriangleString(SeedTriangles::Two));
	m_SeedTrianglesCombo.AddString(GetSeedTriangleString(SeedTriangles::Four));
	m_SeedTrianglesCombo.AddString(GetSeedTriangleString(SeedTriangles::Eight));

	m_SeedTrianglesCombo.SetCurSel(GetIndexForSeedTriangle(m_seedTriangles));
}

// CVertexPage message handlers

void CVertexPage::SetVertexIterations(int iterations)
{
	m_vertexIterations = iterations;

	CalculateStatistics();
}

int CVertexPage::GetVertexIterations() const
{
	return m_vertexIterations;
}

void CVertexPage::SetSeedTriangles(DXF::SeedTriangles seedTriangles)
{
	m_seedTriangles = seedTriangles;
}

DXF::SeedTriangles CVertexPage::GetSeedTriangles() const
{
	return m_seedTriangles;
}

void CVertexPage::OnOK()
{
	CModelSheet* pModelSheet = dynamic_cast<CModelSheet*>(GetParent());
	if (pModelSheet)
		pModelSheet->OnOk();

	CMFCPropertyPage::OnOK();
}

void CVertexPage::CalculateStatistics()
{
	m_vertexNumber = VertexStatistics::CalculateNumberOfVertices(m_vertexIterations, m_seedTriangles);
	m_triangleNumber = VertexStatistics::CalculateNumberOfTriangles(m_vertexIterations, m_seedTriangles);
}

void CVertexPage::OnKillfocusIterationEdit()
{
	UpdateData(TRUE);

	CalculateStatistics();

	UpdateData(FALSE);
}

void CVertexPage::OnCbnSelchangeSeedTrianglesCombo()
{
	int index = m_SeedTrianglesCombo.GetCurSel();
	if (index == CB_ERR)
	{
		m_seedTriangles = SeedTriangles::Eight;
		m_SeedTrianglesCombo.SetCurSel(GetIndexForSeedTriangle(m_seedTriangles));
	}
	else
	{
		m_seedTriangles = GetSeedTriangleFromIndex(index);
	}

	CalculateStatistics();

	UpdateData(FALSE);
}

