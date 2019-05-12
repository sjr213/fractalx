
#include "stdafx.h"
#include "DxColorUtilities.h"
#include <assert.h>
#include <algorithm>

namespace DxColor::Utilities
{
	static bite WeightedComponent(bite lf, bite rt, double ratio)
	{
		double lfPart = (1.0 - ratio)* lf;
		double rtPart = ratio * rt;
		double combination = lfPart + rtPart + 0.49999;

		combination = std::min<double>(combination, 255.0);
		combination = std::max<double>(combination, 0.0);

		return static_cast<bite>(combination);
	}

	ColorArgb AverageColors(const ColorArgb& lf, const ColorArgb& rt, double ratio)
	{
		assert(ratio >= 0.0);
		assert(ratio <= 1.0);

		ColorArgb newColor;
		newColor.A = WeightedComponent(lf.A, rt.A, ratio);
		newColor.R = WeightedComponent(lf.R, rt.R, ratio);
		newColor.G = WeightedComponent(lf.G, rt.G, ratio);
		newColor.B = WeightedComponent(lf.B, rt.B, ratio);

		return newColor;
	}
}