#pragma once

#include "TraceParams.h"

class CTraceParamsPage : public CMFCPropertyPage
{
	DECLARE_DYNAMIC(CTraceParamsPage)

public:
	CTraceParamsPage();
	virtual ~CTraceParamsPage();

	enum { IDD = IDD_TRACE_PARAMS_PAGE };

	void SetTraceParams(const DXF::TraceParams& traceParams);

	DXF::TraceParams GetTraceParams() const;

protected:
	DECLARE_MESSAGE_MAP()

	virtual void DoDataExchange(CDataExchange* pDX);

	virtual BOOL OnSetActive();

	void OnOK() override;

	afx_msg void OnKillfocusEdit();

private:
	DXF::TraceParams m_traceParams;
};