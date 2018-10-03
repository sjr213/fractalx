#pragma once
#include "ColorContrast.h"

class CContrastDlg : public CDialogEx
{
public:
	virtual ~CContrastDlg() {};

	virtual DxColor::ColorContrast GetContrast() = 0;

	static std::shared_ptr<CContrastDlg> CreateContrastDlg(const DxColor::ColorContrast& contrast, CWnd* pParent = nullptr);

protected:
	CContrastDlg(UINT nIDTemplate, CWnd* pParent)
		: CDialogEx(nIDTemplate, pParent)
	{}

};