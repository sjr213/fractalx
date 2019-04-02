#pragma once

#include "Light.h"

class CLightsDlg : public CDialogEx
{
public:
	virtual ~CLightsDlg() {};

	virtual DxColor::Lights GetLights() = 0;

	static std::shared_ptr<CLightsDlg> CreateLightsDlg(const DxColor::Lights& lights, CWnd* pParent = nullptr);

protected:
	CLightsDlg(UINT nIDTemplate, CWnd* pParent)
		: CDialogEx(nIDTemplate, pParent)
	{}

};