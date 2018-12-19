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
}