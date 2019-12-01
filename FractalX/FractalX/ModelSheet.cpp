
#include "stdafx.h"
#include "FractalX.h"
#include "ModelSheet.h"

using namespace DXF;

// CStrangeDesignSheet

IMPLEMENT_DYNAMIC(CModelSheet, CMFCPropertySheet)

CModelSheet::CModelSheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CMFCPropertySheet(nIDCaption, pParentWnd, iSelectPage)
	, m_traceParams(std::make_shared<TraceParams>())
{
	AddPages();
}

CModelSheet::CModelSheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	: CMFCPropertySheet(pszCaption, pParentWnd, iSelectPage)
	, m_traceParams(std::make_shared<TraceParams>())
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
	m_vertexPage.SetVertexSource(m_data.SourceVertices);

	VertexRect vRect{ m_data.VertexTL, m_data.VertexTR, m_data.VertexBL, m_data.VertexBR };
	m_vertexPage.SetVertices(vRect);
}

ModelData CModelSheet::GetModelData() const
{
	return m_data;
}

void CModelSheet::SetTraceParams(std::shared_ptr<const DXF::TraceParams>& traceParams)
{
	*m_traceParams = *traceParams;
	m_traceParamsPage.SetTraceParams(*m_traceParams);
	m_fractalParamsPage.SetBailOut(m_traceParams->Fractal.Bailout);
	m_fractalParamsPage.SetDerivative(m_traceParams->Fractal.Derivative);
	m_fractalParamsPage.SetPower(m_traceParams->Fractal.Power);
	m_fractalParamsPage.SetModelType(m_traceParams->Fractal.FractalModelType);
	m_fractalParamsPage.SetCartesianType(m_traceParams->Fractal.CartesianType);
	m_fractalParamsPage.SetNormalizationType(m_traceParams->Fractal.NormalizationType);
	m_fractalParamsPage.SetNormalizationRoot(m_traceParams->Fractal.NormalizationRoot);
	m_fractalParamsPage.SetCartesianConversionGroup(m_traceParams->Fractal.ConversionGroup);
	m_fractalParamsPage.SetConstantC(m_traceParams->Fractal.ConstantC);
	m_positionRangePage.SetOrigin(m_traceParams->Bulb.Origin);
	m_positionRangePage.SetStretchParams(m_traceParams->Stretch);
}

std::shared_ptr<DXF::TraceParams> CModelSheet::GetTraceParams() const
{
	return m_traceParams;
}

void CModelSheet::SetEnableAutoZoom(bool bEnable)
{
	m_vertexPage.SetEnableAutoZoom(bEnable);
}

bool CModelSheet::GetAutoZoom() const
{
	return m_autoZoom;
}

void CModelSheet::OnOk()
{
	m_data.VertexIterations = m_vertexPage.GetVertexIterations();
	m_data.TriangleSeeds = m_vertexPage.GetSeedTriangles();
	m_data.SourceVertices = m_vertexPage.GetVertexSource();
	auto vRect = m_vertexPage.GetVertices();
	m_data.VertexTL = vRect.TL;
	m_data.VertexTR = vRect.TR;
	m_data.VertexBL = vRect.BL;
	m_data.VertexBR = vRect.BR;

	m_autoZoom = m_vertexPage.GetAutoZoom();

	*m_traceParams = m_traceParamsPage.GetTraceParams();
	m_traceParams->Fractal.Bailout = m_fractalParamsPage.GetBailOut();
	m_traceParams->Fractal.Derivative = m_fractalParamsPage.GetDerivative();
	m_traceParams->Fractal.Power = m_fractalParamsPage.GetPower();
	m_traceParams->Fractal.FractalModelType = m_fractalParamsPage.GetModelType();
	m_traceParams->Fractal.CartesianType = m_fractalParamsPage.GetCartesianType();
	m_traceParams->Fractal.NormalizationType = m_fractalParamsPage.GetNormalizationType();
	m_traceParams->Fractal.NormalizationRoot = m_fractalParamsPage.GetNormalizationRoot();
	m_traceParams->Fractal.ConversionGroup = *m_fractalParamsPage.GetCartesianConversionGroup();
	m_traceParams->Fractal.ConstantC = m_fractalParamsPage.GetConstantC();
	m_traceParams->Bulb.Origin = m_positionRangePage.GetOrigin();
	m_traceParams->Stretch = m_positionRangePage.GetStretchParams();
}
