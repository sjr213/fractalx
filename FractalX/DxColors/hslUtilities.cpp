#include "stdafx.h"
#include "hslUtilities.h"

#include <algorithm>
#include "ColorPin.h"
#include "ColorContrast.h"

namespace DxColor
{
	namespace 
	{
		RgbPart GetMaxComponent(ColorArgb rgb, bite minVal, bite maxVal)
		{
			if (maxVal == minVal)
				return RgbPart::total;

			if (rgb.R == maxVal)
				return RgbPart::red;

			if (rgb.G == maxVal)
				return RgbPart::green;

			if (rgb.B == maxVal)
				return RgbPart::blue;

			return RgbPart::blue;
		}

		double GetHue(ColorArgb rgb, bite minVal, bite maxVal, RgbPart maxComponent)
		{
			if (maxComponent == RgbPart::total)
			{
				if (maxVal == 255)
					return MAX_HUE;

				return 0.0;
			}
				
			int C = maxVal - minVal;
			double hue = 0.0;

			if (C == 0)
				return hue;

			int r = rgb.R;
			int g = rgb.G;
			int b = rgb.B;

			if (maxComponent == RgbPart::red)
			{
				double h = static_cast<double>(g - b) / C;
				if (h < 0)
					h += 6;

				hue = 60.0 * h;
			}

			if (maxComponent == RgbPart::green)
			{
				double h = static_cast<double>(b - r) / C + 2.0;
				hue = 60.0 * h;
			}

			if (maxComponent == RgbPart::blue)
			{
				double h = static_cast<double>(r - g) / C + 4.0;
				hue = 60.0 * h;
			}

			while (hue < 0.0)
				hue += 360.0;

			return hue;
		}

		double GetLightness(bite maxVal, bite minVal)
		{
			double dMax = maxVal / 255.0;
			double dMin = minVal / 255.0;
			return (dMin + dMax) / 2;
		}

		double GetSaturation(bite maxVal, bite minVal, double l)
		{
			if (minVal == maxVal)
				return 0.0;

			double dMax = maxVal / 255.0;
			double dMin = minVal / 255.0;

			if (l <= 0.5)
			{
				return (dMax - dMin) / (2 * l);
			}

			return (dMax - dMin) / (2.0 - 2 * l);
		}

	}

	HSL ToHsl(ColorArgb rgb)
	{
		HSL hsl; 
		bite max = std::max(std::max(rgb.R, rgb.G), rgb.B);
		bite min = std::min(std::min(rgb.R, rgb.G), rgb.B);

		RgbPart maxComponent = GetMaxComponent(rgb, min, max);

		hsl.H = GetHue(rgb, min, max, maxComponent);
		hsl.L = GetLightness(max, min);
		hsl.S = GetSaturation(max, min, hsl.L);

		return hsl;
	}

	ColorArgb Rgb::ToColorArgb(bite a)
	{
		bite r = std::max((bite)0, std::min((bite)(_r * 255 + 0.5), (bite)255));
		bite g = std::max((bite)0, std::min((bite)(_g * 255 + 0.5), (bite)255));
		bite b = std::max((bite)0, std::min((bite)(_b * 255 + 0.5), (bite)255));

		ColorArgb color;
		color.A = a;
		color.R = r;
		color.G = g;
		color.B = b;

		return color;
	}

	ColorArgb ToRgb(HSL hsl, bite a)
	{
		double c = (1 - fabs(2 * hsl.L - 1)) * hsl.S;
		double h = hsl.H / 60.0;
		double x = c * (1.0 - fabs(fmod(h, 2.0) - 1));

		double r = 0, g = 0, b = 0;
		if (h >= 0 && h < 1.0)
		{
			r = c;
			g = x;
			b = 0.0;
		}
		else if (h >= 1.0 && h < 2.0)
		{
			r = x;
			g = c;
			b = 0.0;
		}
		else if (h >= 2.0 && h < 3.0)
		{
			r = 0.0;
			g = c;
			b = x;
		}
		else if (h >= 3.0 && h < 4.0)
		{
			r = 0.0;
			g = x;
			b = c;
		}
		else if (h >= 4.0 && h < 5.0)
		{
			r = x;
			g = 0.0;
			b = c;
		}
		else if (h >= 5.0 && h < 6.0)
		{
			r = c;
			g = 0.0;
			b = x;
		}
		else
		{
			r = 0.0;
			g = 0.0;
			b = 0.0;
		}

		double m = hsl.L - 0.5 * c;

		Rgb rgb(r + m, g + m, b + m);

		return rgb.ToColorArgb(a);
	}

	HslPair GetMinMaxHue(const PinPalette& palette)
	{
		HslPair huePair;
		huePair.maxValue = 0.0;
		huePair.minValue = 360.0;

		bool bBandLast = false;

		int nPins = static_cast<int>(palette.Pins.size());

		for(int iPin = 0; iPin < nPins; ++iPin)
		{
			const ColorPin& pin = palette.Pins.at(iPin);
			auto pinColor = pin.Color1;
			HSL hsl = ToHsl(pinColor);
			huePair.minValue = std::min(huePair.minValue, hsl.H);
			huePair.maxValue = std::max(huePair.maxValue, hsl.H);

			// we don't need the last color2 if there is no next color and it's not used for the last cycle
			bool bandNow = (pin.CurveType == ColorCurveType::DoubleBand) && iPin < nPins-1;
			if (bandNow || bBandLast)
			{
				auto pinColor2 = pin.Color2;
				HSL hsl2 = ToHsl(pinColor);
				huePair.minValue = std::min(huePair.minValue, hsl2.H);
				huePair.maxValue = std::max(huePair.maxValue, hsl2.H);
			}

			bBandLast = bandNow;
		}

		return huePair;
	}

	HslPair GetMinMaxLightness(const PinPalette& palette)
	{
		HslPair lightPair;
		lightPair.maxValue = 0.0;
		lightPair.minValue = 1.0;

		bool bBandLast = false;

		int nPins = static_cast<int>(palette.Pins.size());

		for (int iPin = 0; iPin < nPins; ++iPin)
		{
			const ColorPin& pin = palette.Pins.at(iPin);
			auto pinColor = pin.Color1;
			HSL hsl = ToHsl(pinColor);
			lightPair.minValue = std::min(lightPair.minValue, hsl.L);
			lightPair.maxValue = std::max(lightPair.maxValue, hsl.L);

			// we don't need the last color2 if there is no next color and it's not used for the last cycle
			bool bandNow = (pin.CurveType == ColorCurveType::DoubleBand) && iPin < nPins - 1;
			if (bandNow || bBandLast)
			{
				auto pinColor2 = pin.Color2;
				HSL hsl2 = ToHsl(pinColor);
				lightPair.minValue = std::min(lightPair.minValue, hsl2.L);
				lightPair.maxValue = std::max(lightPair.maxValue, hsl2.L);
			}

			bBandLast = bandNow;
		}

		return lightPair;
	}

	HslPair GetMinMaxSaturation(const PinPalette& palette)
	{
		HslPair satPair;
		satPair.maxValue = 0.0;
		satPair.minValue = 1.0;

		bool bBandLast = false;

		int nPins = static_cast<int>(palette.Pins.size());

		for (int iPin = 0; iPin < nPins; ++iPin)
		{
			const ColorPin& pin = palette.Pins.at(iPin);
			auto pinColor = pin.Color1;
			HSL hsl = ToHsl(pinColor);
			satPair.minValue = std::min(satPair.minValue, hsl.S);
			satPair.maxValue = std::max(satPair.maxValue, hsl.S);

			// we don't need the last color2 if there is no next color and it's not used for the last cycle
			bool bandNow = (pin.CurveType == ColorCurveType::DoubleBand) && iPin < nPins - 1;
			if (bandNow || bBandLast)
			{
				auto pinColor2 = pin.Color2;
				HSL hsl2 = ToHsl(pinColor);
				satPair.minValue = std::min(satPair.minValue, hsl2.S);
				satPair.maxValue = std::max(satPair.maxValue, hsl2.S);
			}

			bBandLast = bandNow;
		}

		return satPair;
	}

	HslScaleParams CalculateScaleParams(const PinPalette& palette, const ColorContrast& contrast)
	{
		HslScaleParams scales;

		scales.HuePair = GetMinMaxHue(palette);
		scales.LightPair = GetMinMaxLightness(palette);
		scales.SatPair = GetMinMaxSaturation(palette);

		scales.HueScale = 1.0;
		if (scales.HuePair.maxValue > scales.HuePair.minValue)
			scales.HueScale = (contrast.MaxHue - contrast.MinHue) / (scales.HuePair.maxValue - scales.HuePair.minValue);

		scales.LightScale = 1.0;
		if (scales.LightPair.maxValue > scales.LightPair.minValue)
			scales.LightScale = (contrast.MaxLightness - contrast.MinLightness) / (scales.LightPair.maxValue - scales.LightPair.minValue);

		scales.SatScale = 1.0;
		if (scales.SatPair.maxValue > scales.SatPair.minValue)
			scales.SatScale = (contrast.MaxSaturation - contrast.MinSaturation) / (scales.SatPair.maxValue - scales.SatPair.minValue);

		return scales;
	}

	ColorArgb StretchHslColor(ColorArgb color, const HslScaleParams& scaleParams, const ColorContrast& contrast)
	{
		HSL hsl = ToHsl(color);

		double newSat = contrast.MinSaturation + (hsl.S - scaleParams.SatPair.minValue) * scaleParams.SatScale;
		hsl.S = newSat;

		double newLight = contrast.MinLightness + (hsl.L - scaleParams.LightPair.minValue) * scaleParams.LightScale;
		hsl.L = newLight;

		double newHue = contrast.MinHue + (hsl.H - scaleParams.HuePair.minValue) * scaleParams.HueScale;
		hsl.H = newHue;

		return ToRgb(hsl, color.A);
	}
}