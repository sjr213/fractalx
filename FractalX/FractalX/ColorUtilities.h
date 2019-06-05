#pragma once

namespace DxColor
{
	struct PinPalette;
	struct ColorContrast;
}

namespace fx::ColorUtilities
{
	std::vector<uint32_t> CalculatePaletteColors(const DxColor::PinPalette& palette, int nColors, const DxColor::ColorContrast& contrast);

	COLORREF ToColorRef(uint32_t color);

	CString GetPaletteFileExtension();

	std::shared_ptr<DxColor::PinPalette> LoadPalette(const CString& palettePath);
}
