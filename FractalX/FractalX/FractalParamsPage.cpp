#include "stdafx.h"
#include "CartesianConversionDlg.h"
#include "FractalX.h"
#include "FractalParamsPage.h"
#include "TraceParams.h"

#include "ModelSheet.h"


using namespace DXF;

IMPLEMENT_DYNAMIC(CFractalParamsPage, CMFCPropertyPage)

CFractalParamsPage::CFractalParamsPage()
	: CMFCPropertyPage(CFractalParamsPage::IDD)
	, m_bailout(2.0)
	, m_derivative(1.0)
	, m_power(8.0)
	, m_modelType(FractalTypeToInt(FractalType::StandardBulb))
	, m_cartesianType(CartesianConversionTypeToInt(CartesianConversionType::StandardConversion))
	, m_normalizationType(BulbNormalizeTypeToInt(BulbNormalizeType::StandardNormalization))
	, m_normalizationRoot(0.5)
	, m_cartesianGroup(new CartesianConverterGroup)
	, m_constantC(Vertex<double>(0.0,0.0,0.0))
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

void CFractalParamsPage::SetDerivative(double derivative)
{
	m_derivative = derivative;
}

double CFractalParamsPage::GetDerivative() const
{
	return m_derivative;
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

void CFractalParamsPage::SetCartesianConversionGroup(DXF::CartesianConverterGroup& group)
{
	*m_cartesianGroup = group;
}

std::shared_ptr<CartesianConverterGroup> CFractalParamsPage::GetCartesianConversionGroup()
{
	return m_cartesianGroup;
}

void CFractalParamsPage::SetConstantC(const Vertex<double>& constantC)
{
	m_constantC = constantC;
}

Vertex<double> CFractalParamsPage::GetConstantC() const
{
	return m_constantC;
}

BEGIN_MESSAGE_MAP(CFractalParamsPage, CMFCPropertyPage)
	ON_EN_KILLFOCUS(IDC_BAILOUT_EDIT, &CFractalParamsPage::OnKillfocusEdit)
	ON_EN_KILLFOCUS(IDC_CONSTANT_C_EDIT, &CFractalParamsPage::OnKillfocusEdit)
	ON_EN_KILLFOCUS(IDC_POWER_EDIT, &CFractalParamsPage::OnKillfocusEdit)
	ON_CBN_SELCHANGE(IDC_MODEL_TYPE_COMBO, &CFractalParamsPage::OnModelComboChanged)
	ON_CBN_SELCHANGE(IDC_CARTESIAN_COMBO, &CFractalParamsPage::OnComboChanged)
	ON_CBN_SELCHANGE(IDC_NORMALIZATION_COMBO, &CFractalParamsPage::OnComboChanged)
	ON_EN_KILLFOCUS(IDC_ROOT_EDIT, &CFractalParamsPage::OnKillfocusEdit)
	ON_BN_CLICKED(IDC_CUSTOM_CONVERSION_BUT, &CFractalParamsPage::OnCustomBut)
	ON_CBN_SELCHANGE(IDC_C_X_EDIT, &CFractalParamsPage::OnKillfocusEdit)
	ON_CBN_SELCHANGE(IDC_C_Y_EDIT, &CFractalParamsPage::OnKillfocusEdit)
	ON_CBN_SELCHANGE(IDC_C_Z_EDIT, &CFractalParamsPage::OnKillfocusEdit)
END_MESSAGE_MAP()

void CFractalParamsPage::DoDataExchange(CDataExchange* pDX)
{
	CMFCPropertyPage::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_BAILOUT_EDIT, m_bailout);
	DDV_MinMaxDouble(pDX, m_bailout, 1, 10000);

	DDX_Text(pDX, IDC_CONSTANT_C_EDIT, m_derivative);
	DDV_MinMaxDouble(pDX, m_derivative, -10000, 10000);

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

	DDX_Text(pDX, IDC_C_X_EDIT, m_constantC.X);
	DDV_MinMaxDouble(pDX, m_constantC.X, -100000.0, 100000.0);

	DDX_Text(pDX, IDC_C_Y_EDIT, m_constantC.Y);
	DDV_MinMaxDouble(pDX, m_constantC.Y, -100000.0, 100000.0);

	DDX_Text(pDX, IDC_C_Z_EDIT, m_constantC.Z);
	DDV_MinMaxDouble(pDX, m_constantC.Z, -100000.0, 100000.0);
}

BOOL CFractalParamsPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	InitializeFractalTypeCombo();
	InitializeCartesianTypeCombo();
	InitializeNormalizationTypeCombo();

	return TRUE;
}

BOOL CFractalParamsPage::OnSetActive()
{
	EnableCtrls();

	return CPropertyPage::OnSetActive();
}

void CFractalParamsPage::EnableCtrls()
{
	bool stdBulb = FractalTypeFromInt(m_modelType+1) == FractalType::StandardBulb;

	CWnd* pCartesianTypeCombo = GetDlgItem(IDC_CARTESIAN_COMBO);
	if (pCartesianTypeCombo)
		pCartesianTypeCombo->EnableWindow(stdBulb);

	CWnd* pCustomeBut = GetDlgItem(IDC_CUSTOM_CONVERSION_BUT);
	if (pCustomeBut)
		pCustomeBut->EnableWindow(!stdBulb);
}

void CFractalParamsPage::InitializeFractalTypeCombo()
{
	auto pCombo = (CComboBox*)GetDlgItem(IDC_MODEL_TYPE_COMBO);
	if (!pCombo)
		return;

	pCombo->InsertString(0, FractalTypeString(FractalType::StandardBulb));
	pCombo->InsertString(1, FractalTypeString(FractalType::DoubleBulb));
	pCombo->InsertString(2, FractalTypeString(FractalType::InglesFractal));
	pCombo->InsertString(3, FractalTypeString(FractalType::InglesFractal2));

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

	pCombo->SetCurSel(m_normalizationType);
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

void CFractalParamsPage::OnModelComboChanged()
{
	UpdateData(TRUE);
	EnableCtrls();
}

void CFractalParamsPage::OnCustomBut()
{
	auto pDlg = CCartesianConversionDlg::CreateCartesianConversionDlg(*m_cartesianGroup, this);

	if (pDlg->DoModal() == IDOK)
	{
		m_cartesianGroup = pDlg->GetConversionGroup();
	}
}