#include "pch.h"
#include "Dx12TextureMapper.h"

#include "NarrowCast.h"

using namespace DxCore;

namespace DxSupport::Dx12TextureMapper
{
	// The actual max D3D11_REQ_TEXTURE1D_U_DIMENSION: 16384
	

	UINT GetTextureWidth(size_t size)
	{
		return narrow<UINT>(size > MaxTextureWidth ? MaxTextureWidth : size);
	}

	UINT GetTextureHeight(size_t size)
	{
		auto ht = ceil(static_cast<double>(size) / MaxTextureWidth);
		return narrow<UINT>(ht);
	}

	void ExpandColorsIfNecessary(std::vector<uint32_t>& colors)
	{
		auto w = GetTextureWidth(colors.size());
		auto h = GetTextureHeight(colors.size());

		size_t expectedSize = w * h;
		if (expectedSize > colors.size())
			colors.resize(expectedSize);
	}
}