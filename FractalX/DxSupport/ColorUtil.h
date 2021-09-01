#pragma once
#include <vector>

namespace DXF::ColorUtil
{
	uint32_t MakeColor(byte b1, byte b2, byte b3, byte b4);

	byte GetComponent(uint32_t color, int index);

	uint32_t ArgbToRgba(uint32_t oldColor);

	std::vector<uint32_t> ConvertFromARGBtoRGBA(const std::vector<uint32_t>& originalColors);
}