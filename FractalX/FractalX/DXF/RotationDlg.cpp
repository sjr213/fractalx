#include "stdafx.h"
#include "RotationDlg.h"

namespace DXF
{
	BEGIN_MESSAGE_MAP(CRotationDlg, CDialogEx)
		ON_EN_KILLFOCUS(IDC_X_ANGLE_EDIT, &CRotationDlg::OnKillfocusXAngleEdit)
		ON_EN_KILLFOCUS(IDC_Y_ANGLE_EDIT, &CRotationDlg::OnKillfocusYAngleEdit)
		ON_EN_KILLFOCUS(IDC_Z_ANGLE_EDIT, &CRotationDlg::OnKillfocusZAngleEdit)
		ON_CBN_SELCHANGE(IDC_SPEED_COMBO, &CRotationDlg::OnSpeedChanged)
	END_MESSAGE_MAP()

	CRotationDlg::CRotationDlg()
		: CDialogEx(IDD_ROTATION_DLG)
		, m_action(RotationActionToInt(RotationAction::RotateAll))
		, m_angleX(0.f)
		, m_angleY(0.f)
		, m_angleZ(0.f)
		, m_speed(1)
		, m_rotateBackgroundWithMain(false)
	{
	}


	CRotationDlg::~CRotationDlg()
	{
	}

	void CRotationDlg::DoDataExchange(CDataExchange* pDX)
	{
		CDialogEx::DoDataExchange(pDX);
		DDX_Radio(pDX, IDC_FIXED_RAD, m_action);	
		DDX_Text(pDX, IDC_X_ANGLE_EDIT, m_angleX);
		DDV_MinMaxFloat(pDX, m_angleX, -360.0f, 360.0f);
		DDX_Text(pDX, IDC_Y_ANGLE_EDIT, m_angleY);
		DDV_MinMaxFloat(pDX, m_angleY, -360.0f, 360.0f);
		DDX_Text(pDX, IDC_Z_ANGLE_EDIT, m_angleZ);
		DDV_MinMaxFloat(pDX, m_angleZ, -360.0f, 360.0f);

		DDX_Control(pDX, IDC_SPEED_COMBO, m_SpeedCombo);
		DDX_CBIndex(pDX, IDC_SPEED_COMBO, m_speed);

		BOOL mapToMain = m_rotateBackgroundWithMain;
		DDX_Check(pDX, IDC_COPY_MAIN_TO_BKGND_CHECK, mapToMain);
		m_rotateBackgroundWithMain = mapToMain;
	}


	BOOL CRotationDlg::OnInitDialog()
	{
		CDialogEx::OnInitDialog();

		InitializeSpeedCombo();

		return TRUE;  // return TRUE unless you set the focus to a control
					  // EXCEPTION: OCX Property Pages should return FALSE
	}

	void CRotationDlg::InitializeSpeedCombo()
	{
		auto pCombo = (CComboBox*)GetDlgItem(IDC_SPEED_COMBO);
		if (!pCombo)
			return;

		pCombo->InsertString(0, _T("Slow"));
		pCombo->InsertString(1, _T("Medium"));
		pCombo->InsertString(2, _T("Fast"));

		pCombo->SetCurSel(m_speed);
	}


	void CRotationDlg::OnCancel()
	{
		// TODO: Add your specialized code here and/or call the base class

		CDialogEx::OnCancel();
	}


	void CRotationDlg::OnOK()
	{
		UpdateData(TRUE);

		CDialogEx::OnOK();
	}

	RotationGroup CRotationDlg::GetRotationGroup()
	{
		RotationParams rp(RotationActionFromInt(m_action), -1.0f * m_angleX, -1.0f * m_angleY, -1.0f * m_angleZ);
		rp.Speed = RotationSpeedFromInt(m_speed);
		m_rotationGroup.RotationParamsMain = rp;
		if (m_rotateBackgroundWithMain)
			m_rotationGroup.RotationType = RotationSelectionType::LockBackgroundOnModel;

		return m_rotationGroup;
	}

	void CRotationDlg::SetRotationGroup(RotationGroup rg)
	{
		m_rotationGroup = rg;
		m_action = RotationActionToInt(rg.RotationParamsMain.Action);
		m_angleX = -1.0f * rg.RotationParamsMain.AngleXDegrees;
		m_angleY = -1.0f * rg.RotationParamsMain.AngleYDegrees;
		m_angleZ = -1.0f * rg.RotationParamsMain.AngleZDegrees;
		m_speed = RotationSpeedToInt(rg.RotationParamsMain.Speed);
		m_rotateBackgroundWithMain = rg.RotationType == RotationSelectionType::LockBackgroundOnModel;
	}

	void CRotationDlg::OnKillfocusXAngleEdit()
	{
		UpdateData(TRUE);
	}


	void CRotationDlg::OnKillfocusYAngleEdit()
	{
		UpdateData(TRUE);
	}

	void CRotationDlg::OnKillfocusZAngleEdit()
	{
		UpdateData(TRUE);
	}

	void CRotationDlg::OnSpeedChanged()
	{
		UpdateData(TRUE);
	}
}




