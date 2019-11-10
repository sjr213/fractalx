#pragma once

#include "TraceParams.h"
#include <Vertex.h>

class CFractalParamsPage : public CMFCPropertyPage
{
	DECLARE_DYNAMIC(CFractalParamsPage)

public:
	CFractalParamsPage();
	virtual ~CFractalParamsPage();

	enum { IDD = IDD_FRACTAL_PARAMS_PAGE };

	void SetBailOut(double bailout);

	double GetBailOut() const;

	void SetDerivative(double derivative);

	double GetDerivative() const;

	void SetPower(double power);

	double GetPower() const;

	void SetModelType(DXF::FractalType fractalType);

	DXF::FractalType GetModelType() const;

	void SetCartesianType(DXF::CartesianConversionType cartesianType);

	DXF::CartesianConversionType GetCartesianType() const;

	void SetNormalizationType(DXF::BulbNormalizeType normalizationType);

	DXF::BulbNormalizeType GetNormalizationType() const;

	void SetNormalizationRoot(double root);

	double GetNormalizationRoot() const;

	void SetCartesianConversionGroup(DXF::CartesianConverterGroup& group);

	std::shared_ptr<DXF::CartesianConverterGroup> GetCartesianConversionGroup();

	void SetConstantC(const DXF::Vertex<double>& constantC);

	DXF::Vertex<double> GetConstantC() const;

protected:
	DECLARE_MESSAGE_MAP()

	void DoDataExchange(CDataExchange* pDX) override;

	void InitializeFractalTypeCombo();

	void InitializeCartesianTypeCombo();

	void InitializeNormalizationTypeCombo();

	void EnableCtrls();

	void OnModelComboChanged();

	void OnComboChanged();

	BOOL OnInitDialog() override;

	BOOL OnSetActive() override;

	void OnOK() override;

	afx_msg void OnKillfocusEdit();

	void OnCustomBut();

private:

	double m_bailout;
	double m_derivative;
	double m_power;
	int m_modelType;
	int m_cartesianType;
	int m_normalizationType;
	double m_normalizationRoot;
	std::shared_ptr<DXF::CartesianConverterGroup> m_cartesianGroup;
	DXF::Vertex<double> m_constantC;

	CComboBox m_modelCombo;
	CComboBox m_cartesianCombo;
	CComboBox m_normalizationCombo;
};