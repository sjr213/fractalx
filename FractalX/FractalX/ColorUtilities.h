#pragma once

namespace DxColor
{
	struct PinPalette;
}

namespace fx
{
	namespace ColorUtilities
	{
		std::vector<uint32_t> ConvertPalette(DxColor::PinPalette& palette, int nColors);

		COLORREF ToColorRef(uint32_t color);
	}
}
