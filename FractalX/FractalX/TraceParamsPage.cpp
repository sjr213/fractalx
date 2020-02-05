#include "stdafx.h"
#include "FractalX.h"
#include "TraceParamsPage.h"

#include "ModelSheet.h"

using namespace DXF;

IMPLEMENT_DYNAMIC(CTraceParamsPage, CMFCPropertyPage)

CTraceParamsPage::CTraceParamsPage()
	: CMFCPropertyPage(CTraceParamsPage::IDD)
{}

CTraceParamsPage::~CTraceParamsPage()
{}

void CTraceParamsPage::SetTraceParams(const TraceParams& traceParams)
{
	m_traceParams = traceParams;
}

TraceParams CTraceParamsPage::GetTraceParams() const
{
	return m_traceParams;
}

BEGIN_MESSAGE_MAP(CTraceParamsPage, CMFCPropertyPage)
	ON_EN_KILLFOCUS(IDC_MAX_RAY_STEPS_EDIT, &CTraceParamsPage::OnKillfocusEdit)
	ON_EN_KILLFOCUS(IDC_MIN_RAY_DISTANCE_EDIT, &CTraceParamsPage::OnKillfocusEdit)
	ON_EN_KILLFOCUS(IDC_STEP_DIVISOR_EDIT, &CTraceParamsPage::OnKillfocusEdit)
	ON_EN_KILLFOCUS(IDC_ITERATIONS_EDIT, &CTraceParamsPage::OnKillfocusEdit)
END_MESSAGE_MAP()

void CTraceParamsPage::DoDataExchange(CDataExchange* pDX)
{
	CMFCPropertyPage::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_MAX_RAY_STEPS_EDIT, m_traceParams.Bulb.MaxRaySteps);
	DDV_MinMaxInt(pDX, m_traceParams.Bulb.MaxRaySteps, 1, 1000000);

	DDX_Text(pDX, IDC_MIN_RAY_DISTANCE_EDIT, m_traceParams.Bulb.MinRayDistance);
	DDV_MinMaxDouble(pDX, m_traceParams.Bulb.MinRayDistance, 1e-8, 1);

	DDX_Text(pDX, IDC_STEP_DIVISOR_EDIT, m_traceParams.Bulb.StepDivisor);
	DDV_MinMaxDouble(pDX, m_traceParams.Bulb.StepDivisor, 1, 1000);

	DDX_Text(pDX, IDC_ITERATIONS_EDIT, m_traceParams.Bulb.Iterations);
	DDV_MinMaxInt(pDX, m_traceParams.Bulb.Iterations, 2, 1000000);

	BOOL fractional = m_traceParams.Bulb.Fractional;
	DDX_Check(pDX, IDC_FRACTIONAL_CHECK, fractional);
	m_traceParams.Bulb.Fractional = fractional ? true : false;
}

BOOL CTraceParamsPage::OnSetActive()
{
	return CPropertyPage::OnSetActive();
}

void CTraceParamsPage::OnOK()
{
	CModelSheet* pModelSheet = dynamic_cast<CModelSheet*>(GetParent());
	if (pModelSheet)
		pModelSheet->OnOk();

	CMFCPropertyPage::OnOK();
}

void CTraceParamsPage::OnKillfocusEdit()
{
	UpdateData(TRUE);
}
