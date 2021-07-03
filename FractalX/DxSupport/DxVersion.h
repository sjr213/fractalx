#pragma once


namespace DXF
{
	enum class DxVersion : std::uint16_t
	{
		Dx11 = 1,
		Dx12
	};

	CString GetDxVersionString(DxVersion dxVersion);
	int GetIndexForDxVersion(DxVersion dxVer);
	DxVersion GetDxVersionFromIndex(int index);

	void Serialize(CArchive& ar, DxVersion& dx);
}