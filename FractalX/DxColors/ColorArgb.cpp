#include "stdafx.h"
#include "ColorArgb.h"


namespace DxColor
{
	bool operator==(const ColorArgb& lf, const ColorArgb& rt)
	{
		return (lf.A == rt.A && lf.R == rt.R && lf.G == rt.G && lf.B == rt.B);
	}

	ColorArgb& ColorArgb::operator=(const ColorArgb& c)
	{
		A = c.A;
		R = c.R;
		G = c.G;
		B = c.B;
		return *this;
	}
}