#pragma once

#include "ColorArgb.h"
#include <vector>
#include <atlstr.h>

class CArchive;

namespace DxColor
{
	enum class ColorCurveType
	{
		Normal = 1, Curve = 2, DoubleBand = 3
	};

	struct ColorPin
	{
		ColorArgb Color1;
		ColorArgb Color2;	// used for DoubleBand

		double Index = 0.0;

		double Curve = 0.0;

		ColorCurveType CurveType = ColorCurveType::Normal;

		// When DoubleBand these describe the width of the stripes
		// Colors spread between 0.0 and 1.0, if DoubleBand, each band will be 0.001 colors wide by default
		// So if there are 30,000 colors, a band will be 30 colors wide
		double IndexWidth1 = 0.001;
		double IndexWidth2 = 0.001;
	};

	struct PinPalette
	{
		CString Name;
		std::vector<ColorPin> Pins;
	};

	// Make sure indices are between 0 and 1.0 and in ascending order
	void ValidatePalette(PinPalette& palette);

	// Make sure the pins don't overlap
	void SpacePins(PinPalette& palette);

	void SerializePalette(CArchive& ar, PinPalette& palette);
}