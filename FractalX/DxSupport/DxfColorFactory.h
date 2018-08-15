#pragma once
#include <vector>

namespace DXF
{
	int GetNumberOfTextures();

	CString GetTextureName(int i);

	std::vector<uint32_t> GetTexture(int i);

	std::vector<uint32_t> CreateRainbow();

	std::vector<uint32_t> CreateStripes();
}
