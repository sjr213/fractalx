#pragma once

#include "stdafx.h"
#include "ColorArgb.h"

namespace ColorUtils
{
	COLORREF ToColorRef(const DxColor::ColorArgb& argb);

	DxColor::ColorArgb FromColorRef(COLORREF cr);

	DxColor::ColorArgb ToArgb(COLORREF cr, bite alpha);
}
