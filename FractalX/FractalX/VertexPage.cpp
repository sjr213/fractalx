// VertexPage.cpp : implementation file
//

#include "stdafx.h"
#include "FractalX.h"
#include "VertexPage.h"
#include "ModelSheet.h"
#include "UiUtilities.h"
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
	ON_BN_CLICKED(IDC_SPHERICAL_RAD, &CVertexPage::OnBnClickSphericalVertexRad)
	ON_BN_CLICKED(IDC_CUSTOM_VERTEX_RAD, &CVertexPage::OnBnClickCustomVertexRad)
END_MESSAGE_MAP()

void CVertexPage::DoDataExchange(CDataExchange* pDX)
{
	CMFCPropertyPage::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_SEED_TRIANGLES_COMBO, m_SeedTrianglesCombo);

	DDX_Text(pDX, IDC_VERTEX_ITERATIONS_EDIT, m_vertexIterations);
	DDV_MinMaxInt(pDX, m_vertexIterations, 1, 10);

	DDX_Text(pDX, IDC_VERTEX_NUMBER_EDIT, m_vertexNumber);
	DDX_Text(pDX, IDC_TRIANGLE_NUMBER_EDIT, m_triangleNumber);

	DDX_Text(pDX, IDC_TL_X_EDIT, m_vertexTL.X);
	DDX_Text(pDX, IDC_TL_Y_EDIT, m_vertexTL.Y);
	DDX_Text(pDX, IDC_TL_Z_EDIT, m_vertexTL.Z);

	DDX_Text(pDX, IDC_TR_X_EDIT, m_vertexTR.X);
	DDX_Text(pDX, IDC_TR_Y_EDIT, m_vertexTR.Y);
	DDX_Text(pDX, IDC_TR_Z_EDIT, m_vertexTR.Z);

	DDX_Text(pDX, IDC_BL_X_EDIT, m_vertexBL.X);
	DDX_Text(pDX, IDC_BL_Y_EDIT, m_vertexBL.Y);
	DDX_Text(pDX, IDC_BL_Z_EDIT, m_vertexBL.Z);

	DDX_Text(pDX, IDC_BR_X_EDIT, m_vertexBR.X);
	DDX_Text(pDX, IDC_BR_Y_EDIT, m_vertexBR.Y);
	DDX_Text(pDX, IDC_BR_Z_EDIT, m_vertexBR.Z);
}

BOOL CVertexPage::OnSetActive()
{
	PopulateMSeedTriangleCombo();

	EnableCtrls();

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

void CVertexPage::SetVertexSource(VertexSource vertexSource)
{
	m_vertexSource = vertexSource;
}

VertexSource CVertexPage::GetVertexSource() const
{
	return m_vertexSource;
}

void CVertexPage::SetVertices(const VertexRect& vRect)
{
	m_vertexTL = vRect.TL;
	m_vertexTR = vRect.TR;
	m_vertexBL = vRect.BL;
	m_vertexBR = vRect.BR;
}

VertexRect CVertexPage::GetVertices() const
{
	VertexRect vRect{ m_vertexTL, m_vertexTR, m_vertexBL, m_vertexBR };
	return vRect;
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

void CVertexPage::OnBnClickSphericalVertexRad()
{
	m_vertexSource = VertexSource::Spherical;
	EnableCtrls();
}

void CVertexPage::OnBnClickCustomVertexRad()
{
	m_vertexSource = VertexSource::CustomVertexPair;
	EnableCtrls();
}

void CVertexPage::EnableCtrls()
{
	using namespace UiUtilities;

	bool enableCustom = m_vertexSource == VertexSource::CustomVertexPair;

	SafeCheck(this, IDC_SPHERICAL_RAD, !enableCustom);
	SafeEnable(this, IDC_SEED_TRIANGLES_COMBO, !enableCustom);

	SafeCheck(this, IDC_CUSTOM_VERTEX_RAD, enableCustom);
	SafeEnable(this, IDC_TL_X_EDIT, enableCustom);
	SafeEnable(this, IDC_TL_Y_EDIT, enableCustom);
	SafeEnable(this, IDC_TL_Z_EDIT, enableCustom);
	SafeEnable(this, IDC_TR_X_EDIT, enableCustom);
	SafeEnable(this, IDC_TR_Y_EDIT, enableCustom);
	SafeEnable(this, IDC_TR_Z_EDIT, enableCustom);
	SafeEnable(this, IDC_BL_X_EDIT, enableCustom);
	SafeEnable(this, IDC_BL_Y_EDIT, enableCustom);
	SafeEnable(this, IDC_BL_Z_EDIT, enableCustom);
	SafeEnable(this, IDC_BR_X_EDIT, enableCustom);
	SafeEnable(this, IDC_BR_Y_EDIT, enableCustom);
	SafeEnable(this, IDC_BR_Z_EDIT, enableCustom);
}