#pragma once

namespace DxColor
{
	struct PinPalette;
}

class CPaletteSelectionDlg : public CDialogEx
{
public:
	virtual ~CPaletteSelectionDlg() {};

	virtual std::shared_ptr<DxColor::PinPalette> GetSelectedPalette() = 0;

	static std::shared_ptr<CPaletteSelectionDlg> CreatePaletteSelectionDlg(CWnd* pParent = nullptr);

protected:
	CPaletteSelectionDlg(UINT nIDTemplate, CWnd* pParent)
		: CDialogEx(nIDTemplate, pParent)
	{}

};