#pragma once

#include "ColorArgb.h"

namespace DxColor
{
	struct EffectColors
	{
		EffectColors()
		{
			SetDefaultEffectColors(*this);
		}

		static void SetDefaultEffectColors(EffectColors& colors)
		{
			colors.Ambient = ColorArgb((bite)255, (bite)64, (bite)64, (bite)64);
			colors.Diffuse = ColorArgb((bite)255, (bite)192, (bite)192, (bite)192);
			colors.Specular = ColorArgb((bite)255, (bite)192, (bite)192, (bite)192);
			colors.SpecularPower = 16.0f;
		}

		ColorArgb Ambient;
		ColorArgb Diffuse;
		ColorArgb Specular;
		float SpecularPower;
	};


}