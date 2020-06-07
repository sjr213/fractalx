#pragma once
#include "Resource.h"
#include "Vertex.h"

class CBackgroundPage : public CMFCPropertyPage
{
	DECLARE_DYNAMIC(CBackgroundPage)

public:
	CBackgroundPage();
	virtual ~CBackgroundPage() = default;

	enum { IDD = IDD_BACKGROUND_PAGE };

	void SetFilename(const std::wstring& filename);

	std::wstring GetFilename() const;

	void SetShowBackground(bool show);

	bool GetShowBackground() const;

	void SetModelVertices(const std::vector<DXF::Vertex<float>>& model);

	std::vector<DXF::Vertex<float>> GetModelVertices() const;

protected:
	DECLARE_MESSAGE_MAP()

	void DoDataExchange(CDataExchange* pDX) override;

	BOOL OnInitDialog() override;

	BOOL OnSetActive() override;

	BOOL OnKillActive() override;

	void OnOK() override;

	void OnBrowseBut();

	void EnableCtrls();

	afx_msg void OnShowClicked();

	afx_msg void DefaultModelClicked();

private:
	CString m_filename;
	bool m_showBackground = false;
	std::vector<DXF::Vertex<float>> m_backgroundModel;
};