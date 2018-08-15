#pragma once

#include "ColorPin.h"


class CPaletteViewDlg : public CDialogEx
{
public:
	virtual ~CPaletteViewDlg() {};

	virtual void SetNewPaletteMethod(std::function<void(const DxColor::PinPalette&)> newPaletteMethod) = 0;

	static std::shared_ptr<CPaletteViewDlg> CreatePaletteViewDlg(const DxColor::PinPalette& palette, CWnd* pParent = nullptr);

protected:
	CPaletteViewDlg(UINT nIDTemplate, CWnd* pParent)
		: CDialogEx(nIDTemplate, pParent)
	{}

};