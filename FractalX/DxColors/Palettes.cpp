#include "stdafx.h"
#include "Palettes.h"
#include "ColorPin.h"
#include <atlstr.h>

namespace DxColor
{
	std::shared_ptr<PinPalette> GetRainbow()
	{
		auto rainbow = std::make_shared<PinPalette>();
		rainbow->Name = CString(_T("default Rainbow"));

		ColorPin pin1;
		pin1.Color1 = ColorArgb{ 255, 0, 0, 0 };
		pin1.Index = 0.0;
		pin1.CurveType = ColorCurveType::Normal;
		rainbow->Pins.push_back(pin1);

		ColorPin pin2;
		pin2.Color1 = ColorArgb{ 255, 255, 0, 255 };
		pin2.Index = 0.125;
		pin2.CurveType = ColorCurveType::Normal;
		rainbow->Pins.push_back(pin2);

		ColorPin pin3;
		pin3.Color1 = ColorArgb{ 255, 0, 0, 255 };
		pin3.Index = 0.25;
		pin3.CurveType = ColorCurveType::Normal;
		rainbow->Pins.push_back(pin3);

		ColorPin pin4;
		pin4.Color1 = ColorArgb{ 255, 0, 255, 255 };
		pin4.Index = 0.375;
		pin4.CurveType = ColorCurveType::Normal;
		rainbow->Pins.push_back(pin4);

		ColorPin pin5;
		pin5.Color1 = ColorArgb{ 255, 0, 255, 0 };
		pin5.Index = 0.5;
		pin5.CurveType = ColorCurveType::Normal;
		rainbow->Pins.push_back(pin5);

		ColorPin pin6;
		pin6.Color1 = ColorArgb{ 255, 255, 255, 0 };
		pin6.Index = 0.625;
		pin6.CurveType = ColorCurveType::Normal;
		rainbow->Pins.push_back(pin6);

		ColorPin pin7;
		pin7.Color1 = ColorArgb{ 255, 255, 130, 0 };
		pin7.Index = 0.75;
		pin7.CurveType = ColorCurveType::Normal;
		rainbow->Pins.push_back(pin7);

		ColorPin pin8;
		pin8.Color1 = ColorArgb{ 255, 255, 0, 0 };
		pin8.Index = 0.875;
		pin8.CurveType = ColorCurveType::Normal;
		rainbow->Pins.push_back(pin8);

		ColorPin pin9;
		pin9.Color1 = ColorArgb{ 255, 255, 255, 255 };
		pin9.Index = 1.0;
		pin9.CurveType = ColorCurveType::Normal;
		rainbow->Pins.push_back(pin9);

		return rainbow;
	}

	std::shared_ptr<PinPalette> GetStripes()
	{
		auto stripes = std::make_shared<PinPalette>();
		stripes->Name = CString(_T("Stripes"));

		ColorPin pin1;
		pin1.Color1 = ColorArgb{ 255, 0, 0, 0 };
		pin1.Index = 0.0;
		pin1.CurveType = ColorCurveType::Normal;
		stripes->Pins.push_back(pin1);

		ColorPin pin2;
		pin2.Color1 = ColorArgb{ 255, 255, 0, 0 };
		pin2.Index = 0.04348;
		pin2.CurveType = ColorCurveType::Normal;
		stripes->Pins.push_back(pin2);

		ColorPin pin3;
		pin3.Color1 = ColorArgb{ 255, 0, 0, 255 };
		pin3.Index = 0.08696;
		pin3.CurveType = ColorCurveType::Normal;
		stripes->Pins.push_back(pin3);

		ColorPin pin4;
		pin4.Color1 = ColorArgb{ 255, 255, 0, 0 };
		pin4.Index = 0.13043;
		pin4.CurveType = ColorCurveType::Normal;
		stripes->Pins.push_back(pin4);

		ColorPin pin5;
		pin5.Color1 = ColorArgb{ 255, 0, 0, 255 };
		pin5.Index = 0.17391;
		pin5.CurveType = ColorCurveType::Normal;
		stripes->Pins.push_back(pin5);

		ColorPin pin6;
		pin6.Color1 = ColorArgb{ 255, 255, 0, 0 };
		pin6.Index = 0.21739;
		pin6.CurveType = ColorCurveType::Normal;
		stripes->Pins.push_back(pin6);

		ColorPin pin7;
		pin7.Color1 = ColorArgb{ 255, 0, 0, 255 };
		pin7.Index = 0.26087;
		pin7.CurveType = ColorCurveType::Normal;
		stripes->Pins.push_back(pin7);

		ColorPin pin8;
		pin8.Color1 = ColorArgb{ 255, 255, 0, 0 };
		pin8.Index = 0.30435;
		pin8.CurveType = ColorCurveType::Normal;
		stripes->Pins.push_back(pin8);

		ColorPin pin9;
		pin9.Color1 = ColorArgb{ 255, 0, 0, 255 };
		pin9.Index = 0.34783;
		pin9.CurveType = ColorCurveType::Normal;
		stripes->Pins.push_back(pin9);

		ColorPin pin10;
		pin10.Color1 = ColorArgb{ 255, 255, 0, 0 };
		pin10.Index = 0.39130;
		pin10.CurveType = ColorCurveType::Normal;
		stripes->Pins.push_back(pin10);

		ColorPin pin11;
		pin11.Color1 = ColorArgb{ 255, 0, 0, 255 };
		pin11.Index = 0.43478;
		pin11.CurveType = ColorCurveType::Normal;
		stripes->Pins.push_back(pin11);

		ColorPin pin12;
		pin12.Color1 = ColorArgb{ 255, 255, 0, 0 };
		pin12.Index = 0.47826;
		pin12.CurveType = ColorCurveType::Normal;
		stripes->Pins.push_back(pin12);

		ColorPin pin13;
		pin13.Color1 = ColorArgb{ 255, 0, 0, 255 };
		pin13.Index = 0.52174;
		pin13.CurveType = ColorCurveType::Normal;
		stripes->Pins.push_back(pin13);

		ColorPin pin14;
		pin14.Color1 = ColorArgb{ 255, 255, 0, 0 };
		pin14.Index = 0.56522;
		pin14.CurveType = ColorCurveType::Normal;
		stripes->Pins.push_back(pin14);

		ColorPin pin15;
		pin15.Color1 = ColorArgb{ 255, 0, 0, 255 };
		pin15.Index = 0.6087;
		pin15.CurveType = ColorCurveType::Normal;
		stripes->Pins.push_back(pin15);

		ColorPin pin16;
		pin16.Color1 = ColorArgb{ 255, 255, 0, 0 };
		pin16.Index = 0.65217;
		pin16.CurveType = ColorCurveType::Normal;
		stripes->Pins.push_back(pin16);

		ColorPin pin17;
		pin17.Color1 = ColorArgb{ 255, 0, 0, 255 };
		pin17.Index = 0.6957;
		pin17.CurveType = ColorCurveType::Normal;
		stripes->Pins.push_back(pin17);

		ColorPin pin18;
		pin18.Color1 = ColorArgb{ 255, 255, 0, 0 };
		pin18.Index = 0.73913;
		pin18.CurveType = ColorCurveType::Normal;
		stripes->Pins.push_back(pin18);

		ColorPin pin19;
		pin19.Color1 = ColorArgb{ 255, 0, 0, 255 };
		pin19.Index = 0.78261;
		pin19.CurveType = ColorCurveType::Normal;
		stripes->Pins.push_back(pin19);

		ColorPin pin20;
		pin20.Color1 = ColorArgb{ 255, 255, 0, 0 };
		pin20.Index = 0.82609;
		pin20.CurveType = ColorCurveType::Normal;
		stripes->Pins.push_back(pin20);

		ColorPin pin21;
		pin21.Color1 = ColorArgb{ 255, 0, 0, 255 };
		pin21.Index = 0.86957;
		pin21.CurveType = ColorCurveType::Normal;
		stripes->Pins.push_back(pin21);

		ColorPin pin22;
		pin22.Color1 = ColorArgb{ 255, 255, 0, 0 };
		pin22.Index = 0.91304;
		pin22.CurveType = ColorCurveType::Normal;
		stripes->Pins.push_back(pin22);

		ColorPin pin23;
		pin23.Color1 = ColorArgb{ 255, 0, 0, 255 };
		pin23.Index = 0.95652;
		pin23.CurveType = ColorCurveType::Normal;
		stripes->Pins.push_back(pin23);

		ColorPin pin24;
		pin24.Color1 = ColorArgb{ 255, 255, 255, 255 };
		pin24.Index = 1.0;
		pin24.CurveType = ColorCurveType::Normal;
		stripes->Pins.push_back(pin24);

		return stripes;
	}
}
