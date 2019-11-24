#pragma once

#include <Vertex.h>

class CWorldScaleDlg : public CDialogEx
{
public:
	virtual ~CWorldScaleDlg() {};

	virtual DXF::Vertex<float> GetWorldScale() const = 0;

	static std::shared_ptr<CWorldScaleDlg> CreateWorldScaleDlg(const DXF::Vertex<float>& worldScale, CWnd* pParent = nullptr);

protected:
	CWorldScaleDlg(UINT nIDTemplate, CWnd* pParent)
		: CDialogEx(nIDTemplate, pParent)
	{}

};