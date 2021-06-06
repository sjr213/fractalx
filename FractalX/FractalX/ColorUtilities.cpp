#include "stdafx.h"

#define NOMINMAX

#include <algorithm>
#include "ColorUtilities.h"
#include "ColorPin.h"
#include "ColorContrast.h"
#include "hslUtilities.h"
#include <sstream>

using namespace DxColor;

namespace fx::ColorUtilities
{
	namespace
	{
		const bite ColorRange = 255;
		const double Rounder = 0.49999;

		struct CalcParams
		{
			double StretchRed = 0.0;
			double StretchGreen = 0.0;
			double StretchBlue = 0.0;

			HslScaleParams ScaleParams;
		};
		uint32_t BitesToXmcolor(bite a, bite r, bite g, bite b)
		{
			// XMCOLOR is structured 0xaarrggbb
			// COLORREF is structured 0x00bbggrr
			return (a << 24) | (r << 16) | (g << 8) | b;
		}

		void ContrastColor(bite& r, bite& g, bite& b, const ColorContrast& contrast, const CalcParams& calcParams)
		{
			int red = static_cast<int>((r - contrast.MinContrast[0]) * calcParams.StretchRed + Rounder);
			r = static_cast<bite>(std::min<int>(std::max<int>(0, red), ColorRange));

			int green = static_cast<int>((g - contrast.MinContrast[1]) * calcParams.StretchGreen + Rounder);
			g = static_cast<bite>(std::min<int>(std::max<int>(0, green), ColorRange));

			int blue = static_cast<int>((b - contrast.MinContrast[2]) * calcParams.StretchBlue + Rounder);
			b = static_cast<bite>(std::min<int>(std::max<int>(0, blue), ColorRange));
		}

		void StretchHsl(bite& r, bite& g, bite& b, const ColorContrast& contrast, const HslScaleParams& scaleParams)
		{
			ColorArgb color(255, r, g, b);
			auto newColor = StretchHslColor(color, scaleParams, contrast);

			r = newColor.R;
			g = newColor.G;
			b = newColor.B;
		}

		void AddColor(std::vector<uint32_t>& colors, bite a, bite r, bite g, bite b, const ColorContrast& contrast, const CalcParams& calcParams)
		{
			if (contrast.Mode == ContrastType::Contrast)
				ContrastColor(r, g, b, contrast, calcParams);

			if (contrast.Mode == ContrastType::HSL)
				StretchHsl(r, g, b, contrast, calcParams.ScaleParams);

			colors.push_back(BitesToXmcolor(a, r, g, b));
		}

		bite StretchBite(bite start, bite end, int nStep, int nSteps)
		{
			double eStart = start;
			double eRange = static_cast<double>(end) - static_cast<double>(start);
			double eStretch = static_cast<double>(nStep) / nSteps;
			double result = eStart + eStretch * eRange;
			return static_cast<bite>(std::min<double>(result + 0.5, 255.0));
		}

		bite CurveBite(bite start, bite end, double difference)
		{
			double eStart = start;
			double eRange = static_cast<double>(end) - static_cast<double>(start);
			double result = eStart + difference * eRange;
			return static_cast<bite>(std::min<double>(result + 0.5, 255.0));
		}

		int ColorsRemaining(std::vector<uint32_t>& colors, int nColors)
		{
			return nColors - static_cast<int>(colors.size());
		}

		void FillSpaceBeforeFirstPin(const ColorPin& pin1, std::vector<uint32_t>& colors, int nColors,
			const ColorContrast& contrast, const CalcParams& calcParams)
		{
			if (pin1.Index <= 0.0)
				return;

			int nSteps = static_cast<int>(pin1.Index * nColors);
			nSteps = std::min<int>(nSteps, nColors);

			for (int i = 0; i < nSteps; ++i)
				AddColor(colors, pin1.Color1.A, pin1.Color1.R, pin1.Color1.G, pin1.Color1.B, contrast, calcParams);
		}

		void StretchPinsCurved(const ColorPin& pin1, const ColorPin& pin2, std::vector<uint32_t>& colors, int nSteps,
			const ColorContrast& contrast, const CalcParams& calcParams)
		{
			// add intermediate colors
			for (int i = 1; i < nSteps; ++i)
			{
				double fractionalPosition = static_cast<double>(i) / nSteps;
				double colorDifference = pow(fractionalPosition, pin1.Curve);

				bite a = CurveBite(pin1.Color1.A, pin2.Color1.A, colorDifference);
				bite r = CurveBite(pin1.Color1.R, pin2.Color1.R, colorDifference);
				bite g = CurveBite(pin1.Color1.G, pin2.Color1.G, colorDifference);
				bite b = CurveBite(pin1.Color1.B, pin2.Color1.B, colorDifference);
				AddColor(colors, a, r, g, b, contrast, calcParams);
			}
		}

		bool IsSecondBand(double band1, double band2, double index)
		{
			double currentIndex = band1;

			bool secondBand = false;

			while (currentIndex < index)
			{
				currentIndex += band2;
				if (currentIndex >
					index)
				{
					secondBand = true;
					break;
				}

				currentIndex += band1;
				secondBand = false;
			}

			return secondBand;
		}

		void StretchPinsBanded(const ColorPin& pin1, const ColorPin& pin2, std::vector<uint32_t>& colors, int nSteps,
			const ColorContrast& contrast, const CalcParams& calcParams)
		{
			double band1 = pin1.IndexWidth1;
			double band2 = pin1.IndexWidth2;

			double indexRange = pin2.Index - pin1.Index;

			for (int i = 1; i < nSteps; ++i)
			{
				double currentIndex = i * indexRange / nSteps;

				bool secondBand = IsSecondBand(band1, band2, currentIndex);

				ColorArgb color1 = secondBand ? pin1.Color2 : pin1.Color1;
				ColorArgb color2 = secondBand ? pin2.Color2 : pin2.Color1;

				bite a = StretchBite(color1.A, color2.A, i, nSteps);
				bite r = StretchBite(color1.R, color2.R, i, nSteps);
				bite g = StretchBite(color1.G, color2.G, i, nSteps);
				bite b = StretchBite(color1.B, color2.B, i, nSteps);
				AddColor(colors, a, r, g, b, contrast, calcParams);
			}
		}

		void StretchPinsNormal(const ColorPin& pin1, const ColorPin& pin2, std::vector<uint32_t>& colors, int nSteps,
			const ColorContrast& contrast, const CalcParams& calcParams)
		{
			// add intermediate colors
			for (int i = 1; i < nSteps; ++i)
			{
				bite a = StretchBite(pin1.Color1.A, pin2.Color1.A, i, nSteps);
				bite r = StretchBite(pin1.Color1.R, pin2.Color1.R, i, nSteps);
				bite g = StretchBite(pin1.Color1.G, pin2.Color1.G, i, nSteps);
				bite b = StretchBite(pin1.Color1.B, pin2.Color1.B, i, nSteps);
				AddColor(colors, a, r, g, b, contrast, calcParams);
			}
		}

		void StretchPins(const ColorPin& pin1, const ColorPin& pin2, std::vector<uint32_t>& colors, int nColors,
			bool lastPins, const ColorContrast& contrast, const CalcParams& calcParams)
		{
			int nSteps = static_cast<int>((pin2.Index - pin1.Index) * nColors + 0.5);

			int colorsRemaining = ColorsRemaining(colors, nColors);
			if (colorsRemaining < 1)
				return;

			if (lastPins)
			{
				if (pin2.Index >= 1.0)
					nSteps = colorsRemaining;

				// remove 1 for last color
				--nSteps;
			}

			// always add one color for first pin
			AddColor(colors, pin1.Color1.A, pin1.Color1.R, pin1.Color1.G, pin1.Color1.B, contrast, calcParams);

			if (pin1.CurveType == ColorCurveType::Curve)
				StretchPinsCurved(pin1, pin2, colors, nSteps, contrast, calcParams);
			else if (pin1.CurveType == ColorCurveType::DoubleBand)
				StretchPinsBanded(pin1, pin2, colors, nSteps, contrast, calcParams);
			else
				StretchPinsNormal(pin1, pin2, colors, nSteps, contrast, calcParams);

			if (!lastPins)
				return;

			// add last color
			AddColor(colors, pin2.Color1.A, pin2.Color1.R, pin2.Color1.G, pin2.Color1.B, contrast, calcParams);

			while (static_cast<int>(colors.size()) < nColors)
				AddColor(colors, pin2.Color1.A, pin2.Color1.R, pin2.Color1.G, pin2.Color1.B, contrast, calcParams);
		}

		CalcParams CalculateParams(const PinPalette& palette, const ColorContrast& contrast)
		{
			CalcParams params;

			params.StretchRed = static_cast<double>(ColorRange) / (contrast.MaxContrast[0] - contrast.MinContrast[0]);
			params.StretchGreen = static_cast<double>(ColorRange) / (contrast.MaxContrast[1] - contrast.MinContrast[1]);
			params.StretchBlue = static_cast<double>(ColorRange) / (contrast.MaxContrast[2] - contrast.MinContrast[2]);

			params.ScaleParams = CalculateScaleParams(palette, contrast);

			return params;
		}
	}


	std::vector<uint32_t> CalculatePaletteColors(const PinPalette& palette, int nColors, const ColorContrast& contrast)
	{
		int nPins = static_cast<int>(palette.Pins.size());

		const auto defaultColor = BitesToXmcolor(255, 0, 0, 0);

		std::vector<uint32_t> colors;
		colors.reserve(nColors);

		if (nPins == 0)
		{
			for (int i = 0; i < nColors; ++i)
				colors.push_back(defaultColor);
			return colors;
		}

		CalcParams params = CalculateParams(palette, contrast);

		ColorPin pin1 = palette.Pins.at(0);
		FillSpaceBeforeFirstPin(pin1, colors, nColors, contrast, params);

		if (nPins < 2)
		{
			int nCurrentColors = static_cast<int>(colors.size());
			for (int i = nCurrentColors; i < nColors; ++i)
				colors.push_back(defaultColor);
			return colors;
		}

		for (int i = 1; i < nPins; ++i)
		{
			const ColorPin& pin2 = palette.Pins.at(i);
			StretchPins(pin1, pin2, colors, nColors, i == nPins - 1, contrast, params);
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

	CString GetPaletteFileExtension()
	{
		return _T("pins");
	}

	std::shared_ptr<PinPalette> LoadPalette(const CString& palettePath)
	{
		CFile colorFile;

		// do something about exceptions
		CFileException FileExcept;
		if (!colorFile.Open(palettePath, CFile::modeRead, &FileExcept))
		{
			CString error;
			wchar_t message[100] = { 0 };
			FileExcept.GetErrorMessage(message, 100);

			std::wstringstream ss;
			ss << L"Error opening file : " << palettePath.GetString() << L": " << message;
			AfxMessageBox(ss.str().c_str(), MB_ICONWARNING);

			return nullptr;
		}

		std::shared_ptr<PinPalette> palette = std::make_shared<PinPalette>();

		try
		{
			CArchive ar(&colorFile, CArchive::load);
			DxColor::Serialize(ar, *palette);

			// close up
			ar.Close();
			colorFile.Close();
		}
		catch (CArchiveException* e)
		{
			e->Delete();
			return nullptr;
		}
		catch (CMemoryException* e)
		{
			e->Delete();
			return nullptr;
		}
		catch (CFileException* e)
		{
			e->Delete();
			return nullptr;
		}

		return palette;
	}
}