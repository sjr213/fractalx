#include "stdafx.h"
#include <afx.h>
#include "ColorPin.h"
#include <assert.h>
#include <algorithm>

namespace DxColor
{
	const int CurrentPinVersion = 1;

	static void SerializeColorArgb(CArchive& ar, ColorArgb& color)
	{
		if (ar.IsStoring())
			ar << color.A << color.R << color.G << color.B;
		else
			ar >> color.A >> color.R << color.G << color.B;
	}

	static void SerializePin(CArchive& ar, ColorPin& pin, int version)
	{
		assert(version == CurrentPinVersion);
		if (version != CurrentPinVersion)
			return;

		SerializeColorArgb(ar, pin.Color1);
		SerializeColorArgb(ar, pin.Color2);
		SerializeColorArgb(ar, pin.Color3);

		if (ar.IsStoring())
		{
			ar << pin.Index;
			ar << static_cast<int>(pin.CurveType);
			ar << pin.IndexWidth1 << pin.IndexWidth2 << pin.IndexWidth3;
		}
		else
		{
			int curveType = 1;
			ar >> pin.Index;
			ar >> curveType;
			ar >> pin.IndexWidth1 >> pin.IndexWidth2 >> pin.IndexWidth3;

			pin.CurveType = static_cast<ColorCurveType>(curveType);
		}
	}

	void ExportPins(CArchive& ar, PinPalette& palette)
	{
		if (ar.IsStoring())
		{
			ar << CurrentPinVersion;
			ar << palette.Name;
			ar << static_cast<int>(palette.Pins.size());

			for (size_t i = 0; i < palette.Pins.size(); ++i)
				SerializePin(ar, palette.Pins.at(i), CurrentPinVersion);
		}
		else
		{
			int version = 0;
			ar >> version;

			assert(version == CurrentPinVersion);
			if (version != CurrentPinVersion)
				return;

			ar >> palette.Name;

			int nPins = 0;
			ar >> nPins;

			if (nPins <= 0)
				return;

			palette.Pins.resize(nPins);

			for (int i = 0; i < nPins; ++i)
				SerializePin(ar, palette.Pins.at(i), version);
		}
	}

	static void StretchPaletteIndices(PinPalette& palette, double oldMin, double oldMax)
	{
		if (oldMin == oldMax) // stretch evenly since the pin indices are the same
		{
			int nPins = static_cast<int>(palette.Pins.size());
			double interval = 1.0 /(nPins - 1);

			for (int i = 0; i < nPins; ++i)
			{
				palette.Pins.at(i).Index = i * interval;
			}
		}
		else // stretch between 0 and 1
		{
			double range = oldMax - oldMin;
			int nPins = static_cast<int>(palette.Pins.size());

			for (int i = 0; i < nPins; ++i)
			{
				palette.Pins.at(i).Index = (palette.Pins.at(i).Index - oldMin) / range;
			}
		}
	}

	// Make sure indices are between 0 and 1.0 and in ascending order
	void ValidatePalette(PinPalette& palette)
	{
		if (palette.Pins.size() < 2)
			throw std::exception("less than 2 pins");

		std::sort(begin(palette.Pins), end(palette.Pins), [&](const ColorPin& lf, const ColorPin& rt)
		{
			return lf.Index < rt.Index;
		});

		auto pr = std::minmax_element(begin(palette.Pins), end(palette.Pins), [&](const ColorPin& lf, const ColorPin& rt)
		{
			return lf.Index < rt.Index;
		});

		if (pr.first->Index != 0.0 || pr.second->Index != 1.0)
			StretchPaletteIndices(palette, pr.first->Index, pr.second->Index);
	}
}