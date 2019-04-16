
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

	float cx = std::get<0>(m_camera);
	DDX_Text(pDX, IDC_CAMERA_X_EDIT, cx);
	DDV_MinMaxFloat(pDX, cx, -10000.0f, 10000.0f);
	std::get<0>(m_camera) = cx;

	float cy = std::get<1>(m_camera);
	DDX_Text(pDX, IDC_CAMERA_Y_EDIT, cy);
	DDV_MinMaxFloat(pDX, cy, -10000.0f, 10000.0f);
	std::get<1>(m_camera) = cy;

	float cz = std::get<2>(m_camera);
	DDX_Text(pDX, IDC_CAMERA_Z_EDIT, cz);
	DDV_MinMaxFloat(pDX, cz, -10000.0f, 10000.0f);
	std::get<2>(m_camera) = cz;

	float x = std::get<0>(m_target);
	DDX_Text(pDX, IDC_TARGET_X_EDIT, x);
	DDV_MinMaxFloat(pDX, x, -10000.0f, 10000.0f);
	std::get<0>(m_target) = x;

	float y = std::get<1>(m_target);
	DDX_Text(pDX, IDC_TARGET_Y_EDIT, y);
	DDV_MinMaxFloat(pDX, y, -10000.0f, 10000.0f);
	std::get<1>(m_target) = y;

	float z = std::get<2>(m_target);
	DDX_Text(pDX, IDC_TARGET_Z_EDIT, z);
	DDV_MinMaxFloat(pDX, z, -10000.0f, 10000.0f);
	std::get<2>(m_target) = z;
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