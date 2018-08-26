#include "stdafx.h"

#define NOMINMAX

#include <algorithm>
#include "ColorUtilities.h"
#include "..\DxColors\ColorPin.h"

using namespace DxColor;

namespace fx
{
	namespace ColorUtilities
	{
		static uint32_t BitesToXmcolor(bite a, bite r, bite g, bite b)
		{
			// XMCOLOR is structured 0xaarrggbb
			// COLORREF is structured 0x00bbggrr
			return (a << 24) | (r << 16) | (g<< 8) | b;
		}

		static void AddColor(std::vector<uint32_t>& colors, bite a, bite r, bite g, bite b)
		{
			colors.push_back(BitesToXmcolor(a, r, g, b));
		}

		static bite StretchBite(bite start, bite end, int nStep, int nSteps)
		{
			double eStart = start;
			double eRange = static_cast<double>(end) - static_cast<double>(start);
			double eStretch = static_cast<double>(nStep) / nSteps;
			double result = eStart + eStretch * eRange;
			return static_cast<bite>(result + 0.5);
		}

		static int ColorsRemaining(std::vector<uint32_t>& colors, int nColors)
		{
			return nColors - static_cast<int>(colors.size());
		}

		void FillSpaceBeforeFirstPin(const DxColor::ColorPin& pin1, std::vector<uint32_t>& colors, int nColors)
		{
			if (pin1.Index <= 0.0)
				return;

			int nSteps = static_cast<int>(pin1.Index * nColors);
			nSteps = std::min<int>(nSteps, nColors);

			for(int i = 0; i < nSteps; ++i)
				AddColor(colors, pin1.Color1.A, pin1.Color1.R, pin1.Color1.G, pin1.Color1.B);
		}

		static void StretchPins(const DxColor::ColorPin& pin1, const DxColor::ColorPin& pin2, std::vector<uint32_t>& colors, int nColors, bool lastPins)
		{
			if (pin1.CurveType != DxColor::ColorCurveType::Normal)
				throw std::exception("Only Normal curve type is implemented");

			int nSteps = static_cast<int>((pin2.Index - pin1.Index) * nColors + 0.5);

			if (lastPins)
			{
				int colorsRemaining = ColorsRemaining(colors, nColors);
				if(colorsRemaining <= 1)
					throw std::exception("stretch failed, too many colors!");

				if (pin2.Index >= 1.0)
					nSteps = colorsRemaining;
			}

			// always add one color for each pin
			AddColor(colors, pin1.Color1.A, pin1.Color1.R, pin1.Color1.G, pin1.Color1.B);

			// add intermediate colors
			if (nSteps > 2)
			{
				for (int i = 1; i < nSteps - 1; ++i)
				{
					bite a = StretchBite(pin1.Color1.A, pin2.Color1.A, i, nSteps);
					bite r = StretchBite(pin1.Color1.R, pin2.Color1.R, i, nSteps);
					bite g = StretchBite(pin1.Color1.G, pin2.Color1.G, i, nSteps);
					bite b = StretchBite(pin1.Color1.B, pin2.Color1.B, i, nSteps);
					AddColor(colors, a, r, g, b);
				}
			}

			// add last color
			AddColor(colors, pin2.Color1.A, pin2.Color1.R, pin2.Color1.G, pin2.Color1.B);

			if (!lastPins)
				return;

			while(static_cast<int>(colors.size()) < nColors)
				AddColor(colors, pin2.Color1.A, pin2.Color1.R, pin2.Color1.G, pin2.Color1.B);
		}

		std::vector<uint32_t> ConvertPalette(DxColor::PinPalette& palette, int nColors)
		{
			// this can throw std::exception if there are less than 2 colors
			DxColor::ValidatePalette(palette);

			int nPins = static_cast<int>(palette.Pins.size());
			DxColor::ColorPin pin1 = palette.Pins.at(0);

			std::vector<uint32_t> colors;
			colors.reserve(nColors);

			FillSpaceBeforeFirstPin(pin1, colors, nColors);

			for (int i = 1; i < nPins; ++i)
			{
				const DxColor::ColorPin& pin2 = palette.Pins.at(i);
				StretchPins(pin1, pin2, colors, nColors, i == nPins - 1);
				pin1 = pin2;
			}

			ASSERT(static_cast<int>(colors.size() == nColors));

			return colors;
		}

		COLORREF ToColorRef(uint32_t color)
		{
			// opposite (a << 24) | (r << 16) | (g << 8) | b;

			int b = color & 0xff;
			int g = (color >> 8) & 0xff;
			int r = (color >> 16) & 0xff;

			return RGB(r, g, b);
		}
	}
}