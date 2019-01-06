#include "stdafx.h"
#include "RotationDlg.h"

namespace DXF
{
	BEGIN_MESSAGE_MAP(CRotationDlg, CDialogEx)
		ON_EN_KILLFOCUS(IDC_X_ANGLE_EDIT, &CRotationDlg::OnKillfocusXAngleEdit)
		ON_EN_KILLFOCUS(IDC_Y_ANGLE_EDIT, &CRotationDlg::OnKillfocusYAngleEdit)
		ON_EN_KILLFOCUS(IDC_Z_ANGLE_EDIT, &CRotationDlg::OnKillfocusZAngleEdit)
	END_MESSAGE_MAP()

	CRotationDlg::CRotationDlg()
		: CDialogEx(IDD_ROTATION_DLG)
		, m_action(RotationActionToInt(RotationAction::RotateAll))
		, m_angleX(0.f)
		, m_angleY(0.f)
		, m_angleZ(0.f)
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
		DDV_MinMaxFloat(pDX, m_angleX, 0.0f, 360.0f);
		DDX_Text(pDX, IDC_Y_ANGLE_EDIT, m_angleY);
		DDV_MinMaxFloat(pDX, m_angleY, 0.0f, 360.0f);
		DDX_Text(pDX, IDC_Z_ANGLE_EDIT, m_angleZ);
		DDV_MinMaxFloat(pDX, m_angleZ, 0.0f, 360.0f);
	}


	BOOL CRotationDlg::OnInitDialog()
	{
		CDialogEx::OnInitDialog();

		// TODO:  Add extra initialization here

		return TRUE;  // return TRUE unless you set the focus to a control
					  // EXCEPTION: OCX Property Pages should return FALSE
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

	RotationParams CRotationDlg::GetRotationParams()
	{
		RotationParams rp(RotationActionFromInt(m_action), -1.0f * m_angleX, -1.0f * m_angleY, -1.0f * m_angleZ);

		return rp;
	}

	void CRotationDlg::SetRotationParams(RotationParams rp)
	{
		m_action = RotationActionToInt(rp.Action);
		m_angleX = -1.0f * rp.AngleXDegrees;
		m_angleY = -1.0f * rp.AngleYDegrees;
		m_angleZ = -1.0f * rp.AngleZDegrees;
	}
}

void DXF::CRotationDlg::OnKillfocusXAngleEdit()
{
	UpdateData(TRUE);
}


void DXF::CRotationDlg::OnKillfocusYAngleEdit()
{
	UpdateData(TRUE);
}

void DXF::CRotationDlg::OnKillfocusZAngleEdit()
{
	UpdateData(TRUE);
}
