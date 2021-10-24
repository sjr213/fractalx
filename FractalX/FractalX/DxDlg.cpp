#include "stdafx.h"
#include "DxDlg.h"

#include "DxVersion.h"
#include "Resource.h"

using namespace DXF;

class CDxDlgImpl : public CDxDlg
{
private:
	DxVersion m_dxVersion;
	CComboBox m_dxVersionCombo;

public:
	CDxDlgImpl(DxVersion dxVersion, CWnd* pParent)
		: CDxDlg(IDD_DX_DLG, pParent)
		, m_dxVersion(dxVersion)

	{}

	DxVersion GetDxVersion() override
	{
		return m_dxVersion;
	}

protected:
	DECLARE_MESSAGE_MAP()

	BOOL OnInitDialog() override
	{
		CDxDlg::OnInitDialog();

		PopulateDxVersionCombo();

		return TRUE;
	}

	void PopulateDxVersionCombo()
	{
		m_dxVersionCombo.ResetContent();

		m_dxVersionCombo.AddString(GetDxVersionString(DxVersion::Dx11));
		m_dxVersionCombo.AddString(GetDxVersionString(DxVersion::Dx12));

		m_dxVersionCombo.SetCurSel(GetIndexForDxVersion(m_dxVersion));
	}

	void DoDataExchange(CDataExchange* pDX) override
	{
		CDxDlg::DoDataExchange(pDX);
		DDX_Control(pDX, IDC_DX_VERSION_COMBO, m_dxVersionCombo);
	}

	void OnCbnSelchangeDxVersionCombo()
	{
		int index = m_dxVersionCombo.GetCurSel();
		if (index == CB_ERR)
		{
			m_dxVersion = DxVersion::Dx11;
			m_dxVersionCombo.SetCurSel(GetIndexForDxVersion(m_dxVersion));
		}
		else
		{
			m_dxVersion = GetDxVersionFromIndex(index);
		}

		UpdateData(FALSE);
	}

	void OnOK() override
	{
		UpdateData(TRUE);

		CDialogEx::OnOK();
	}
};

BEGIN_MESSAGE_MAP(CDxDlgImpl, CDxDlg)
	ON_CBN_SELCHANGE(IDC_DX_VERSION_COMBO, &CDxDlgImpl::OnCbnSelchangeDxVersionCombo)
END_MESSAGE_MAP()

std::shared_ptr<CDxDlg> CDxDlg::CreateDxDlg(DxVersion dxVersion, CWnd* pParent)
{
	return std::make_shared<CDxDlgImpl>(dxVersion, pParent);
}