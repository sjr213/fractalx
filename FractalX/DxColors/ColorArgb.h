#pragma once

typedef unsigned __int8 bite;

namespace DxColor
{
	struct ColorArgb
	{
		bite A = 255;
		bite R = 0;
		bite G = 0;
		bite B = 0;
	};

	COLORREF ToColorRef(const ColorArgb argb)
	{
		return RGB(argb.R, argb.G, argb.B);
	}

	ColorArgb FromColorRef(const COLORREF& cr)
	{
		return ColorArgb{ 255, static_cast<bite>(GetRValue(cr)), static_cast<bite>(GetGValue(cr)), static_cast<bite>(GetBValue(cr)) };
	}
}
