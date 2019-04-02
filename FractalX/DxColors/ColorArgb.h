#pragma once

typedef unsigned __int8 bite;

namespace DxColor
{
	struct ColorArgb
	{
		ColorArgb()
			: A(255)
			, R(0)
			, G(0)
			, B(0)
		{}

		ColorArgb(bite a, bite r, bite g, bite b)
			: A(a)
			, R(r)
			, G(g)
			, B(b)
		{}

		ColorArgb& operator=(const ColorArgb& c);

		bite A = 255;
		bite R = 0;
		bite G = 0;
		bite B = 0;
	};

	

	bool operator==(const ColorArgb& lf, const ColorArgb& rt);

}
