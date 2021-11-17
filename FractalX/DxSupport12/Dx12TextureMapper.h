#pragma once

namespace DxSupport::Dx12TextureMapper
{
	constexpr size_t MaxTextureWidth = 10000;

	UINT GetTextureWidth(size_t size);

	UINT GetTextureHeight(size_t size);

	void ExpandColorsIfNecessary(std::vector<uint32_t>& colors);
}