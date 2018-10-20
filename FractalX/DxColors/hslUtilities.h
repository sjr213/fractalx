#pragma once
#include "ColorArgb.h"

namespace DxColor
{
	struct ColorContrast;
	struct PinPalette;

	enum RgbPart { blue = 0, green, red, total };

	struct HslPair
	{
		double minValue;
		double maxValue;
	};

	struct HslScaleParams
	{
		HslPair HuePair;
		HslPair LightPair;
		HslPair SatPair;

		double HueScale = 1.0;
		double LightScale = 1.0;
		double SatScale = 1.0;
	};

	struct HSL
	{
		HSL() : H(0.0)
			, S(0.0)
			, L(0.0)
		{}

		HSL(double h, double s, double l)
		{
			SetH(h);
			SetS(s);
			SetL(l);
		}

		double H;
		double S;
		double L;

		void SetH(double h)
		{
			while (h < 0.0)
				h += 360.0;

			while (h >= 360.0)
				h -= 360.0;

			H = h;
		}

		void SetS(double s)
		{
			if (s < 0.0)
				s = 0.0;
			if (s > 1.0)
				s = 1.0;
			S = s;
		}

		void SetL(double l)
		{
			if (l < 0.0)
				l = 0.0;
			if (l > 1.0)
				l = 1.0;
			L = l;
		}
	};

	struct Rgb
	{
		double _r;
		double _g;
		double _b;

		void SetR(double r)
		{
			if (r < 0.0)
				r = 0.0;
			if (r > 1.0)
				r = 1.0;

			_r = r;
		}
	
		void SetG(double g)
		{
			if (g < 0.0)
				g = 0.0;
			if (g > 1.0)
				g = 1.0;

			_g = g;
		}

		void SetB(double b)
		{
			if (b < 0.0)
				b = 0.0;
			if (b > 1.0)
				b = 1.0;
			_b = b;
		}

		Rgb(double r = 0.0, double g = 0.0, double b = 0.0)
		{
			SetR(r);
			SetG(g);
			SetB(b);
		}

		ColorArgb ToColorArgb(bite a);
	};

	HSL ToHsl(ColorArgb rgb);

	ColorArgb ToRgb(HSL hsl, bite a);

	HslPair GetMinMaxHue(const PinPalette& palette);

	HslPair GetMinMaxLightness(const PinPalette& palette);

	HslPair GetMinMaxSaturation(const PinPalette& palette);

	HslScaleParams CalculateScaleParams(const PinPalette& palette, const ColorContrast& contrast);

	ColorArgb StretchHslColor(ColorArgb color, const HslScaleParams& scaleParams, const ColorContrast& contrast);
}