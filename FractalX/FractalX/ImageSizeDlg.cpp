#include "stdafx.h"
#include "ImageSizeDlg.h"

#include "resource.h"

class CImageSizeDlgImpl : public CImageSizeDlg
{
private:
	int m_width;
	int m_height;

public:
	CImageSizeDlgImpl(int width, int height, CWnd* pParent)
		: CImageSizeDlg(IDD_IMAGE_SIZE_DLG, pParent)
		, m_width(width)
		, m_height(height)
	{}

	virtual ~CImageSizeDlgImpl() {}

	CSize GetSize() override
	{
		return CSize(m_width, m_height);
	}

protected:
	DECLARE_MESSAGE_MAP()

	BOOL OnInitDialog() override
	{
		CImageSizeDlg::OnInitDialog();


		return TRUE;
	}

	void DoDataExchange(CDataExchange* pDX) override
	{
		CImageSizeDlg::DoDataExchange(pDX);

		DDX_Text(pDX, IDC_WIDTH_EDIT, m_width);
		DDV_MinMaxInt(pDX, m_width, 0, 100000);

		DDX_Text(pDX, IDC_HEIGHT_EDIT, m_height);
		DDV_MinMaxInt(pDX, m_height, 0, 100000);
	}

	void OnKillFocus()
	{
		UpdateData(TRUE);
	}


};

BEGIN_MESSAGE_MAP(CImageSizeDlgImpl, CImageSizeDlg)
	ON_EN_KILLFOCUS(IDC_WIDTH_EDIT, &CImageSizeDlgImpl::OnKillFocus)
	ON_EN_KILLFOCUS(IDC_HEIGHT_EDIT, &CImageSizeDlgImpl::OnKillFocus)
END_MESSAGE_MAP()

std::shared_ptr<CImageSizeDlg> CImageSizeDlg::CreateImageSizeDlg(int width, int height, CWnd* pParent /* = nullptr*/)
{
	return std::make_shared<CImageSizeDlgImpl>(width, height, pParent);
}