
#include "stdafx.h"
#include "BackgroundPage.h"

#include "ModelSheet.h"
#include "UiUtilities.h"

using namespace DXF;

IMPLEMENT_DYNAMIC(CBackgroundPage, CMFCPropertyPage)

BEGIN_MESSAGE_MAP(CBackgroundPage, CMFCPropertyPage)
	ON_BN_CLICKED(IDC_BROWSE_BUT, &CBackgroundPage::OnBrowseBut)
	ON_BN_CLICKED(IDC_SHOW_BACKGROUND_CHECK, &CBackgroundPage::OnShowClicked)
	ON_BN_CLICKED(IDC_DEFAULT_BUT, &CBackgroundPage::DefaultModelClicked)
END_MESSAGE_MAP()

CBackgroundPage::CBackgroundPage()
	: CMFCPropertyPage(CBackgroundPage::IDD)
	, m_backgroundModel(4)
{}

void CBackgroundPage::SetFilename(const std::wstring& filename)
{
	m_filename = filename.c_str();
}

std::wstring CBackgroundPage::GetFilename() const
{
	std::wstring str(m_filename.GetString());
	return str;
}

void CBackgroundPage::SetShowBackground(bool show)
{
	m_showBackground = show;
}

bool CBackgroundPage::GetShowBackground() const
{
	return m_showBackground;
}

void CBackgroundPage::SetModelVertices(const std::vector<DXF::Vertex<float>>& model)
{
	if (model.size() != 4)
	{
		assert(false);
		return;
	}
	m_backgroundModel = model;
}

std::vector<DXF::Vertex<float>> CBackgroundPage::GetModelVertices() const
{
	return m_backgroundModel;
}

void CBackgroundPage::DoDataExchange(CDataExchange* pDX)
{
	CMFCPropertyPage::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_IMAGE_FILE_EDIT, m_filename);

	BOOL show = m_showBackground;
	DDX_Check(pDX, IDC_SHOW_BACKGROUND_CHECK, show);
	m_showBackground = show;

	DDX_Text(pDX, IDC_V1_X_EDIT, m_backgroundModel.at(0).X);
	DDX_Text(pDX, IDC_V1_Y_EDIT, m_backgroundModel.at(0).Y);
	DDX_Text(pDX, IDC_V1_Z_EDIT, m_backgroundModel.at(0).Z);

	DDX_Text(pDX, IDC_V2_X_EDIT, m_backgroundModel.at(1).X);
	DDX_Text(pDX, IDC_V2_Y_EDIT, m_backgroundModel.at(1).Y);
	DDX_Text(pDX, IDC_V2_Z_EDIT, m_backgroundModel.at(1).Z);

	DDX_Text(pDX, IDC_V3_X_EDIT, m_backgroundModel.at(2).X);
	DDX_Text(pDX, IDC_V3_Y_EDIT, m_backgroundModel.at(2).Y);
	DDX_Text(pDX, IDC_V3_Z_EDIT, m_backgroundModel.at(2).Z);

	DDX_Text(pDX, IDC_V4_X_EDIT, m_backgroundModel.at(3).X);
	DDX_Text(pDX, IDC_V4_Y_EDIT, m_backgroundModel.at(3).Y);
	DDX_Text(pDX, IDC_V4_Z_EDIT, m_backgroundModel.at(3).Z);
}

BOOL CBackgroundPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	EnableCtrls();

	return TRUE;
}

BOOL CBackgroundPage::OnSetActive()
{
	return CPropertyPage::OnSetActive();
}

void CBackgroundPage::OnOK()
{
	CModelSheet* pModelSheet = dynamic_cast<CModelSheet*>(GetParent());
	if (pModelSheet)
		pModelSheet->OnOk();

	CMFCPropertyPage::OnOK();
}

void CBackgroundPage::OnBrowseBut()
{
	CString fileName = m_filename;
	CString fileType = _T("Bitmap Files (*.bmp)|*.bmp|JPEG Files (*.jpg)|*.jpg|PNG files (*.png)|*.png|All Files (*.*)|*.*||");
	CString fileExt = _T(".bmp");

	// Open a Save As dialog to get a name
	CFileDialog MyOpenDlg(TRUE, fileExt, NULL, OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, fileType);
	if (MyOpenDlg.DoModal() != IDOK)
		return;

	m_filename = MyOpenDlg.GetPathName();
	UpdateData(FALSE);
}

BOOL CBackgroundPage::OnKillActive()
{
	UpdateData(TRUE);
	if (m_showBackground && ! m_filename.IsEmpty() && ! UiUtilities::FileExists(m_filename))
	{
		AfxMessageBox(_T("Current file is not valid!"));
		return FALSE;
	}

	return CMFCPropertyPage::OnKillActive();
}

void CBackgroundPage::EnableCtrls()
{
	UiUtilities::SafeEnable(this, IDC_BROWSE_BUT, m_showBackground);
	UiUtilities::SafeEnable(this, IDC_IMAGE_FILE_EDIT, m_showBackground);
}

void CBackgroundPage::OnShowClicked()
{
	UpdateData(TRUE);
	EnableCtrls();
}

void CBackgroundPage::DefaultModelClicked()
{
	m_backgroundModel = GetDefaultBackgroundModel();
	UpdateData(FALSE);
}