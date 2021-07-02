#include "stdafx.h"
#include "DxVersion.h"


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

	void Serialize(CArchive& ar, DxVersion& dx)
	{
		if (ar.IsStoring())
		{
			ar << static_cast<std::uint16_t>(dx);
		}
		else
		{
			std::uint16_t dxVer = 1;
			ar >> dxVer;
			dx = static_cast<DxVersion>(dxVer);
		}
	}
}