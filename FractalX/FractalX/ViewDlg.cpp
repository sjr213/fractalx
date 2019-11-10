
#include "stdafx.h"
#include "FractalX.h"
#include "ViewDlg.h"
#include "afxdialogex.h"

using namespace DXF;


IMPLEMENT_DYNAMIC(CViewDlg, CDialogEx)

CViewDlg::CViewDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_VIEW_DLG, pParent)
{
}

CViewDlg::~CViewDlg()
{
}

Vertex<float> CViewDlg::GetCamera() const
{
	return m_camera;
}

Vertex<float> CViewDlg::GetTarget() const
{
	return m_target;
}

void CViewDlg::SetCamera(const Vertex<float>& camera)
{
	m_camera = camera;
}

void CViewDlg::SetTarget(const Vertex<float>& target)
{
	m_target = target;
}

void CViewDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_CAMERA_X_EDIT, m_camera.X);
	DDV_MinMaxFloat(pDX, m_camera.X, -10000.0f, 10000.0f);

	DDX_Text(pDX, IDC_CAMERA_Y_EDIT, m_camera.Y);
	DDV_MinMaxFloat(pDX, m_camera.Y, -10000.0f, 10000.0f);

	DDX_Text(pDX, IDC_CAMERA_Z_EDIT, m_camera.Z);
	DDV_MinMaxFloat(pDX, m_camera.Z, -10000.0f, 10000.0f);

	DDX_Text(pDX, IDC_TARGET_X_EDIT, m_target.X);
	DDV_MinMaxFloat(pDX, m_target.X, -10000.0f, 10000.0f);

	DDX_Text(pDX, IDC_TARGET_Y_EDIT, m_target.Y);
	DDV_MinMaxFloat(pDX, m_target.Y, -10000.0f, 10000.0f);

	DDX_Text(pDX, IDC_TARGET_Z_EDIT, m_target.Z);
	DDV_MinMaxFloat(pDX, m_target.Z, -10000.0f, 10000.0f);
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