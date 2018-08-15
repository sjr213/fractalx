
#include "stdafx.h"
#include "FractalX.h"
#include "ViewDlg.h"
#include "afxdialogex.h"


IMPLEMENT_DYNAMIC(CViewDlg, CDialogEx)

CViewDlg::CViewDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_VIEW_DLG, pParent)
{
}

CViewDlg::~CViewDlg()
{
}

std::tuple<float, float, float> CViewDlg::GetCamera() const
{
	return m_camera;
}

std::tuple<float, float, float> CViewDlg::GetTarget() const
{
	return m_target;
}

void CViewDlg::SetCamera(const std::tuple<float, float, float>& camera)
{
	m_camera = camera;
}

void CViewDlg::SetTarget(const std::tuple<float, float, float>& target)
{
	m_target = target;
}

void CViewDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_CAMERA_X_EDIT, std::get<0>(m_camera));
	DDV_MinMaxFloat(pDX, std::get<0>(m_camera), -10000.0f, 10000.0f);

	DDX_Text(pDX, IDC_CAMERA_Y_EDIT, std::get<1>(m_camera));
	DDV_MinMaxFloat(pDX, std::get<1>(m_camera), -10000.0f, 10000.0f);

	DDX_Text(pDX, IDC_CAMERA_Z_EDIT, std::get<2>(m_camera));
	DDV_MinMaxFloat(pDX, std::get<2>(m_camera), -10000.0f, 10000.0f);

	DDX_Text(pDX, IDC_TARGET_X_EDIT, std::get<0>(m_target));
	DDV_MinMaxFloat(pDX, std::get<0>(m_target), -10000.0f, 10000.0f);

	DDX_Text(pDX, IDC_TARGET_Y_EDIT, std::get<1>(m_target));
	DDV_MinMaxFloat(pDX, std::get<1>(m_target), -10000.0f, 10000.0f);

	DDX_Text(pDX, IDC_TARGET_Z_EDIT, std::get<2>(m_target));
	DDV_MinMaxFloat(pDX, std::get<2>(m_target), -10000.0f, 10000.0f);
}


BEGIN_MESSAGE_MAP(CViewDlg, CDialogEx)
	ON_EN_KILLFOCUS(IDC_CAMERA_X_EDIT, &CViewDlg::OnKillfocusEdit)
	ON_EN_KILLFOCUS(IDC_CAMERA_Y_EDIT, &CViewDlg::OnKillfocusEdit)
	ON_EN_KILLFOCUS(IDC_CAMERA_Z_EDIT, &CViewDlg::OnKillfocusEdit)
	ON_EN_KILLFOCUS(IDC_TARGET_X_EDIT, &CViewDlg::OnKillfocusEdit)
	ON_EN_KILLFOCUS(IDC_TARGET_Y_EDIT, &CViewDlg::OnKillfocusEdit)
	ON_EN_KILLFOCUS(IDC_TARGET_Z_EDIT, &CViewDlg::OnKillfocusEdit)
END_MESSAGE_MAP()

void CViewDlg::OnKillfocusEdit()
{
	UpdateData(TRUE);
}