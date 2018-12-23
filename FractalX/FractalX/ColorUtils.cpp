#include "stdafx.h"
#include "ColorUtils.h"
#include <gdiplus.h>

namespace ColorUtils
{
	COLORREF ToColorRef(const DxColor::ColorArgb& argb)
	{
		return RGB(argb.R, argb.G, argb.B);
	}

	DxColor::ColorArgb FromColorRef(COLORREF cr)
	{
		return DxColor::ColorArgb{ 255, static_cast<bite>(GetRValue(cr)), static_cast<bite>(GetGValue(cr)), static_cast<bite>(GetBValue(cr)) };
	}

	DxColor::ColorArgb ToArgb(COLORREF cr, bite alpha)
	{
		return DxColor::ColorArgb(alpha, GetRValue(cr), GetGValue(cr), GetBValue(cr));
	}

	Gdiplus::Color ConvertToGdiColor(const DxColor::ColorArgb& color)
	{
		Gdiplus::Color gColor(color.A, color.R, color.G, color.B);
		return gColor;
	}

	Gdiplus::Color ConvertToGdiColor(uint32_t color)
	{
		// opposite (a << 24) | (r << 16) | (g << 8) | b;

		int b = color & 0xff;
		int g = (color >> 8) & 0xff;
		int r = (color >> 16) & 0xff;
		int a = (color >> 24) & 0xff;

		Gdiplus::Color gColor(a, r, g, b);
		return gColor;
	}
}