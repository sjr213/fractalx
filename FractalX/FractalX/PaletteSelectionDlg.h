#pragma once

class CPaletteSelectionDlg : public CDialogEx
{
public:
	virtual ~CPaletteSelectionDlg() {};

	static std::shared_ptr<CPaletteSelectionDlg> CreatePaletteSelectionDlg(CWnd* pParent = nullptr);

protected:
	CPaletteSelectionDlg(UINT nIDTemplate, CWnd* pParent)
		: CDialogEx(nIDTemplate, pParent)
	{}

};