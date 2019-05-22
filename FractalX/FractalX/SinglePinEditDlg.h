#pragma once
#include "ColorArgb.h"
#include "ColorPin.h"

class CSinglePinEditDlg : public CDialogEx
{
public:
	virtual ~CSinglePinEditDlg() {};

	virtual std::vector<DxColor::ColorPin> GetPins() = 0;

	static std::shared_ptr<CSinglePinEditDlg> CreateSinglePinEditDlg(const std::vector<DxColor::ColorPin>& pins, int pinIndex, bool enableAlpha, CWnd* pParent = nullptr);

protected:
	CSinglePinEditDlg(UINT nIDTemplate, CWnd* pParent)
		: CDialogEx(nIDTemplate, pParent)
	{}

};