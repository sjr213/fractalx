#pragma once

namespace UiUtilities
{
	void SafeEnable(CWnd* pWnd, UINT ctrl, bool enable);

	void SafeCheck(CWnd* pWnd, UINT ctrl, bool check);

	bool FileExists(CString fileName);
}