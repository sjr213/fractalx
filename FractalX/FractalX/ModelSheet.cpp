
#include "stdafx.h"
#include "FractalX.h"
#include "ModelSheet.h"

using namespace DXF;

// CStrangeDesignSheet

IMPLEMENT_DYNAMIC(CModelSheet, CMFCPropertySheet)

CModelSheet::CModelSheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CMFCPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
	AddPages();
}

CModelSheet::CModelSheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	: CMFCPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
	AddPages();
}

void CModelSheet::AddPages()
{
	AddPage(&m_vertexPage);
	AddPage(&m_fractalParamsPage);
	AddPage(&m_traceParamsPage);
	AddPage(&m_positionRangePage);
}

CModelSheet::~CModelSheet()
{}

BEGIN_MESSAGE_MAP(CModelSheet, CMFCPropertySheet)
END_MESSAGE_MAP()

void CModelSheet::SetModelData(const ModelData& data)
{
	m_data = data;

	m_vertexPage.SetVertexIterations(m_data.VertexIterations);
	m_vertexPage.SetSeedTriangles(m_data.TriangleSeeds);
}

ModelData CModelSheet::GetModelData() const
{
	return m_data;
}

void CModelSheet::SetTraceParams(const DXF::TraceParams& traceParams)
{
	m_traceParams = traceParams;
	m_traceParamsPage.SetTraceParams(m_traceParams);
	m_fractalParamsPage.SetBailOut(m_traceParams.Fractal.Bailout);
	m_fractalParamsPage.SetConstantC(m_traceParams.Fractal.ConstantC);
	m_fractalParamsPage.SetPower(m_traceParams.Fractal.Power);
	m_fractalParamsPage.SetModelType(m_traceParams.Fractal.FractalModelType);
	m_positionRangePage.SetOrigin(m_traceParams.Bulb.Origin);
	m_positionRangePage.SetStretchParams(m_traceParams.Stretch);
}

DXF::TraceParams CModelSheet::GetTraceParams() const
{
	return m_traceParams;
}

void CModelSheet::OnOk()
{
	m_data.VertexIterations = m_vertexPage.GetVertexIterations();
	m_data.TriangleSeeds = m_vertexPage.GetSeedTriangles();

	m_traceParams = m_traceParamsPage.GetTraceParams();
	m_traceParams.Fractal.Bailout = m_fractalParamsPage.GetBailOut();
	m_traceParams.Fractal.ConstantC = m_fractalParamsPage.GetConstantC();
	m_traceParams.Fractal.Power = m_fractalParamsPage.GetPower();
	m_traceParams.Fractal.FractalModelType = m_fractalParamsPage.GetModelType();
	m_traceParams.Bulb.Origin = m_positionRangePage.GetOrigin();
	m_traceParams.Stretch = m_positionRangePage.GetStretchParams();
}
