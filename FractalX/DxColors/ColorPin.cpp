
#include "stdafx.h"
#include <afx.h>

#include <algorithm>
#include <assert.h>
#include "ColorPin.h"
#include "DxColorsSerialization.h"

namespace DxColor
{
	const int CurrentPinVersion = 1;

	static void SerializePin(CArchive& ar, ColorPin& pin, int version)
	{
		assert(version == CurrentPinVersion);
		if (version != CurrentPinVersion)
			return;

		SerializeColorArgb(ar, pin.Color1);
		SerializeColorArgb(ar, pin.Color2);

		if (ar.IsStoring())
		{
			ar << pin.Index;
			ar << static_cast<int>(pin.CurveType);
			ar << pin.Curve;
			ar << pin.IndexWidth1 << pin.IndexWidth2;
		}
		else
		{
			int curveType = 1;
			ar >> pin.Index;
			ar >> curveType;
			ar >> pin.Curve;
			ar >> pin.IndexWidth1 >> pin.IndexWidth2;

			pin.CurveType = static_cast<ColorCurveType>(curveType);
		}
	}

	void SerializePalette(CArchive& ar, PinPalette& palette)
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

	// no longer used
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
			return;

		std::sort(begin(palette.Pins), end(palette.Pins), [&](const ColorPin& lf, const ColorPin& rt)
		{
			return lf.Index < rt.Index;
		});

		auto pr = std::minmax_element(begin(palette.Pins), end(palette.Pins), [&](const ColorPin& lf, const ColorPin& rt)
		{
			return lf.Index < rt.Index;
		});

		if (pr.first->Index < 0.0)
			throw std::exception("lowest index less than 0.0");

		if(pr.second->Index > 1.0)
			throw std::exception("highest index greater than 1.0");
	}

	void SpacePins(PinPalette& palette)
	{
		if (palette.Pins.size() < 2)
			throw std::exception("less than 2 pins");

		std::sort(begin(palette.Pins), end(palette.Pins), [&](const ColorPin& lf, const ColorPin& rt)
		{
			return lf.Index < rt.Index;
		});

		DxColor::ColorPin* pin1 = &palette.Pins.at(0);

		int nPins = static_cast<int>(palette.Pins.size());

		const double space = 0.0001;

		for (int i = 1; i < nPins; ++i)
		{
			DxColor::ColorPin* pin2 = &palette.Pins.at(i);
			if (pin2->Index <= pin1->Index)
			{
				if (pin2->Index >= 1.0 - space)
					pin1->Index -= space;
				else
					pin2->Index += space;
			}

			pin1 = &palette.Pins.at(i);
		}
	}
}