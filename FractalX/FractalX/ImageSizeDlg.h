#pragma once

class CImageSizeDlg : public CDialogEx
{
public:
	virtual ~CImageSizeDlg() {};

	virtual CSize GetSize() = 0;

	static std::shared_ptr<CImageSizeDlg> CreateImageSizeDlg(int width, int height, CWnd* pParent = nullptr);

protected:
	CImageSizeDlg(UINT nIDTemplate, CWnd* pParent)
		: CDialogEx(nIDTemplate, pParent)
	{}

};