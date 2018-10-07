#pragma once

#include "ColorPin.h"

namespace DxColor
{
	struct ColorContrast;
}

class CPaletteViewDlg : public CDialogEx
{
public:
	virtual ~CPaletteViewDlg() {};

	virtual void SetNewPaletteMethod(std::function<void(const DxColor::PinPalette&)> newPaletteMethod) = 0;

	static std::shared_ptr<CPaletteViewDlg> CreatePaletteViewDlg(const DxColor::PinPalette& palette, DxColor::ColorContrast& contrast, CWnd* pParent = nullptr);

protected:
	CPaletteViewDlg(UINT nIDTemplate, CWnd* pParent)
		: CDialogEx(nIDTemplate, pParent)
	{}

};