#include "stdafx.h"
#include "DxVersion.h"


// Test commit again
namespace DXF
{
	CString GetDxVersionString(DxVersion dxVersion)
	{
		switch (dxVersion)
		{
		case DXF::DxVersion::Dx11:
			return _T("DirectX 11");
		case DXF::DxVersion::Dx12:
			return _T("DirectX 12");
		default:
			assert(false);
			return _T("DirectX 11");
		}

	}
}