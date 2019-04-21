#include "stdafx.h"
#include "FileSupport.h"

namespace FileSupport
{
	CString GetExecutablePath()
	{
		CString path;
		GetModuleFileName(NULL, path.GetBuffer(MAX_PATH), MAX_PATH);
		path.ReleaseBuffer();

		wchar_t path_buffer[_MAX_PATH];
		wchar_t drive[_MAX_DRIVE];
		wchar_t dir[_MAX_DIR];
		wchar_t fname[_MAX_FNAME];
		wchar_t ext[_MAX_EXT];

		_wsplitpath_s(path, drive, _MAX_DRIVE, dir, _MAX_DIR, fname, _MAX_FNAME, ext, _MAX_EXT);

		_wmakepath_s(path_buffer, MAX_PATH, drive, dir, NULL, NULL);

		return path_buffer;
	}
}