#pragma once

namespace DXF
{
	struct CartesianConverterGroup;
}

class CCartesianConversionDlg : public CDialogEx
{
public:
	virtual ~CCartesianConversionDlg() {};

	virtual std::shared_ptr<DXF::CartesianConverterGroup> GetConversionGroup() = 0;

	static std::shared_ptr<CCartesianConversionDlg> CreateCartesianConversionDlg(const DXF::CartesianConverterGroup& ccg, CWnd* pParent = nullptr);

protected:
	CCartesianConversionDlg(UINT nIDTemplate, CWnd* pParent)
		: CDialogEx(nIDTemplate, pParent)
	{}

};