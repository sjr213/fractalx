#pragma once

#include "ModelData.h"
#include "TraceParams.h"
#include "FractalParamsPage.h"
#include "TraceParamsPage.h"
#include "VertexPage.h"
#include "PositionRangePage.h"

class CModelSheet : public CMFCPropertySheet
{
	DECLARE_DYNAMIC(CModelSheet)

public:
	CModelSheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CModelSheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	virtual ~CModelSheet();

	void SetModelData(const DXF::ModelData& data);

	void SetTraceParams(const DXF::TraceParams& traceParams);

	DXF::ModelData GetModelData() const;

	DXF::TraceParams GetTraceParams() const;

	void OnOk();

protected:
	void AddPages();

	DECLARE_MESSAGE_MAP()

	CVertexPage	m_vertexPage;

	CFractalParamsPage m_fractalParamsPage;

	CTraceParamsPage m_traceParamsPage;

	CPositionRangePage m_positionRangePage;

	DXF::ModelData m_data;

	DXF::TraceParams m_traceParams;
};