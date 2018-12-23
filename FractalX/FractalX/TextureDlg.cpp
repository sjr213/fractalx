
// This is no longer used - delete if there is a problem
#include "stdafx.h"
#include "TextureDlg.h"
#include "DxfColorFactory.h"

BEGIN_MESSAGE_MAP(CTextureDlg, CDialogEx)
END_MESSAGE_MAP()

CTextureDlg::CTextureDlg() : CDialogEx(IDD_TEXTURE_DLG)
	, m_selectedTextureId(-1)
{}

CTextureDlg::~CTextureDlg()
{}

void CTextureDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_TEXTURE_LIST, m_textureList);
}

int CTextureDlg::GetTextureId() const
{
	return m_selectedTextureId;
}

BOOL CTextureDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	int nTextures = DXF::GetNumberOfTextures();
	for (int i = 0; i < nTextures; ++i)
	{
		auto name = DXF::GetTextureName(i);
		m_textureList.AddString(name);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CTextureDlg::OnCancel()
{
	CDialogEx::OnCancel();
}

void CTextureDlg::OnOK()
{
	UpdateData(TRUE);

	int id = m_textureList.GetCurSel();
	if (id == CB_ERR)
		return;

	m_selectedTextureId = id;

	CDialogEx::OnOK();
}