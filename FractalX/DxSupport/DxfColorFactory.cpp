
#include "stdafx.h"
#include "DxfColorFactory.h"


namespace DXF
{
	static uint32_t ColorrefToXmcolor(COLORREF cr, uint32_t alpha)
	{
		// XMCOLOR is structured 0xaarrggbb
		// COLORREF is structured 0x00bbggrr
		return (alpha << 24) | (GetRValue(cr) << 16) | (GetGValue(cr) << 8) | GetBValue(cr);
	}

	std::vector<uint32_t> CreateRainbow()
	{
		std::vector<uint32_t> colors;
		colors.push_back(ColorrefToXmcolor(RGB(0, 0, 0), 255));
		colors.push_back(ColorrefToXmcolor(RGB(255, 0, 255), 255));
		colors.push_back(ColorrefToXmcolor(RGB(0, 0, 255), 255));
		colors.push_back(ColorrefToXmcolor(RGB(0, 255, 255), 255));
		colors.push_back(ColorrefToXmcolor(RGB(0, 255, 0), 255));
		colors.push_back(ColorrefToXmcolor(RGB(255, 255, 0), 255));
		colors.push_back(ColorrefToXmcolor(RGB(255, 130, 0), 255));
		colors.push_back(ColorrefToXmcolor(RGB(255, 0, 0), 255));
		colors.push_back(ColorrefToXmcolor(RGB(255, 255, 255), 255));

		return colors;
	}

	std::vector<uint32_t> CreateStripes()
	{
		std::vector<uint32_t> colors;
		colors.push_back(ColorrefToXmcolor(RGB(0, 0, 0), 255));

		colors.push_back(ColorrefToXmcolor(RGB(255, 0, 0), 255));
		colors.push_back(ColorrefToXmcolor(RGB(0, 0, 255), 255));

		colors.push_back(ColorrefToXmcolor(RGB(255, 0, 0), 255));
		colors.push_back(ColorrefToXmcolor(RGB(0, 0, 255), 255));

		colors.push_back(ColorrefToXmcolor(RGB(255, 0, 0), 255));
		colors.push_back(ColorrefToXmcolor(RGB(0, 0, 255), 255));

		colors.push_back(ColorrefToXmcolor(RGB(255, 0, 0), 255));
		colors.push_back(ColorrefToXmcolor(RGB(0, 0, 255), 255));

		colors.push_back(ColorrefToXmcolor(RGB(255, 0, 0), 255));
		colors.push_back(ColorrefToXmcolor(RGB(0, 0, 255), 255));

		colors.push_back(ColorrefToXmcolor(RGB(255, 0, 0), 255));
		colors.push_back(ColorrefToXmcolor(RGB(0, 0, 255), 255));

		colors.push_back(ColorrefToXmcolor(RGB(255, 0, 0), 255));
		colors.push_back(ColorrefToXmcolor(RGB(0, 0, 255), 255));

		colors.push_back(ColorrefToXmcolor(RGB(255, 0, 0), 255));
		colors.push_back(ColorrefToXmcolor(RGB(0, 0, 255), 255));

		colors.push_back(ColorrefToXmcolor(RGB(255, 0, 0), 255));
		colors.push_back(ColorrefToXmcolor(RGB(0, 0, 255), 255));

		colors.push_back(ColorrefToXmcolor(RGB(255, 0, 0), 255));
		colors.push_back(ColorrefToXmcolor(RGB(0, 0, 255), 255));

		colors.push_back(ColorrefToXmcolor(RGB(255, 0, 0), 255));
		colors.push_back(ColorrefToXmcolor(RGB(0, 0, 255), 255));

		colors.push_back(ColorrefToXmcolor(RGB(255, 255, 255), 255));

		return colors;
	}

	int GetNumberOfTextures()
	{
		return 2;
	}

	CString GetTextureName(int i)
	{
		if (i == 0)
			return L"Rainbow";
		if (i == 1)
			return L"Stripes";

		return L"";
	}

	std::vector<uint32_t> GetTexture(int i)
	{
		if (i == 0)
			return CreateRainbow();
		if (i == 1)
			return CreateStripes();

		ASSERT(false);

		return std::vector<uint32_t>();
	}
}