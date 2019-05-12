#pragma once

#include "ColorArgb.h"

namespace DxColor::Utilities
{
	// ratio is the proportion of the 2nd color used relative to the first
	ColorArgb AverageColors(const ColorArgb& lf, const ColorArgb& rt, double ratio);
}