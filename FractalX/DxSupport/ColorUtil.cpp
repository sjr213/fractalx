#include "stdafx.h"
#include "ColorUtil.h"

namespace DXF::ColorUtil
{
	uint32_t MakeColor(byte b1, byte b2, byte b3, byte b4)
	{
		return (b1 << 24) | (b2 << 16) | (b3 << 8) | b4;
	}

	byte GetComponent(uint32_t color, int index)
	{
		assert(index >= 0 && index < 4);

		int rtShift = (3 - index) * 8;

		return static_cast<byte>(0xff & color >> rtShift);
	}

	uint32_t ArgbToRgba(uint32_t oldColor)
	{
		return (oldColor << 8) | (0xff & oldColor >> 24);
	}

	std::vector<uint32_t> ConvertFromARGBtoRGBA(const std::vector<uint32_t>& originalColors)
	{
		std::vector<uint32_t> rgba(originalColors.size());

		for (auto i = 0; i < originalColors.size(); ++i)
		{
			auto color = originalColors.at(i);
			rgba.at(i) = ArgbToRgba(color);
		}
		return rgba;
	}
}