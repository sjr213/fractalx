#pragma once

#include <atomic>

// CProgressDlg dialog

struct CancelEvent;

class CProgressDlg : public CDialogEx
{
public:
	CProgressDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CProgressDlg();

	void SetProgress(double progress);

	void SetCancelEvent(std::shared_ptr<CancelEvent> cancelEvent);

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PROGRESS_DLG };
#endif

protected:
	BOOL OnInitDialog() override;

	void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	CProgressCtrl m_ProgressCtrl;
	std::shared_ptr<CancelEvent> m_CancelEvent;

public:
	afx_msg void OnBnClickedCancel();
};
