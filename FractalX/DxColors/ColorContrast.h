#pragma once
#include "ColorArgb.h"

namespace DxColor
{
	const double MAX_HUE = 359.9;
	const double MAX_SATURATION = 1.0;
	const double MAX_LIGHTNESS = 1.0;

	enum class ContrastType
	{
		None = 1, Contrast = 2, HSL = 3
	};

	struct ColorContrast
	{
		ContrastType Mode = ContrastType::None;

		bite MinContrast[3];
		bite MaxContrast[3];

		double MinHue = 0;
		double MaxHue = MAX_HUE;

		double MinSaturation = 0;
		double MaxSaturation = MAX_SATURATION;

		double MinLightness = 0;
		double MaxLightness = MAX_LIGHTNESS;
	};
}
