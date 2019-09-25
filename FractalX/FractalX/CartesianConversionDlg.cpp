#include "stdafx.h"
#include "CartesianConversionDlg.h"

#include "CartesianConverterGroup.h"
#include "TrigOption.h"
#include "Resource.h"

using namespace DXF;
using namespace std;

namespace
{
	const double MIN_MULT = -1000.0;
	const double MAX_MULT = 1000.0;
}

class CartesianConversionDlgImp : public CCartesianConversionDlg
{
private:
	shared_ptr<CartesianConverterGroup> m_group;

	CComboBox m_thetaXCombo;
	CComboBox m_phiXCombo;
	CComboBox m_thetaYCombo;
	CComboBox m_phiYCombo;
	CComboBox m_thetaZCombo;
	CComboBox m_phiZCombo;

public:
	CartesianConversionDlgImp(std::shared_ptr<CartesianConverterGroup> group, CWnd* pParent)
		: CCartesianConversionDlg(IDD_CARTESIAN_CONVERSION_DLG, pParent)
		, m_group(group)
	{}

	virtual ~CartesianConversionDlgImp() {}

	shared_ptr<CartesianConverterGroup> GetConversionGroup() override
	{
		return m_group;
	}

protected:
	DECLARE_MESSAGE_MAP()

	BOOL OnInitDialog() override
	{
		CCartesianConversionDlg::OnInitDialog();

		InitializeCtrls();

		return TRUE;
	}

	void InitializeCtrls()
	{
		InitializeTrigOptionCombo(IDC_THETA_X_COMBO, m_group->ThetaOptionX);
		InitializeTrigOptionCombo(IDC_PHI_X_COMBO, m_group->PhiOptionX);
		InitializeTrigOptionCombo(IDC_THETA_Y_COMBO, m_group->ThetaOptionY);
		InitializeTrigOptionCombo(IDC_PHI_Y_COMBO, m_group->PhiOptionY);
		InitializeTrigOptionCombo(IDC_THETA_Z_COMBO, m_group->ThetaOptionZ);
		InitializeTrigOptionCombo(IDC_PHI_Z_COMBO, m_group->PhiOptionZ);
	}

	void InitializeTrigOptionCombo(UINT comboId, TrigOption opt)
	{
		auto pCombo = (CComboBox*)GetDlgItem(comboId);
		if (!pCombo)
			return;

		pCombo->InsertString(0, TrigOptionString(TrigOption::None));
		pCombo->InsertString(1, TrigOptionString(TrigOption::Sin));
		pCombo->InsertString(2, TrigOptionString(TrigOption::Cos));
		pCombo->InsertString(3, TrigOptionString(TrigOption::Tan));

		pCombo->SetCurSel(TrigOptionToInt(opt));
	}

	void DoDataExchange(CDataExchange* pDX) override
	{
		CCartesianConversionDlg::DoDataExchange(pDX);

		DDX_Text(pDX, IDC_MULTIPLIER_X, m_group->MultiplerX);
		DDV_MinMaxDouble(pDX, m_group->MultiplerX, MIN_MULT, MAX_MULT);
		DDX_Text(pDX, IDC_MULTIPLIER_Y, m_group->MultiplerY);
		DDV_MinMaxDouble(pDX, m_group->MultiplerY, MIN_MULT, MAX_MULT);
		DDX_Text(pDX, IDC_MULTIPLIER_Z, m_group->MultiplerZ);
		DDV_MinMaxDouble(pDX, m_group->MultiplerZ, MIN_MULT, MAX_MULT);

		int thetaX = TrigOptionToInt(m_group->ThetaOptionX);
		DDX_Control(pDX, IDC_THETA_X_COMBO, m_thetaXCombo);
		DDX_CBIndex(pDX, IDC_THETA_X_COMBO, thetaX);

		int phiX = TrigOptionToInt(m_group->PhiOptionX);
		DDX_Control(pDX, IDC_PHI_X_COMBO, m_phiXCombo);
		DDX_CBIndex(pDX, IDC_PHI_X_COMBO, phiX);

		int thetaY = TrigOptionToInt(m_group->ThetaOptionY);
		DDX_Control(pDX, IDC_THETA_Y_COMBO, m_thetaYCombo);
		DDX_CBIndex(pDX, IDC_THETA_Y_COMBO, thetaY);

		int phiY = TrigOptionToInt(m_group->PhiOptionY);
		DDX_Control(pDX, IDC_PHI_Y_COMBO, m_phiYCombo);
		DDX_CBIndex(pDX, IDC_PHI_Y_COMBO, phiY);

		int thetaZ = TrigOptionToInt(m_group->ThetaOptionZ);
		DDX_Control(pDX, IDC_THETA_Z_COMBO, m_thetaZCombo);
		DDX_CBIndex(pDX, IDC_THETA_Z_COMBO, thetaZ);

		int phiZ = TrigOptionToInt(m_group->PhiOptionZ);
		DDX_Control(pDX, IDC_PHI_Z_COMBO, m_phiZCombo);
		DDX_CBIndex(pDX, IDC_PHI_Z_COMBO, phiZ);

		if (pDX->m_bSaveAndValidate)
		{
			m_group->ThetaOptionX = TrigOptionFromInt(thetaX);
			m_group->PhiOptionX = TrigOptionFromInt(phiX);
			m_group->ThetaOptionY = TrigOptionFromInt(thetaY);
			m_group->PhiOptionY = TrigOptionFromInt(phiY);
			m_group->ThetaOptionZ = TrigOptionFromInt(thetaZ);
			m_group->PhiOptionZ = TrigOptionFromInt(phiZ);
		}
	}

	void OnOK() override
	{
		UpdateData(TRUE);

		CCartesianConversionDlg::OnOK();
	}

	void OnDefaultsClicked()
	{
		*m_group = CartesianConverterGroup();
		UpdateData(FALSE);
	}

};

BEGIN_MESSAGE_MAP(CartesianConversionDlgImp, CCartesianConversionDlg)
	ON_BN_CLICKED(IDC_DEFAULT_BUT, &CartesianConversionDlgImp::OnDefaultsClicked)
END_MESSAGE_MAP()


shared_ptr<CCartesianConversionDlg> CCartesianConversionDlg::CreateCartesianConversionDlg(const CartesianConverterGroup& ccg, CWnd* pParent)
{
	return make_shared<CartesianConversionDlgImp>(make_shared<CartesianConverterGroup>(ccg), pParent);
}