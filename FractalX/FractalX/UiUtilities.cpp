#include "stdafx.h"
#include "UiUtilities.h"

namespace UiUtilities
{
	void SafeEnable(CWnd* pWnd, UINT ctrl, bool enable)
	{
		if (!pWnd)
		{
			ASSERT(false);
			return;
		}
		CWnd* pCtrl = pWnd->GetDlgItem(ctrl);
		if (pCtrl)
			pCtrl->EnableWindow(enable);
	}

	void SafeCheck(CWnd* pWnd, UINT ctrl, bool check)
	{
		if (!pWnd)
		{
			ASSERT(false);
			return;
		}

		CButton* pBut = (CButton*)pWnd->GetDlgItem(ctrl);
		if (pBut)
			pBut->SetCheck(check);
	}

	bool FileExists(CString fileName)
	{
		auto attr = GetFileAttributes(fileName); 
		if (INVALID_FILE_ATTRIBUTES == attr && GetLastError() == ERROR_FILE_NOT_FOUND)
			return false;

		return true;
	}

	void SafeShow(CWnd* pWnd, UINT ctrl, int show)
	{
		{
			if (!pWnd)
			{
				ASSERT(false);
				return;
			}
			CWnd* pCtrl = pWnd->GetDlgItem(ctrl);
			if (pCtrl)
				pCtrl->ShowWindow(show);
		}
	}
}
