#pragma once

namespace DxColor
{
	struct ColorContrast;
	struct PinPalette;
}

class CPaletteSelectionDlg : public CDialogEx
{
public:
	virtual ~CPaletteSelectionDlg() {};

	virtual std::shared_ptr<DxColor::PinPalette> GetSelectedPalette() = 0;

	virtual DxColor::ColorContrast GetContrast() = 0;

	virtual void SetNewPaletteMethod(std::function<void(const DxColor::PinPalette&, const DxColor::ColorContrast&)> newPaletteMethod) = 0;

	static std::shared_ptr<CPaletteSelectionDlg> CreatePaletteSelectionDlg(const DxColor::ColorContrast& contrast, CWnd* pParent = nullptr);

protected:
	CPaletteSelectionDlg(UINT nIDTemplate, CWnd* pParent)
		: CDialogEx(nIDTemplate, pParent)
	{}

};