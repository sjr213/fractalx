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

namespace 
{
	const float maxDif = 0.0001f;
}

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
	ON_EN_KILLFOCUS(IDC_TL_X_EDIT, &CVertexPage::OnKillFocusVertex)
	ON_EN_KILLFOCUS(IDC_TL_Y_EDIT, &CVertexPage::OnKillFocusVertex)
	ON_EN_KILLFOCUS(IDC_TL_Z_EDIT, &CVertexPage::OnKillFocusVertex)
	ON_EN_KILLFOCUS(IDC_TR_X_EDIT, &CVertexPage::OnKillFocusVertex)
	ON_EN_KILLFOCUS(IDC_TR_Y_EDIT, &CVertexPage::OnKillFocusVertex)
	ON_EN_KILLFOCUS(IDC_TR_Z_EDIT, &CVertexPage::OnKillFocusVertex)
	ON_EN_KILLFOCUS(IDC_BL_X_EDIT, &CVertexPage::OnKillFocusVertex)
	ON_EN_KILLFOCUS(IDC_BL_Y_EDIT, &CVertexPage::OnKillFocusVertex)
	ON_EN_KILLFOCUS(IDC_BL_Z_EDIT, &CVertexPage::OnKillFocusVertex)
	ON_EN_KILLFOCUS(IDC_BR_X_EDIT, &CVertexPage::OnKillFocusVertex)
	ON_EN_KILLFOCUS(IDC_BR_Y_EDIT, &CVertexPage::OnKillFocusVertex)
	ON_EN_KILLFOCUS(IDC_BR_Z_EDIT, &CVertexPage::OnKillFocusVertex)
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

	DDX_Check(pDX, IDC_AUTO_ZOOM_CHECK, m_autoZoom);
	DDX_Check(pDX, IDC_AUTO_TRANSLATE_CHECK, m_autoTranslate);

	DDX_Check(pDX, IDC_DEFAULT_TARGET_CHECK, m_defaultTargetAndWorldScale);

	if (pDX->m_bSaveAndValidate)
		UpdateAutoZoomCtrls();
}

BOOL CVertexPage::OnSetActive()
{
	PopulateSeedTriangleCombo();

	EnableCtrls();

	return CPropertyPage::OnSetActive();
}

void CVertexPage::PopulateSeedTriangleCombo()
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
	m_vertexTL_original = m_vertexTL = vRect.TL;
	m_vertexTR_original = m_vertexTR = vRect.TR;
	m_vertexBL_original = m_vertexBL = vRect.BL;
	m_vertexBR_original = m_vertexBR = vRect.BR;
}

bool CVertexPage::VerticesChanged()
{
	
	bool areEqual = AreVerticesSimilar(m_vertexTL_original, m_vertexTL, maxDif)
		&& AreVerticesSimilar(m_vertexTR_original, m_vertexTR, maxDif)
		&& AreVerticesSimilar(m_vertexBL_original, m_vertexBL, maxDif)
		&& AreVerticesSimilar(m_vertexBR_original, m_vertexBR, maxDif);

	return ! areEqual;
}

bool CVertexPage::AreVerticesUnique()
{
	if (AreVerticesSimilar(m_vertexTL, m_vertexTR, maxDif))
		return false;

	if (AreVerticesSimilar(m_vertexTL, m_vertexBL, maxDif))
		return false;

	if (AreVerticesSimilar(m_vertexTL, m_vertexBR, maxDif))
		return false;

	if (AreVerticesSimilar(m_vertexTR, m_vertexBL, maxDif))
		return false;

	if (AreVerticesSimilar(m_vertexTR, m_vertexBR, maxDif))
		return false;

	if (AreVerticesSimilar(m_vertexBL, m_vertexBR, maxDif))
		return false;

	return true;
}

void CVertexPage::UpdateAutoZoomCtrls()
{
	EnableCtrls();
}

VertexRect CVertexPage::GetVertices() const
{
	VertexRect vRect{ m_vertexTL, m_vertexTR, m_vertexBL, m_vertexBR };
	return vRect;
}

void CVertexPage::SetAutoZoom(bool autoZoom)
{
	m_autoZoom = autoZoom;
}

bool CVertexPage::GetAutoZoom() const
{
	return m_autoZoom;
}

void CVertexPage::SetAutoTranslate(bool autoTranslate)
{
	m_autoTranslate = autoTranslate;
}

bool CVertexPage::GetAutoTranslate() const
{
	return m_autoTranslate;
}

void CVertexPage::OnOK()
{
	UpdateData(TRUE);

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

void CVertexPage::OnKillFocusVertex()
{
	UpdateData(TRUE);
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
	m_autoZoom = FALSE;
	m_autoTranslate = FALSE;
	EnableCtrls();
	UpdateData(FALSE);
}

void CVertexPage::OnBnClickCustomVertexRad()
{
	m_vertexSource = VertexSource::CustomVertexPair;
	EnableCtrls();
	UpdateData(FALSE);
}

void CVertexPage::EnableCtrls()
{
	using namespace UiUtilities;

	bool allowCustomVertexes = AreVerticesUnique();

	bool customSelected = m_vertexSource == VertexSource::CustomVertexPair;

	if (!allowCustomVertexes)
	{
		m_vertexSource = VertexSource::Spherical;
		customSelected = false;
		m_autoZoom = FALSE;
		m_autoTranslate = FALSE;
	}

	SafeCheck(this, IDC_SPHERICAL_RAD, !customSelected);
	SafeEnable(this, IDC_SEED_TRIANGLES_COMBO, !customSelected);

	SafeCheck(this, IDC_CUSTOM_VERTEX_RAD, customSelected);
	SafeCheck(this, IDC_AUTO_ZOOM_CHECK, m_autoZoom);
	SafeCheck(this, IDC_AUTO_TRANSLATE_CHECK, m_autoTranslate);

	SafeEnable(this, IDC_CUSTOM_VERTEX_RAD, allowCustomVertexes);
	SafeEnable(this, IDC_AUTO_ZOOM_CHECK, allowCustomVertexes);
	SafeEnable(this, IDC_AUTO_TRANSLATE_CHECK, allowCustomVertexes);
}

bool CVertexPage::GetDefaultTargetAndWorkScale() const
{
	return m_defaultTargetAndWorldScale;
}