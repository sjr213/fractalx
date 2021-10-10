#pragma once

#include "DxVersion.h"
#include "EffectColors.h"

class CEffectColorDlg : public CDialogEx
{
public:
	virtual ~CEffectColorDlg() {};

	virtual DxColor::EffectColors GetColors() = 0;

	static std::shared_ptr<CEffectColorDlg> CreateEffectColorDlg(DxColor::EffectColors colors,
		DXF::DxVersion version, CWnd* pParent = nullptr);

protected:
	CEffectColorDlg(UINT nIDTemplate, CWnd* pParent)
		: CDialogEx(nIDTemplate, pParent)
	{}

};