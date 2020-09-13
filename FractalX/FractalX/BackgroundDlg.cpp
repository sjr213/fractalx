#include "stdafx.h"
#include "BackgroundDlg.h"

#include "DefaultFields.h"
#include "Resource.h"
#include "UiUtilities.h"

using namespace DXF;
using namespace std;

class CBackgroundDlgImp : public CBackgroundDlg
{
private:
	CString m_filename;
	bool m_showBackground = false;
	std::vector<DXF::Vertex<float>> m_backgroundModel;

public:
	CBackgroundDlgImp(CWnd* pParent)
		: CBackgroundDlg(IDD_BACKGROUND_PAGE, pParent)
		, m_backgroundModel(4)
	{}

	virtual ~CBackgroundDlgImp()
	{}

	void SetFilename(const std::wstring& filename) override
	{
		m_filename = filename.c_str();
	}

	std::wstring GetFilename() const override
	{
		std::wstring str(m_filename.GetString());
		return str;
	}

	void SetShowBackground(bool show) override
	{
		m_showBackground = show;
	}

	bool GetShowBackground() const override
	{
		return m_showBackground;
	}

	void SetModelVertices(const std::vector<DXF::Vertex<float>>& model) override
	{
		if (model.size() != 4)
		{
			assert(false);
			return;
		}
		m_backgroundModel = model;
	}

	std::vector<DXF::Vertex<float>> GetModelVertices() const override
	{
		return m_backgroundModel;
	}

	void DoDataExchange(CDataExchange* pDX)
	{
		CDialogEx::DoDataExchange(pDX);

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

	BOOL OnInitDialog()
	{
		EnableWindow(TRUE);
		ShowWindow(SW_SHOW);

		CDialogEx::OnInitDialog();

		UiUtilities::SafeShow(this, IDOK, SW_SHOW);
		UiUtilities::SafeEnable(this, IDOK, true);
		UiUtilities::SafeShow(this, IDCANCEL, SW_SHOW);
		UiUtilities::SafeEnable(this, IDCANCEL, true);

		EnableCtrls();

		return TRUE;
	}

	void OnBrowseBut()
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

	void OnOK() override
	{
		UpdateData(TRUE);
		if (m_showBackground && !m_filename.IsEmpty() && !UiUtilities::FileExists(m_filename))
		{
			AfxMessageBox(_T("Current file is not valid!"));
			return;
		}

		CDialogEx::OnOK();
	}

protected:
	DECLARE_MESSAGE_MAP()

	void EnableCtrls()
	{
		UiUtilities::SafeEnable(this, IDC_BROWSE_BUT, m_showBackground);
		UiUtilities::SafeEnable(this, IDC_IMAGE_FILE_EDIT, m_showBackground);
	}

	void OnShowClicked()
	{
		UpdateData(TRUE);
		EnableCtrls();
	}

	void DefaultModelClicked()
	{
		m_backgroundModel = GetDefaultBackgroundModel();
		UpdateData(FALSE);
	}
};

BEGIN_MESSAGE_MAP(CBackgroundDlgImp, CBackgroundDlg)
	ON_BN_CLICKED(IDC_BROWSE_BUT, &CBackgroundDlgImp::OnBrowseBut)
	ON_BN_CLICKED(IDC_SHOW_BACKGROUND_CHECK, &CBackgroundDlgImp::OnShowClicked)
	ON_BN_CLICKED(IDC_DEFAULT_BUT, &CBackgroundDlgImp::DefaultModelClicked)
END_MESSAGE_MAP()


shared_ptr<CBackgroundDlg> CBackgroundDlg::CreateBackgroundDlg(CWnd* pParent)
{
	return make_shared<CBackgroundDlgImp>(pParent);
}