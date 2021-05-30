#include "stdafx.h"
#include "ColorUtils.h"
#include <gdiplus.h>
#include "NarrowCast.h"
#include <SimpleMath.h>

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

		BYTE b = DxCore::narrow_cast<BYTE>(color & 0xff);
		BYTE g = DxCore::narrow_cast<BYTE>((color >> 8) & 0xff);
		BYTE r = DxCore::narrow_cast<BYTE>((color >> 16) & 0xff);
		BYTE a = DxCore::narrow_cast<BYTE>((color >> 24) & 0xff);

		Gdiplus::Color gColor(a, r, g, b);
		return gColor;
	}

	DirectX::SimpleMath::Color ConvertToDxColor(const DxColor::ColorArgb& color)
	{
		float r = static_cast<float>(color.R) / 255;
		float g = static_cast<float>(color.G) / 255;
		float b = static_cast<float>(color.B) / 255;
		float a = static_cast<float>(color.A) / 255;

		return DirectX::SimpleMath::Vector4(r,g,b,a);
	}
}