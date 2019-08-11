#pragma once

#include "TraceParams.h"

class CFractalParamsPage : public CMFCPropertyPage
{
	DECLARE_DYNAMIC(CFractalParamsPage)

public:
	CFractalParamsPage();
	virtual ~CFractalParamsPage();

	enum { IDD = IDD_FRACTAL_PARAMS_PAGE };

	void SetBailOut(double bailout);

	double GetBailOut() const;

	void SetConstantC(double constantC);

	double GetConstantC() const;

	void SetPower(double power);

	double GetPower() const;

	void SetModelType(DXF::FractalType fractalType);

	DXF::FractalType GetModelType() const;

protected:
	DECLARE_MESSAGE_MAP()

	virtual void DoDataExchange(CDataExchange* pDX);

	void InitializeFractalTypeCombo();

	void OnFractalTypeChanged();

	virtual BOOL OnSetActive();

	void OnOK() override;

	afx_msg void OnKillfocusEdit();

private:

	double m_bailout;
	double m_constantC;
	double m_power;
	int m_modelType;

	CComboBox m_modelCombo;
};