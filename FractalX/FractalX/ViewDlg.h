#pragma once

#include <tuple>

class CViewDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CViewDlg)

public:
	CViewDlg(CWnd* pParent = nullptr);
	~CViewDlg();

	std::tuple<float, float, float> GetCamera() const;
	std::tuple<float, float, float> GetTarget() const;

	void SetCamera(const std::tuple<float, float, float>& camera);
	void SetTarget(const std::tuple<float, float, float>& target);

	// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_VIEW_DLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	afx_msg void OnKillfocusEdit();

	DECLARE_MESSAGE_MAP()

	std::tuple<float, float, float> m_camera = std::make_tuple(0.0f, 0.0f, 0.0f);
	std::tuple<float, float, float> m_target = std::make_tuple(0.0f, 0.0f, 0.0f);
};

