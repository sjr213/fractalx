// ProgressDlg.cpp : implementation file
//

#include "stdafx.h"
#include "FractalX.h"
#include "CancelException.h"
#include "ProgressDlg.h"
#include "ProgressCancelSignaling.h"
#include "afxdialogex.h"

const int RANGE = 1000;

// CProgressDlg dialog

CProgressDlg::CProgressDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_PROGRESS_DLG, pParent)
{
}

CProgressDlg::~CProgressDlg()
{
}

BOOL CProgressDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_ProgressCtrl.SetRange(0, RANGE);

	return TRUE;
}

void CProgressDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS1, m_ProgressCtrl);
}

void CProgressDlg::SetProgress(double progress)
{
	if (progress > 1.0)
		progress = 1.0;

	if (progress < 0.0)
		progress = 0;

	int nProgress = static_cast<int>(RANGE * progress);

	m_ProgressCtrl.SetPos(nProgress);
}

void CProgressDlg::SetCancelEvent(std::shared_ptr<CancelEvent> cancelEvent)
{
	m_CancelEvent = cancelEvent;
}

BEGIN_MESSAGE_MAP(CProgressDlg, CDialogEx)
	ON_BN_CLICKED(IDCANCEL, &CProgressDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// CProgressDlg message handlers


void CProgressDlg::OnBnClickedCancel()
{
	if (!m_CancelEvent)
		return;

	m_CancelEvent->Canceled = true;
}
