#pragma once

#include "stdafx.h"
#include "ColorArgb.h"

namespace ColorUtils
{
	COLORREF ToColorRef(const DxColor::ColorArgb& argb)
	{
		return RGB(argb.R, argb.G, argb.B);
	}

	DxColor::ColorArgb FromColorRef(const COLORREF& cr)
	{
		return DxColor::ColorArgb{ 255, static_cast<bite>(GetRValue(cr)), static_cast<bite>(GetGValue(cr)), static_cast<bite>(GetBValue(cr)) };
	}
}
