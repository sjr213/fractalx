#pragma once

#include "stdafx.h"
#include "ColorArgb.h"

namespace Gdiplus
{
	class Color;
}

namespace DirectX::SimpleMath
{
	struct Color;
}

namespace ColorUtils
{
	COLORREF ToColorRef(const DxColor::ColorArgb& argb);

	DxColor::ColorArgb FromColorRef(COLORREF cr);

	DxColor::ColorArgb ToArgb(COLORREF cr, bite alpha);

	Gdiplus::Color ConvertToGdiColor(const DxColor::ColorArgb& color);

	Gdiplus::Color ConvertToGdiColor(uint32_t color);

	DirectX::SimpleMath::Color ConvertToDxColor(const DxColor::ColorArgb& color);
}
