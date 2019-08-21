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
	, m_cartesianType(CartesianConversionTypeToInt(CartesianConversionType::StandardConversion))
	, m_normalizationType(BulbNormalizeTypeToInt(BulbNormalizeType::StandardNormalization))
	, m_normalizationRoot(0.5)
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

void CFractalParamsPage::SetCartesianType(DXF::CartesianConversionType cartesianType)
{
	m_cartesianType = CartesianConversionTypeToInt(cartesianType) - 1;
}

DXF::CartesianConversionType CFractalParamsPage::GetCartesianType() const
{
	return CartesianConversionTypeFromInt(m_cartesianType + 1);
}

void CFractalParamsPage::SetNormalizationType(DXF::BulbNormalizeType normalizationType)
{
	m_normalizationType = BulbNormalizeTypeToInt(normalizationType) - 1;
}

DXF::BulbNormalizeType CFractalParamsPage::GetNormalizationType() const
{
	return BulbNormalizeTypeFromInt(m_normalizationType + 1);
}

void CFractalParamsPage::SetNormalizationRoot(double root)
{
	m_normalizationRoot = root;
}

double CFractalParamsPage::GetNormalizationRoot() const
{
	return m_normalizationRoot;
}

BEGIN_MESSAGE_MAP(CFractalParamsPage, CMFCPropertyPage)
	ON_EN_KILLFOCUS(IDC_BAILOUT_EDIT, &CFractalParamsPage::OnKillfocusEdit)
	ON_EN_KILLFOCUS(IDC_CONSTANT_C_EDIT, &CFractalParamsPage::OnKillfocusEdit)
	ON_EN_KILLFOCUS(IDC_POWER_EDIT, &CFractalParamsPage::OnKillfocusEdit)
	ON_CBN_SELCHANGE(IDC_MODEL_TYPE_COMBO, &CFractalParamsPage::OnComboChanged)
	ON_CBN_SELCHANGE(IDC_CARTESIAN_COMBO, &CFractalParamsPage::OnComboChanged)
	ON_CBN_SELCHANGE(IDC_NORMALIZATION_COMBO, &CFractalParamsPage::OnComboChanged)
	ON_EN_KILLFOCUS(IDC_ROOT_EDIT, &CFractalParamsPage::OnKillfocusEdit)
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

	DDX_Control(pDX, IDC_CARTESIAN_COMBO, m_cartesianCombo);
	DDX_CBIndex(pDX, IDC_CARTESIAN_COMBO, m_cartesianType);

	DDX_Control(pDX, IDC_NORMALIZATION_COMBO, m_normalizationCombo);
	DDX_CBIndex(pDX, IDC_NORMALIZATION_COMBO, m_normalizationType);

	DDX_Text(pDX, IDC_ROOT_EDIT, m_normalizationRoot);
	DDV_MinMaxDouble(pDX, m_normalizationRoot, 0.001, 10);
}

BOOL CFractalParamsPage::OnSetActive()
{
	InitializeFractalTypeCombo();
	InitializeCartesianTypeCombo();
	InitializeNormalizationTypeCombo();

	return CPropertyPage::OnSetActive();
}

void CFractalParamsPage::InitializeFractalTypeCombo()
{
	auto pCombo = (CComboBox*)GetDlgItem(IDC_MODEL_TYPE_COMBO);
	if (!pCombo)
		return;

	pCombo->InsertString(0, FractalTypeString(FractalType::StandardBulb));

	pCombo->SetCurSel(m_modelType);
}


void CFractalParamsPage::InitializeCartesianTypeCombo()
{
	auto pCombo = (CComboBox*)GetDlgItem(IDC_CARTESIAN_COMBO);
	if (!pCombo)
		return;

	pCombo->InsertString(0, CartesianConversionTypeString(CartesianConversionType::StandardConversion));
	pCombo->InsertString(1, CartesianConversionTypeString(CartesianConversionType::CartesianConvertAltX1));
	pCombo->InsertString(2, CartesianConversionTypeString(CartesianConversionType::CartesianConvertAltX2));
	pCombo->InsertString(3, CartesianConversionTypeString(CartesianConversionType::CartesianConvertAltY1));
	pCombo->InsertString(4, CartesianConversionTypeString(CartesianConversionType::CartesianConvertAltZ1));

	pCombo->SetCurSel(m_cartesianType);
}

void CFractalParamsPage::InitializeNormalizationTypeCombo()
{
	auto pCombo = (CComboBox*)GetDlgItem(IDC_NORMALIZATION_COMBO);
	if (!pCombo)
		return;

	pCombo->InsertString(0, BulbNormalizeTypeString(BulbNormalizeType::StandardNormalization));
	pCombo->InsertString(1, BulbNormalizeTypeString(BulbNormalizeType::AltRoots));

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

void CFractalParamsPage::OnComboChanged()
{
	UpdateData(TRUE);
}