#pragma once

#include <afxdialogex.h>
#include "DxVersion.h"
#include <memory>

class CDxDlg : public CDialogEx
{
public:
	virtual ~CDxDlg() {};

	virtual DXF::DxVersion GetDxVersion() = 0;

	static std::shared_ptr<CDxDlg> CreateDxDlg(DXF::DxVersion dxVersion, CWnd* pParent = nullptr);

protected:
	CDxDlg(UINT nIDTemplate, CWnd* pParent)
		: CDialogEx(nIDTemplate, pParent)
	{}
};

