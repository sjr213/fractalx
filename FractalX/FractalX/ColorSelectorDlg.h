#pragma once
#include "ColorArgb.h"

class CColorSelectorDlg : public CDialogEx
{
public:
	virtual ~CColorSelectorDlg() {};

	virtual DxColor::ColorArgb GetColor() = 0;

	static std::shared_ptr<CColorSelectorDlg> CreateColorSelectorDlg(DxColor::ColorArgb color, bool enableAlpha, CWnd* pParent = nullptr);

protected:
	CColorSelectorDlg(UINT nIDTemplate, CWnd* pParent)
		: CDialogEx(nIDTemplate, pParent)
	{}

};