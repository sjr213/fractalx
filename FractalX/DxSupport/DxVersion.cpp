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

	int GetIndexForDxVersion(DxVersion dxVer)
	{
		switch (dxVer)
		{
		case DxVersion::Dx11:
			return 0;
		case DxVersion::Dx12:
			return 1;
		default:
			assert(false);
			return 0;
		}
	}

	DxVersion GetDxVersionFromIndex(int index)
	{
		switch (index)
		{
		case 0:
			return DxVersion::Dx11;
		case 1:
			return DxVersion::Dx12;
		default:
			assert(false);
			return DxVersion::Dx11;
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