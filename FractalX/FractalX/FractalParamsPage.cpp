#include "stdafx.h"
#include "FractalX.h"
#include "FractalParamsPage.h"
#include "TraceParams.h"

#include "ModelSheet.h"

using namespace DXF;

IMPLEMENT_DYNAMIC(CFractalParamsPage, CMFCPropertyPage)

CFractalParamsPage::CFractalParamsPage()
	: CMFCPropertyPage(CFractalParamsPage::IDD)
	, m_bailout(2.0)
	, m_constantC(1.0)
	, m_power(8.0)
	, m_modelType(FractalTypeToInt(FractalType::StandardBulb))
{}

CFractalParamsPage::~CFractalParamsPage()
{}

void CFractalParamsPage::SetBailOut(double bailout)
{
	m_bailout = bailout;
}

double CFractalParamsPage::GetBailOut() const
{
	return m_bailout;
}

void CFractalParamsPage::SetConstantC(double constantC)
{
	m_constantC = constantC;
}

double CFractalParamsPage::GetConstantC() const
{
	return m_constantC;
}

void CFractalParamsPage::SetPower(double power)
{
	m_power = power;
}

double CFractalParamsPage::GetPower() const
{
	return m_power;
}

void CFractalParamsPage::SetModelType(FractalType fractalType)
{
	m_modelType = FractalTypeToInt(fractalType) - 1;
}

FractalType CFractalParamsPage::GetModelType() const
{
	return FractalTypeFromInt(m_modelType + 1);
}

BEGIN_MESSAGE_MAP(CFractalParamsPage, CMFCPropertyPage)
	ON_EN_KILLFOCUS(IDC_BAILOUT_EDIT, &CFractalParamsPage::OnKillfocusEdit)
	ON_EN_KILLFOCUS(IDC_CONSTANT_C_EDIT, &CFractalParamsPage::OnKillfocusEdit)
	ON_EN_KILLFOCUS(IDC_POWER_EDIT, &CFractalParamsPage::OnKillfocusEdit)
	ON_CBN_SELCHANGE(IDC_MODEL_TYPE_COMBO, &CFractalParamsPage::OnFractalTypeChanged)
END_MESSAGE_MAP()

void CFractalParamsPage::DoDataExchange(CDataExchange* pDX)
{
	CMFCPropertyPage::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_BAILOUT_EDIT, m_bailout);
	DDV_MinMaxDouble(pDX, m_bailout, 1, 10000);

	DDX_Text(pDX, IDC_CONSTANT_C_EDIT, m_constantC);
	DDV_MinMaxDouble(pDX, m_constantC, -10000, 10000);

	DDX_Text(pDX, IDC_POWER_EDIT, m_power);
	DDV_MinMaxDouble(pDX, m_power, 0, 10000);

	DDX_Control(pDX, IDC_MODEL_TYPE_COMBO, m_modelCombo);
	DDX_CBIndex(pDX, IDC_MODEL_TYPE_COMBO, m_modelType);
}

BOOL CFractalParamsPage::OnSetActive()
{
	InitializeFractalTypeCombo();

	return CPropertyPage::OnSetActive();
}

void CFractalParamsPage::InitializeFractalTypeCombo()
{
	auto pCombo = (CComboBox*)GetDlgItem(IDC_MODEL_TYPE_COMBO);
	if (!pCombo)
		return;

	pCombo->InsertString(0, FractalTypeString(FractalType::StandardBulb));
	pCombo->InsertString(1, FractalTypeString(FractalType::CartesianConvertAltX1));

	pCombo->SetCurSel(m_modelType);
}

void CFractalParamsPage::OnOK()
{
	CModelSheet* pModelSheet = dynamic_cast<CModelSheet*>(GetParent());
	if (pModelSheet)
		pModelSheet->OnOk();

	CMFCPropertyPage::OnOK();
}

void CFractalParamsPage::OnKillfocusEdit()
{
	UpdateData(TRUE);
}

void CFractalParamsPage::OnFractalTypeChanged()
{
	UpdateData(TRUE);
}