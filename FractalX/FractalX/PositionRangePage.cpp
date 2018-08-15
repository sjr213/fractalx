
#include "stdafx.h"
#include "FractalX.h"
#include "PositionRangePage.h"

#include "ModelSheet.h"

using namespace DXF;

IMPLEMENT_DYNAMIC(CPositionRangePage, CMFCPropertyPage)

CPositionRangePage::CPositionRangePage()
	: CMFCPropertyPage(CPositionRangePage::IDD)
{}

CPositionRangePage::~CPositionRangePage()
{}

void CPositionRangePage::SetOrigin(const DirectX::SimpleMath::Vector3 origin)
{
	m_origin = origin;
}

DirectX::SimpleMath::Vector3 CPositionRangePage::GetOrigin() const
{
	return m_origin;
}

void CPositionRangePage::SetStretchParams(const DXF::StretchDistanceParams& stretchParams)
{
	m_stretchParams = stretchParams;
}

DXF::StretchDistanceParams CPositionRangePage::GetStretchParams() const
{
	return m_stretchParams;
}

BEGIN_MESSAGE_MAP(CPositionRangePage, CMFCPropertyPage)
	ON_EN_KILLFOCUS(IDC_ORIGIN_X_EDIT, &CPositionRangePage::OnKillfocusEdit)
	ON_EN_KILLFOCUS(IDC_ORIGIN_Y_EDIT, &CPositionRangePage::OnKillfocusEdit)
	ON_EN_KILLFOCUS(IDC_ORIGIN_Z_EDIT, &CPositionRangePage::OnKillfocusEdit)
	ON_EN_KILLFOCUS(IDC_MIN_DISTANCE_EDIT, &CPositionRangePage::OnKillfocusEdit)
	ON_EN_KILLFOCUS(IDC_MAX_DISTANCE_EDIT, &CPositionRangePage::OnKillfocusEdit)
	ON_BN_CLICKED(IDC_STRETCH_DISTANCE_CHECK, &CPositionRangePage::OnStretchClicked)
	ON_BN_CLICKED(IDC_ESTIMATE_MIN_MAX_DISTANCE_CHECK, &CPositionRangePage::OnStretchClicked)
END_MESSAGE_MAP()

void CPositionRangePage::DoDataExchange(CDataExchange* pDX)
{
	CMFCPropertyPage::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_ORIGIN_X_EDIT, m_origin.x);
	DDV_MinMaxFloat(pDX, m_origin.x, -5.0f, 5.0f);

	DDX_Text(pDX, IDC_ORIGIN_Y_EDIT, m_origin.y);
	DDV_MinMaxFloat(pDX, m_origin.y, -5.0f, 5.0f);

	DDX_Text(pDX, IDC_ORIGIN_Z_EDIT, m_origin.z);
	DDV_MinMaxFloat(pDX, m_origin.z, -5.0f, 5.0f);

	BOOL stretch = m_stretchParams.StretchDistance;
	DDX_Check(pDX, IDC_STRETCH_DISTANCE_CHECK, stretch);
	m_stretchParams.StretchDistance = stretch ? true : false;

	BOOL estimate = m_stretchParams.EstimateMinMax;
	DDX_Check(pDX, IDC_ESTIMATE_MIN_MAX_DISTANCE_CHECK, estimate);
	m_stretchParams.EstimateMinMax = estimate ? true : false;

	DDX_Text(pDX, IDC_MIN_DISTANCE_EDIT, m_stretchParams.MinDistance);
	DDV_MinMaxDouble(pDX, m_stretchParams.MinDistance, 0.0, 1000.0);

	DDX_Text(pDX, IDC_MAX_DISTANCE_EDIT, m_stretchParams.MaxDistance);
	DDV_MinMaxDouble(pDX, m_stretchParams.MaxDistance, m_stretchParams.MinDistance, 1000.0);
}

BOOL CPositionRangePage::OnSetActive()
{
	EnableStretchCtrls();

	return CPropertyPage::OnSetActive();
}

void CPositionRangePage::OnOK()
{
	CModelSheet* pModelSheet = dynamic_cast<CModelSheet*>(GetParent());
	if (pModelSheet)
		pModelSheet->OnOk();

	CMFCPropertyPage::OnOK();
}

void CPositionRangePage::OnKillfocusEdit()
{
	UpdateData(TRUE);
}

void CPositionRangePage::OnStretchClicked()
{
	UpdateData(TRUE);
	
	EnableStretchCtrls();
}

void CPositionRangePage::EnableStretchCtrls()
{
	bool enableAuto = m_stretchParams.StretchDistance;
	bool enableMinMax = enableAuto && ! m_stretchParams.EstimateMinMax;

	CWnd *pAuto = GetDlgItem(IDC_ESTIMATE_MIN_MAX_DISTANCE_CHECK);
	if (pAuto)
		pAuto->EnableWindow(enableAuto);

	CWnd *pMin = GetDlgItem(IDC_MIN_DISTANCE_EDIT);
	if (pMin)
		pMin->EnableWindow(enableMinMax);

	CWnd *pMax = GetDlgItem(IDC_MAX_DISTANCE_EDIT);
	if (pMax)
		pMax->EnableWindow(enableMinMax);
}
