// CPerspectiveDlg.cpp : implementation file
//

#include "stdafx.h"
#include "FractalX.h"
#include "PerspectiveDlg.h"
#include "afxdialogex.h"


// CPerspectiveDlg dialog

IMPLEMENT_DYNAMIC(CPerspectiveDlg, CDialogEx)

CPerspectiveDlg::CPerspectiveDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_PERSPECTIVE_DLG, pParent)
	, m_nearDistance(0.1f)
	, m_farDistance(10.0f)
{

}

CPerspectiveDlg::~CPerspectiveDlg()
{
}

void CPerspectiveDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_NEAR_EDIT, m_nearDistance);
	DDV_MinMaxFloat(pDX, m_nearDistance, 0.000001f, 100.0f);

	DDX_Text(pDX, IDC_FAR_EDIT, m_farDistance);
	DDV_MinMaxFloat(pDX, m_farDistance, 0.000001f, 100.0f);
}


BEGIN_MESSAGE_MAP(CPerspectiveDlg, CDialogEx)
	ON_EN_KILLFOCUS(IDC_NEAR_EDIT, &CPerspectiveDlg::OnKillfocusNearEdit)
	ON_EN_KILLFOCUS(IDC_FAR_EDIT, &CPerspectiveDlg::OnKillfocusFarEdit)
END_MESSAGE_MAP()

void CPerspectiveDlg::SetNear(float nearDistance)
{
	m_nearDistance = nearDistance;
}

void CPerspectiveDlg::SetFarDistance(float farDistance)
{
	m_farDistance = farDistance;
}

float CPerspectiveDlg::GetNearDistance() const
{
	return m_nearDistance;
}

float CPerspectiveDlg::GetFarDistance() const
{
	return m_farDistance;
}

void CPerspectiveDlg::OnKillfocusNearEdit()
{
	UpdateData(TRUE);
}

void CPerspectiveDlg::OnKillfocusFarEdit()
{
	UpdateData(TRUE);
}

// CPerspectiveDlg message handlers
