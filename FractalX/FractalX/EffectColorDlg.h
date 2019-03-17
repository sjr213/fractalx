#pragma once

#include "EffectColors.h"


class CEffectColorDlg : public CDialogEx
{
public:
	virtual ~CEffectColorDlg() {};

	virtual DxColor::EffectColors GetColors() = 0;

	static std::shared_ptr<CEffectColorDlg> CreateEffectColorDlg(DxColor::EffectColors colors, CWnd* pParent = nullptr);

protected:
	CEffectColorDlg(UINT nIDTemplate, CWnd* pParent)
		: CDialogEx(nIDTemplate, pParent)
	{}

};